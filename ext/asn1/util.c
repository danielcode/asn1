#include <stdlib.h>
#include <string.h>
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
int		validate_encoding(VALUE encoding);
int		is_undefined(VALUE v, int base_type);
VALUE	get_optional_value(VALUE v, asn_TYPE_member_t *member);
VALUE	instance_of_undefined(void);

asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);

/*
 * Externals
 */
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


/*
 * validate_encoding
 */
int
validate_encoding(VALUE encoding)
{
	int val;

	ID		asn_module_id, asn_error_module_id, asn_encoder_error_id;
	VALUE	asn_module,    asn_error_module,    asn_encoder_error;
	VALUE	error, argv[0];

	VALUE  symbol_as_string = rb_funcall(encoding, rb_intern("to_s"), 0, rb_ary_new2(0));
	char  *encoder = RSTRING_PTR(symbol_as_string);

	val = strcmp(encoder, "ber");
	if (val == 0)
	{
		return 1;
	}
	else if (strcmp(encoder, "der") == 0)
	{
		return 1;
	}
	else if (strcmp(encoder, "per") == 0)
	{
		return 1;
	}
	else if (strcmp(encoder, "xer") == 0)
	{
		return 1;
	}

	asn_module_id 			= rb_intern("Asn1");
	asn_error_module_id		= rb_intern("Error");
	asn_encoder_error_id	= rb_intern("EncoderTypeError");

	asn_module			= rb_const_get(rb_cObject, asn_module_id);
	asn_error_module	= rb_const_get(asn_module, asn_error_module_id);
	asn_encoder_error	= rb_const_get(asn_error_module, asn_encoder_error_id);

	rb_raise(error, "Invalid encoding");

	return 0;
}


asn_TYPE_descriptor_t *
asn1_get_td_from_schema(VALUE class)
{
	asn_TYPE_descriptor_t *td = NULL;
	char    *tdName = NULL;
	VALUE    vTD;
	int      length;

	/*
	 * Find type descriptor associated with class
	 */
	vTD    = rb_const_get(class, rb_intern("ASN1_TYPE"));
	length = RSTRING_LEN(vTD);
	tdName = RSTRING_PTR(vTD);

	return get_type_descriptor(tdName);
}


int
is_undefined(VALUE v, int base_type)
{
	const char *c = rb_obj_classname(v);
	if (strcmp(c, "Asn1::Undefined") == 0)
	{
		return 1;
	}

	if ((TYPE(v) == T_NIL) && (base_type != ASN1_TYPE_NULL))
	{
		return 1;
	}


	return 0;
}

VALUE
get_optional_value(VALUE v, asn_TYPE_member_t *member)
{
	VALUE memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));

	if (is_undefined(v, member->type->base_type) && !member->optional)
	{
		rb_raise(rb_eStandardError, "Value absent, but not optional");
	}

	return memb;
}

VALUE
instance_of_undefined(void)
{
	ID asn_module_id	  = rb_intern("Asn1");
    ID undefined_class_id = rb_intern("Undefined");

	VALUE asn_module	  = rb_const_get(rb_cObject, asn_module_id);
	VALUE undefined_class = rb_const_get(asn_module, undefined_class_id);

	return rb_funcall(undefined_class, rb_intern("instance"), 0, rb_ary_new2(0));
}
