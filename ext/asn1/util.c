/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>	/* For dlopen, etc. */
#include <ruby.h>

#include "asn_application.h"
#include "util.h"

#include "ENUMERATED.h"

/******************************************************************************/
/* Forward declarations                                                       */
/******************************************************************************/
asn_TYPE_descriptor_t *get_type_descriptor(const char *name);

VALUE   traverse_type(VALUE class, VALUE name);
VALUE   _traverse_type(asn_TYPE_descriptor_t *td);
VALUE	create_attribute_hash(struct asn_TYPE_member_s *element);
void	define_type(VALUE schema_root, VALUE type_root, char *descriptor_symbol);
VALUE	define_sequence(VALUE type_root, asn_TYPE_descriptor_t *td);
VALUE	define_sequence_of(VALUE type_root, asn_TYPE_descriptor_t *td);
VALUE	define_choice(VALUE type_root, asn_TYPE_descriptor_t *td);
VALUE	define_enumerated(VALUE type_root, asn_TYPE_descriptor_t *td);
VALUE	find_or_create_schema(VALUE schema_root, char *descriptor_symbol, VALUE candidate_type);
VALUE	ruby_class_from_asn1_type(asn_TYPE_descriptor_t *td);
void	set_encoder_and_decoder(VALUE schema_class, int base_type);
VALUE   lookup_type(struct asn_TYPE_member_s *tms);
int		consumeBytes(const void *buffer, size_t size, void *application_specific_key);
int		validate_encoding(VALUE encoding);
int		is_undefined(VALUE v, int base_type);
VALUE	get_optional_value(VALUE v, asn_TYPE_member_t *member);
VALUE	instance_of_undefined(void);

asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);

