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
void	define_type(VALUE schema_root, VALUE type_root, char *descriptor_symbol);
VALUE   lookup_type(struct asn_TYPE_member_s *tms);
int		consumeBytes(const void *buffer, size_t size, void *application_specific_key);

/*
 * Externals
 */
extern asn_TYPE_descriptor_t asn_DEF_INTEGER;
extern VALUE encode_sequence(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_sequence(VALUE class, VALUE encoder, VALUE sequence);


/******************************************************************************/
/* define_type                                                                */
/* Returns a ruby class associated with a corresponding ASN.1 type            */
/******************************************************************************/
void
define_type(VALUE schema_root, VALUE type_root, char *descriptor_symbol)
{
	int   i;
	VALUE type_class, schema_class;
	VALUE type_hash = rb_hash_new();
	asn_TYPE_descriptor_t *td = get_type_descriptor(descriptor_symbol);

	type_class   = rb_define_class_under(type_root,   td->name, rb_cObject);
	schema_class = rb_define_class_under(schema_root, td->name, rb_cObject);

	/*
	 * 1. Create reference to schema class from type class
	 */
	rb_define_const(type_class, "ASN1_SCHEMA", schema_class);
	rb_define_const(type_class, "ASN1_TYPE",   rb_str_new2("asn_DEF_SimpleSequence"));

	/*
	 * 2. Define schema class
	 */
	rb_define_const(schema_class, "ANONYMOUS",      INT2FIX(td->anonymous));
	rb_define_const(schema_class, "PRIMITIVE",      INT2FIX(td->generated));
	rb_define_const(schema_class, "CANDIDATE_TYPE", type_class);
	rb_define_const(schema_class, "ASN1_TYPE",      rb_str_new2("asn_DEF_SimpleSequence"));

	rb_define_singleton_method(schema_class, "encode", encode_sequence, 2);
	rb_define_singleton_method(schema_class, "decode", decode_sequence, 2);

	/*
	 * 3. Traverse type elements
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		if (strlen(td->elements[i].name) > 0)
		{
			rb_define_attr(type_class, td->elements[i].name, 1, 1);
			rb_hash_aset(type_hash, rb_str_new2(td->elements[i].name), lookup_type(&td->elements[i]));
		}
	}

	rb_define_const(schema_class, "ATTRIBUTES", type_hash);
}

/*
 * get_type_descriptor
 *
 * Find a symbol, produced by asn1c, that defines an ASN.1 type
 *
 * In particular, the symbol represents an asn1c type def.
 *
 * How to identify symbols that are asn1c type defs is outside
 * the scope of this method.
 */
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

/* ************************************************************************** */
/* traverse_type                                                              */
/* _traverse_type                                                             */
/* ************************************************************************** */
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

/*
 * lookup_type
 */
VALUE
lookup_type(struct asn_TYPE_member_s *tms)
{
	return rb_str_new2(tms->type->name);
}


/*
 * consume_bytes
 */
int
consumeBytes(const void *buffer, size_t size, void *application_specific_key)
{
	bufferInfo_t *bufferInfo = (bufferInfo_t *)application_specific_key;

	memcpy((void *)(bufferInfo->buffer + bufferInfo->offset), buffer, size);

	bufferInfo->offset += size;

	return 0;
}

