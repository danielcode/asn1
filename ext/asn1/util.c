/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dlfcn.h>	/* For dlopen, etc. */
#include <ruby.h>

#include "asn_application.h"
#include "util.h"


/******************************************************************************/
/* Forward declarations                                                       */
/******************************************************************************/
asn_TYPE_descriptor_t *get_type_descriptor(const char *name);

VALUE	 traverse_type(VALUE class, VALUE name);
VALUE	 _traverse_type(asn_TYPE_descriptor_t *td);
VALUE	 create_attribute_hash(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_class, struct asn_TYPE_member_s *element);
VALUE	 define_type(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_base, char *descriptor_symbol);
VALUE	 define_type_from_ruby(VALUE class, VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE symbol);
void	 define_sequence(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
void	 define_sequence_of(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
void	 define_choice(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
void	 define_enumerated(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td);
VALUE	 get_schema_stub(VALUE schema_root, VALUE candidate_type, char *descriptor_symbol, char *name);
VALUE	 get_type_stub(VALUE type_root, asn_TYPE_descriptor_t *td, char *name);
void	 finalize_schema(VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE type_class, asn_TYPE_descriptor_t *td);
void	 set_encoder_and_decoder(VALUE schema_class, int base_type);
VALUE	 lookup_type(asn_TYPE_descriptor_t *td);
int		 consumeBytes(const void *buffer, size_t size, void *application_specific_key);
int		 validate_encoding(VALUE encoding);
VALUE	 get_schema_from_td_string(char *symbol);
int		 is_undefined(VALUE v, int base_type);
VALUE	 get_optional_value(VALUE v, asn_TYPE_member_t *member);
VALUE	 instance_of_undefined(void);
int		 is_anonymous(asn_TYPE_descriptor_t *td);
char	*class_name(asn_TYPE_descriptor_t *td);
VALUE	 get_primitive_type(int t);

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
/******************************************************************************/
VALUE
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

	switch(td->base_type)
	{
		case ASN1_TYPE_INTEGER :
		case ASN1_TYPE_UTF8String :
			return get_primitive_type(td->base_type);
			break;

		default :
			break;
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
	 * 1. Define type, based on base ASN.1 type.
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
	 * 2. Create reference to schema class from type class
	 */
	rb_define_const(type_class, "ASN1_TYPE", rb_str_new2(descriptor_symbol));

	/*
	 * 3. Make schema class and type class refer to each other.
	 */
	rb_define_const(type_class, "ASN1_SCHEMA", schema_class);

	finalize_schema(schema_root, schema_class, type_root, type_class, td);

	free((void *)name);

	return type_class;
}


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
/* define_sequence                                                           */
/******************************************************************************/
void
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
void
define_sequence_of(VALUE schema_root, VALUE type_class, asn_TYPE_descriptor_t *td)
{
	return; /*  type_class;	/* XXXXX - Noop */
}


/******************************************************************************/
/* define_choice                                                              */
/******************************************************************************/
void
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
void
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
/* get_schema_stub	 		                                                  */
/* Find or create a schema class representing an ASN.1 class.				  */
/******************************************************************************/
VALUE
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
VALUE
get_type_stub(VALUE type_root, asn_TYPE_descriptor_t *td, char *name)
{
	return rb_define_class_under(type_root, name, rb_cObject);
}


/******************************************************************************/
/* finalize_schema															  */
/* Everything that can't be done while we're worried about recursive		  */
/* definitions.  Currently, just attaches a candidate type to a schema.		  */
/******************************************************************************/
void
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
/* create_attribute_hash                                                      */
/******************************************************************************/
VALUE
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
/* lookup_type                                                                */
/******************************************************************************/
VALUE
lookup_type(asn_TYPE_descriptor_t *td)
{
	VALUE type;

	if (td->generated == 0)
	{
		switch(td->base_type)
		{
			case ASN1_TYPE_NULL :
				type = rb_cNilClass;
				break;

			case ASN1_TYPE_INTEGER :
				type = rb_cFixnum;
				break;

			case ASN1_TYPE_REAL :
				type = rb_cFloat;
				break;

			case ASN1_TYPE_IA5String :
				type = rb_cString;
				break;

			case ASN1_TYPE_UTF8String :
				type = rb_cString;
				break;

			default :
				rb_raise(rb_eStandardError, "lookup_type: Unknown base type");
				break;
		}
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


/******************************************************************************/
/* validate_encoding														  */
/******************************************************************************/
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


/******************************************************************************/
/* is_undefined																  */
/******************************************************************************/
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


/******************************************************************************/
/* get_optional_value														  */
/******************************************************************************/
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


/******************************************************************************/
/* instance_of_undefined													  */
/******************************************************************************/
VALUE
instance_of_undefined(void)
{
	ID asn_module_id	  = rb_intern("Asn1");
    ID undefined_class_id = rb_intern("Undefined");

	VALUE asn_module	  = rb_const_get(rb_cObject, asn_module_id);
	VALUE undefined_class = rb_const_get(asn_module, undefined_class_id);

	return rb_funcall(undefined_class, rb_intern("instance"), 0, rb_ary_new2(0));
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


/******************************************************************************/
/* class_name																  */
/******************************************************************************/
static char *anon_prefix = "Anon";

char *
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
VALUE
get_primitive_type(int t)
{
	VALUE type;

	switch(t)
	{
		VALUE primitive_id;

		case ASN1_TYPE_INTEGER :
			type = rb_cFixnum;
			break;

		case ASN1_TYPE_IA5String :
		case ASN1_TYPE_UTF8String :
			type = rb_cString;
			break;

		default :
			rb_raise(rb_eStandardError, "type_for_primitive(): can't find primitive type");
			break;
	}

	return type;
}
