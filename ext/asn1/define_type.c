/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ruby.h>

#include "asn_application.h"
#include "ENUMERATED.h" /* Required to defined asn_INTEGER_specifics_t */
#include "util.h"
#include "encode.h"
#include "decode.h"
#include "define_type.h"


/******************************************************************************/
/* Forward declarations                                                       */
/******************************************************************************/
static VALUE	 define_type(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_base, char *descriptor_symbol);
static VALUE	 get_schema_stub(VALUE schema_root, VALUE candidate_type, char *descriptor_symbol, char *name);
static VALUE	 get_type_stub(VALUE type_root, asn_TYPE_descriptor_t *td, char *name);
static void		 define_sequence(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
static void		 define_sequence_of(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
static void		 define_choice(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
static void		 define_enumerated(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
static void		 finalize_schema(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_class, asn_TYPE_descriptor_t *td);
static void	 	 set_encoder_and_decoder(VALUE schema_class, int base_type);
static VALUE	 create_attribute_hash(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_class, struct asn_TYPE_member_s *element);
static char		*class_name(asn_TYPE_descriptor_t *td);
static VALUE	 get_primitive_type(int t);
static VALUE	 lookup_type(asn_TYPE_descriptor_t *td);


/******************************************************************************/
/* Externals                                                                  */
/******************************************************************************/
extern asn_TYPE_descriptor_t *get_type_descriptor(const char *name);
extern int	is_anonymous(asn_TYPE_descriptor_t *td);

/******************************************************************************/
/* define_type_from_ruby													  */
/******************************************************************************/
VALUE
define_type_from_ruby(VALUE class, VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE symbol)
{
	char  *symbol_str = RSTRING_PTR(symbol);
	return define_type(schema_root, schema_base, type_root, type_root, symbol_str);
}


/******************************************************************************/
/* define_type                                                                */
/* Returns a ruby class associated with a corresponding ASN.1 type            */
/******************************************************************************/
static VALUE
define_type(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_base, char *descriptor_symbol)
{
	VALUE type_class;
	VALUE schema_class;
	VALUE symbol_to_schema;
	int	  structured  = 0;
	char *name = NULL;

	asn_TYPE_descriptor_t *td = get_type_descriptor(descriptor_symbol);
	if (!td)
	{
		rb_raise(rb_eException, "Can't find symbol");
	}

	if (td->generated == 0)
	{
		return get_primitive_type(td->base_type);
	}

	name = class_name(td);

	if (td->inherit != NULL)
	{
		(*td->inherit)(td);
	}

	type_class		 = get_type_stub(type_base, td, name);
	schema_class	 = get_schema_stub(schema_base, type_class, descriptor_symbol, name);
	symbol_to_schema = rb_const_get(schema_root, rb_intern("SYMBOL_TO_SCHEMA"));
	rb_hash_aset(symbol_to_schema, rb_str_new2(descriptor_symbol), schema_class);

	/*
	 * Define type, based on base ASN.1 type.
	 */
	switch(td->base_type)
	{
		case ASN1_TYPE_SEQUENCE :
			define_sequence(schema_base, type_class, td);
			structured = 1;
			break;

		case ASN1_TYPE_SEQUENCE_OF :
			define_sequence_of(schema_base, type_class, td);
			structured = 1;
			break;

		case ASN1_TYPE_CHOICE :
			define_choice(schema_base, type_class, td);
			structured = 1;
			break;

		case ASN1_TYPE_ENUMERATED :
			define_enumerated(schema_base, type_class, td);
			structured = 1;
			break;

		default :
			free((void *)name);
			rb_raise(rb_eException, "Can't create type");
			break;
	}

	/*
	 * Create reference to schema class from type class
	 */
	rb_define_const(type_class, "ASN1_TYPE", rb_str_new2(descriptor_symbol));

	/*
	 * Make schema class and type class refer to each other.
	 */
	rb_define_const(type_class, "ASN1_SCHEMA", schema_class);

	finalize_schema(schema_root, schema_class, type_root, type_class, td);

	free((void *)name);

	return type_class;
}


/******************************************************************************/
/* get_schema_stub	 		                                                  */
/* Find or create a schema class representing an ASN.1 class.				  */
/******************************************************************************/
static VALUE
get_schema_stub(VALUE schema_root, VALUE candidate_type, char *descriptor_symbol, char *name)
{
	VALUE schema_class;
	int   i;

	asn_TYPE_descriptor_t *td = get_type_descriptor(descriptor_symbol);
	if (!td)
	{
		rb_raise(rb_eException, "Can't find symbol %s", descriptor_symbol);
	}
	
	schema_class = rb_define_class_under(schema_root, name, rb_cObject);

	/*
	 * Define schema class
	 */
	rb_define_const(schema_class, "ANONYMOUS",      INT2FIX(td->anonymous));
	rb_define_const(schema_class, "PRIMITIVE",      INT2FIX(td->generated));
	rb_define_const(schema_class, "ASN1_TYPE",      rb_str_new2(descriptor_symbol));
	rb_define_const(schema_class, "CANDIDATE_TYPE", candidate_type);

	return schema_class;
}


/******************************************************************************/
/* get_type_stub                                                              */
/******************************************************************************/
static VALUE
get_type_stub(VALUE type_root, asn_TYPE_descriptor_t *td, char *name)
{
	return rb_define_class_under(type_root, name, rb_cObject);
}


/******************************************************************************/
/* define_sequence                                                           */
/******************************************************************************/
static void
define_sequence(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td)
{
	int	i;

	for (i = 0; i < td->elements_count; i++)
	{
		if (strlen(td->elements[i].name) > 0)
		{
			rb_define_attr(type_class, td->elements[i].name, 1, 1);
		}
	}
}


/******************************************************************************/
/* define_seqeuence_of														  */
/******************************************************************************/
static void
define_sequence_of(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td)
{
	return; /*  type_class;	/* XXXXX - Noop */
}


/******************************************************************************/
/* define_choice                                                              */
/******************************************************************************/
static void
define_choice(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td)
{
	ID asn_module_id	  		 = rb_intern("Asn1");
    ID asn_accessorize_module_id = rb_intern("Accessorize");

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

	return;
}


/******************************************************************************/
/* define_enumerated														  */
/******************************************************************************/
static void
define_enumerated(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td)
{
	asn_INTEGER_specifics_t *specifics = (asn_INTEGER_specifics_t *)td->specifics;

	ID asn_module_id	  	   = rb_intern("Asn1");
    ID asn_enumerize_module_id = rb_intern("Enumerize");

	VALUE params	 = rb_ary_new();

	VALUE asn_module	   = rb_const_get(rb_cObject, asn_module_id);
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

	return;
}


/******************************************************************************/
/* finalize_schema															  */
/* Everything that can't be done while we're worried about recursive		  */
/* definitions.  Currently, just attaches a candidate type to a schema.		  */
/******************************************************************************/
static void
finalize_schema(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_base, asn_TYPE_descriptor_t *td)
{
	VALUE type_hash = rb_hash_new();
	int i;

	/* XXXXX - Probably need to handle SET OF differently. */
	if (td->base_type == ASN1_TYPE_SEQUENCE_OF || td->base_type == ASN1_TYPE_SET_OF)
	{
		asn_TYPE_descriptor_t *td2 = td->elements->type;
		VALUE collection_type = lookup_type(td2);
		if (collection_type == Qnil)
		{
			collection_type = define_type(schema_root, schema_root, type_root, type_root, td2->symbol);
		}

		rb_define_const(schema_base, "COLLECTION", INT2FIX(1));
		rb_define_const(schema_base, "COLLECTION_TYPE", collection_type);
	}
	else
	{
		rb_define_const(schema_base, "COLLECTION", INT2FIX(0));
		rb_define_const(schema_base, "COLLECTION_TYPE", Qnil);

		for (i = 0; i < td->elements_count; i++)
		{
			if (strlen(td->elements[i].name) > 0)
			{
				VALUE attributes = create_attribute_hash(schema_root, schema_base, type_root, type_base, &td->elements[i]);
				rb_hash_aset(type_hash, rb_str_new2(td->elements[i].name), attributes);
			}
		}
	}

	rb_define_const(schema_base, "ATTRIBUTES", type_hash);

	set_encoder_and_decoder(schema_base, td->base_type);
}


/******************************************************************************/
/* set_encoder_and_decoder													  */
/******************************************************************************/
static void
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


/******************************************************************************/
/* create_attribute_hash                                                      */
/******************************************************************************/
static VALUE
create_attribute_hash(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_base, struct asn_TYPE_member_s *element)
{
	VALUE attribute_hash = rb_hash_new();
	VALUE type			 = lookup_type(element->type);

	if (type == Qnil)
	{
		VALUE schema;

		/* XXXXX - fix this */
 		if (is_anonymous(element->type))
		{
				type = define_type(schema_root, schema_base, type_root, type_base, element->type->symbol);
		}
		else
		{
				type = define_type(schema_root, schema_root, type_root, type_root, element->type->symbol);
		}
	}

	rb_hash_aset(attribute_hash, rb_str_new2("type"),      type);
	rb_hash_aset(attribute_hash, rb_str_new2("base_type"), rb_str_new2("NOT WORKING"));
	rb_hash_aset(attribute_hash, rb_str_new2("optional"),  INT2FIX(element->optional));

	return attribute_hash;
}


/******************************************************************************/
/* class_name																  */
/******************************************************************************/
static char *anon_prefix = "Anon";

static char *
class_name(asn_TYPE_descriptor_t *td)
{
	char *name = NULL;

	if (is_anonymous(td))
	{
		int len = strlen(anon_prefix) + strlen(td->name) + 1;
		name    = (char *)calloc(len, sizeof(char));
		sprintf(name, "%s%s", anon_prefix, td->name);
	}
	else
	{
		name = strdup(td->name);
	}

	return name;
}


/******************************************************************************/
/* get_primitive_type														  */
/******************************************************************************/
static VALUE
get_primitive_type(int t)
{
	VALUE type;

	switch(t)
	{
		case ASN1_TYPE_NULL :
			type = rb_cNilClass;
			break;

		case ASN1_TYPE_INTEGER :
			type = rb_cFixnum;
			break;

		case ASN1_TYPE_IA5String :
		case ASN1_TYPE_UTF8String :
			type = rb_cString;
			break;

		case ASN1_TYPE_REAL :
			type = rb_cFloat;
			break;

		default :
			rb_raise(rb_eStandardError, "type_for_primitive(): can't find primitive type");
			break;
	}

	return type;
}


/******************************************************************************/
/* lookup_type                                                                */
/******************************************************************************/
static VALUE
lookup_type(asn_TYPE_descriptor_t *td)
{
	VALUE type;

	if (td->generated == 0)
	{
		return get_primitive_type(td->base_type);
	}
	else
	{
		VALUE schema_class = get_schema_from_td_string(td->symbol);
		if (schema_class == Qnil) {
			type = Qnil;
		}
		else
		{
			VALUE candidate = rb_intern("CANDIDATE_TYPE");
			type = rb_const_get(schema_class, candidate);
		}
	}
	
	return type;
}


/******************************************************************************/
/* asn1_get_td_from_schema													  */
/******************************************************************************/
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

	td = get_type_descriptor(tdName);

	return td;
}


/******************************************************************************/
/* get_schema_from_td_string												  */
/******************************************************************************/
VALUE
get_schema_from_td_string(char *symbol)
{
	VALUE asn_module_id		= rb_intern("Asn1");
	VALUE schema_class_id	= rb_intern("Schema");
	VALUE asn_module		= rb_const_get(rb_cObject, asn_module_id);
	VALUE schema_class		= rb_const_get(asn_module, schema_class_id);
	VALUE symbol_to_schema	= rb_const_get(schema_class, rb_intern("SYMBOL_TO_SCHEMA"));
	VALUE schema			= rb_hash_lookup(symbol_to_schema, rb_str_new2(symbol));

	return schema;
}