/******************************************************************************/
/* Externals                                                                  */
/******************************************************************************/
extern VALUE encode_sequence(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_sequence(VALUE class, VALUE encoder, VALUE sequence);
extern VALUE encode_sequence_of(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_sequence_of(VALUE class, VALUE encoder, VALUE sequence);
extern VALUE encode_choice(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_choice(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE encode_enumerated(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_enumerated(VALUE class, VALUE encoder, VALUE byte_string);


/******************************************************************************/
/* define_type                                                                */
/* Returns a ruby class associated with a corresponding ASN.1 type            */
/* XXXXX - need to handle recursively defined types                           */
/******************************************************************************/
void
define_type(VALUE schema_root, VALUE type_root, char *descriptor_symbol)
{
	int   i;
	VALUE type_class, schema_class;
	asn_TYPE_descriptor_t *td = get_type_descriptor(descriptor_symbol);

	/*
	 * 1. 
	 */
	switch(td->base_type)
	{
		case ASN1_TYPE_SEQUENCE :
			type_class = define_sequence(type_root, td);
			break;

		case ASN1_TYPE_SEQUENCE_OF :
			type_class = define_sequence_of(type_root, td);
			break;

		case ASN1_TYPE_CHOICE :
			type_class = define_choice(type_root, td);
			break;

		case ASN1_TYPE_ENUMERATED :
			type_class = define_enumerated(type_root, td);
			break;

		default :
			rb_raise(rb_eException, "Can't create type");
			break;
	}

	/*
	 * 2. Create reference to schema class from type class
	 */
	rb_define_const(type_class, "ASN1_TYPE", rb_str_new2(descriptor_symbol));

	/*
	 * 3. Attach schema to type
	 */
	schema_class = find_or_create_schema(schema_root, descriptor_symbol, type_class);
	rb_define_const(type_class, "ASN1_SCHEMA", schema_class);
}

/******************************************************************************/
/* define_seqeuence                                                           */
/******************************************************************************/
VALUE
define_sequence(VALUE type_root, asn_TYPE_descriptor_t *td)
{
	VALUE type_class = rb_define_class_under(type_root, td->name, rb_cObject);
	int	i;

	for (i = 0; i < td->elements_count; i++)
	{
		if (strlen(td->elements[i].name) > 0)
		{
			rb_define_attr(type_class, td->elements[i].name, 1, 1);
		}
	}

	return type_class;
}

/******************************************************************************/
/* define_seqeuence_of														  */
/******************************************************************************/
VALUE
define_sequence_of(VALUE type_root, asn_TYPE_descriptor_t *td)
{
	VALUE type_class = rb_define_class_under(type_root, td->name, rb_cArray);

	return type_class;
}

/******************************************************************************/
/* define_choice                                                              */
/******************************************************************************/
VALUE
define_choice(VALUE type_root, asn_TYPE_descriptor_t *td)
{
	ID asn_module_id	  		 = rb_intern("Asn1");
    ID asn_accessorize_module_id = rb_intern("Accessorize");

	VALUE type_class = rb_define_class_under(type_root, td->name, rb_cObject);
	VALUE params	 = rb_ary_new();

	VALUE asn_module	  	 = rb_const_get(rb_cObject, asn_module_id);
	VALUE accessorize_module = rb_const_get(asn_module, asn_accessorize_module_id);

	int i;

	rb_extend_object(type_class, accessorize_module);

	for (i = 0; i < td->elements_count; i++)
	{
		if (strlen(td->elements[i].name) > 0)
		{
			VALUE v = rb_str_new2(td->elements[i].name);
			(void)rb_ary_push(params, v);
		}
	}

	rb_funcall(type_class, rb_intern("accessorize"), 1, params);

	return type_class;
}

/******************************************************************************/
/* define_enumerated														  */
/******************************************************************************/
VALUE
define_enumerated(VALUE type_root, asn_TYPE_descriptor_t *td)
{
	asn_INTEGER_specifics_t *specifics = (asn_INTEGER_specifics_t *)td->specifics;

	ID asn_module_id	  	   = rb_intern("Asn1");
    ID asn_enumerize_module_id = rb_intern("Enumerize");

	VALUE type_class = rb_define_class_under(type_root, td->name, rb_cObject);
	VALUE params	 = rb_ary_new();

	VALUE asn_module	  	 = rb_const_get(rb_cObject, asn_module_id);
	VALUE enumerize_module = rb_const_get(asn_module, asn_enumerize_module_id);

	int i;

	rb_extend_object(type_class, enumerize_module);

	for (i = 0; i < specifics->map_count; i++)
	{
		VALUE elem = rb_ary_new();
		asn_INTEGER_enum_map_t *enum_map = &specifics->value2enum[i];

		
		(void)rb_ary_push(elem, rb_str_new(enum_map->enum_name, enum_map->enum_len));
		(void)rb_ary_push(elem, INT2FIX(enum_map->nat_value));

		(void)rb_ary_push(params, elem);
	}

	rb_funcall(type_class, rb_intern("enumerize"), 1, params);

	return type_class;
}

/******************************************************************************/
/* find_or_create_schema                                                      */
/* Returns a ruby class associated with a corresponding ASN.1 type            */
/******************************************************************************/
VALUE
find_or_create_schema(VALUE schema_root, char *descriptor_symbol, VALUE candidate_type)
{
	VALUE schema_class;
	VALUE type_hash = rb_hash_new();
	int   i;

	asn_TYPE_descriptor_t *td = get_type_descriptor(descriptor_symbol);
	schema_class = rb_define_class_under(schema_root, td->name, rb_cObject);

	/*
	 * Define schema class
	 */
	rb_define_const(schema_class, "ANONYMOUS",      INT2FIX(td->anonymous));
	rb_define_const(schema_class, "PRIMITIVE",      INT2FIX(td->generated));
	rb_define_const(schema_class, "CANDIDATE_TYPE", candidate_type);
	rb_define_const(schema_class, "ASN1_TYPE",      rb_str_new2(descriptor_symbol));

	/* XXXXX - Probably need to handle SET OF differently. */
	if (td->base_type == ASN1_TYPE_SEQUENCE_OF || td->base_type == ASN1_TYPE_SET_OF)
	{
		asn_TYPE_descriptor_t *td2 = td->elements->type;
		VALUE collection_type = ruby_class_from_asn1_type(td2);

		rb_define_const(schema_class, "COLLECTION", INT2FIX(1));
		rb_define_const(schema_class, "COLLECTION_TYPE", collection_type);
	}
	else
	{
		rb_define_const(schema_class, "COLLECTION", INT2FIX(0));
		rb_define_const(schema_class, "COLLECTION_TYPE", Qnil);

		for (i = 0; i < td->elements_count; i++)
		{
			if (strlen(td->elements[i].name) > 0)
			{
				rb_hash_aset(type_hash, rb_str_new2(td->elements[i].name),
							 create_attribute_hash(&td->elements[i]));
			}
		}
	}

	rb_define_const(schema_class, "ATTRIBUTES", type_hash);

	set_encoder_and_decoder(schema_class, td->base_type);

	return schema_class;
}


/******************************************************************************/
/* ruby_class_from_asn1_type												  */
/******************************************************************************/
VALUE
ruby_class_from_asn1_type(asn_TYPE_descriptor_t *td)
{
	VALUE type;

	switch(td->base_type)
	{
		case ASN1_TYPE_INTEGER :
			type = rb_cFixnum;
			break;

		default :
			rb_raise(rb_eException, "ruby_class_from_asn1_type(): can't locate type");
			break;
	}

	return type;
}


/******************************************************************************/
/* set_encoder_and_decoder													  */
/******************************************************************************/
void
set_encoder_and_decoder(VALUE schema_class, int base_type)
{
	switch(base_type) {
		case ASN1_TYPE_SEQUENCE :
			rb_define_singleton_method(schema_class, "encode", encode_sequence, 2);
			rb_define_singleton_method(schema_class, "decode", decode_sequence, 2);
			break;

		case ASN1_TYPE_SEQUENCE_OF :
			rb_define_singleton_method(schema_class, "encode", encode_sequence_of, 2);
			rb_define_singleton_method(schema_class, "decode", decode_sequence_of, 2);
			break;

		case ASN1_TYPE_CHOICE :
			rb_define_singleton_method(schema_class, "encode", encode_choice, 2);
			rb_define_singleton_method(schema_class, "decode", decode_choice, 2);
			break;

		case ASN1_TYPE_ENUMERATED :
			rb_define_singleton_method(schema_class, "encode", encode_enumerated, 2);
			rb_define_singleton_method(schema_class, "decode", decode_enumerated, 2);
			break;

		default :
			rb_raise(rb_eException, "set_encoder_and_decoder(): can't find type");
			break;
	}
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

/******************************************************************************/
/* traverse_type                                                              */
/* _traverse_type                                                             */
/******************************************************************************/
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

/******************************************************************************/
/* create_attribute_hash                                                      */
/******************************************************************************/
VALUE
create_attribute_hash(struct asn_TYPE_member_s *element)
{
	VALUE attribute_hash = rb_hash_new();
	rb_hash_aset(attribute_hash, rb_str_new2("type"),      lookup_type(element));
	rb_hash_aset(attribute_hash, rb_str_new2("base_type"), rb_str_new2("NOT WORKING"));
	rb_hash_aset(attribute_hash, rb_str_new2("optional"),  INT2FIX(element->optional));

	return attribute_hash;
}


/******************************************************************************/
/* lookup_type                                                                */
/******************************************************************************/
VALUE
lookup_type(struct asn_TYPE_member_s *tms)
{
	return rb_str_new2(tms->type->name);
}


/******************************************************************************/
/* consume_bytes                                                              */
/******************************************************************************/
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
