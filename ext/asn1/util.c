/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dlfcn.h>	/* For dlopen, etc. */
#include <ruby.h>

#include "asn_application.h"


/******************************************************************************/
/* Forward declarations                                                       */
/******************************************************************************/
asn_TYPE_descriptor_t *get_type_descriptor(const char *name);

VALUE	 traverse_type(VALUE class, VALUE name);
VALUE	 _traverse_type(asn_TYPE_descriptor_t *td);
int		 is_anonymous(asn_TYPE_descriptor_t *td);


/******************************************************************************/
/* get_type_descriptor														  */
/*																			  */
/* Find a symbol, produced by asn1c, that defines an ASN.1 type				  */
/*																			  */
/* In particular, the symbol represents an asn1c type def.					  */
/*																			  */
/* How to identify symbols that are asn1c type defs is outside				  */
/* the scope of this method.												  */
/******************************************************************************/
asn_TYPE_descriptor_t *
get_type_descriptor(const char *symbol)
{
	asn_TYPE_descriptor_t *td = NULL;
	void *sym = NULL;

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


/******************************************************************************/
/* traverse_type                                                              */
/* _traverse_type                                                             */
/******************************************************************************/
VALUE
traverse_type(VALUE class, VALUE name)
{
	const char *symbol = StringValuePtr(name);
	asn_TYPE_descriptor_t *td = get_type_descriptor(symbol);
	if (!td)
	{
		rb_raise(rb_eException, "Can't find symbol");
	}

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


/******************************************************************************/
/* is_anonymous																  */
/******************************************************************************/
int
is_anonymous(asn_TYPE_descriptor_t *td)
{
	if (td->name == NULL || strlen(td->name) == 0)
	{
		return 1;
	}

	if (islower(td->name[0]))
	{
		return 1;
	}

	return 0;
}
