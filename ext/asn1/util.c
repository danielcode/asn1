#include <stdlib.h>
#include <dlfcn.h>	/* For dlopen, etc. */
#include <ruby.h>

#include "asn_application.h"
#include "util.h"

/*
 * Forward declarations
 */
asn_TYPE_descriptor_t *get_type_descriptor(const char *name);

VALUE   traverse_type(VALUE class, VALUE name);
VALUE   _traverse_type(asn_TYPE_descriptor_t *td);
VALUE	define_types(VALUE module_or_class, char *descriptor_symbol);

int		consumeBytes(const void *buffer, size_t size, void *application_specific_key);

/*
 * Externals
 */
extern asn_TYPE_descriptor_t asn_DEF_INTEGER;


asn_TYPE_descriptor_t *
get_type_descriptor(const char *symbol)
{
	asn_TYPE_descriptor_t *td = NULL;
	void *sym	 = NULL;

	/*
	 * Find an interesting symbol
	 */
	sym = dlsym(RTLD_DEFAULT, symbol); // "asn_DEF_AnonSequence");
	if (!sym)
	{
		rb_raise(rb_eException, "Can't find symbol");
	}
	td = (asn_TYPE_descriptor_t *)sym;

	return td;
}

VALUE
traverse_type(VALUE class, VALUE name)
{
	const char *symbol = StringValuePtr(name);
	asn_TYPE_descriptor_t *td = get_type_descriptor(symbol);

	/*
	 * Go type spelunking
	 */
	return _traverse_type(td);
}


VALUE
_traverse_type(asn_TYPE_descriptor_t *td)
{
	int i;

	VALUE hsh = rb_hash_new();

	rb_hash_aset(hsh, rb_str_new2("name"), rb_str_new2(td->name));

	if (td->elements_count > 0)
	{
		VALUE elements = rb_ary_new();

		for (i = 0; i < td->elements_count; i++)
		{
			VALUE element = rb_hash_new();

			if (strlen(td->elements[i].name) > 0)
			{
				rb_hash_aset(element, rb_str_new2("instance_name"), rb_str_new2(td->elements[i].name));
			}

			rb_hash_aset(element, rb_str_new2("type"), _traverse_type(td->elements[i].type));

			(void)rb_ary_push(elements, element);
		}

		rb_hash_aset(hsh, rb_str_new2("elements"), elements);
	}

	return hsh;
}

VALUE
define_types(VALUE module_or_class, char *descriptor_symbol)
{
	VALUE class;
	int i;

	asn_TYPE_descriptor_t *td = get_type_descriptor(descriptor_symbol);

	class = rb_define_class_under(module_or_class, td->name, rb_cObject);

	for (i = 0; i < td->elements_count; i++)
	{
		if (strlen(td->elements[i].name) > 0)
		{
			rb_define_attr(class, td->elements[i].name, 1, 1);
		}
	}

	return class;
}

int
consumeBytes(const void *buffer, size_t size, void *application_specific_key)
{
	bufferInfo_t *bufferInfo = (bufferInfo_t *)application_specific_key;

	memcpy((void *)(bufferInfo->buffer + bufferInfo->offset), buffer, size);

	bufferInfo->offset += size;

	return 0;
}
