/*
 *
 */
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "expr_types.h"

#include "util.h"

#include "INTEGER.h"
#include "IA5String.h"
#include "OCTET_STRING.h"
#include "SimpleSequence.h"

/*
 * Forward declarations
 */
VALUE  encode_sequence(VALUE class, VALUE encoder, VALUE v);
VALUE  decode_sequence(VALUE class, VALUE encoder, VALUE sequence);
void  *enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE v);
VALUE  destruct_sequence(VALUE schema_class, char *buffer);
char  *enstruct_member(VALUE v, asn_TYPE_member_t *member, char *member_struct);
char  *enstruct_primitive(VALUE v, asn_TYPE_member_t *member, char *member_struct);
VALUE  asn1_enstruct_integer(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  asn1_enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container);

VALUE destruct_member(asn_TYPE_member_t *member, char *member_struct);
VALUE destruct_primitive(asn_TYPE_member_t *member, char *member_struct);
VALUE asn1_destruct_integer(char *member_struct);
VALUE asn1_destruct_ia5string(char *member_struct);

asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);
static char *setter_name_from_member_name(char *name);

/*
 * Externals
 */
extern asn_TYPE_descriptor_t *get_type_descriptor(const char *symbol);
extern int	consumeBytes(const void *buffer, size_t size, void *application_specific_key);

VALUE
encode_sequence(VALUE class, VALUE encoder, VALUE v)
{
	asn_TYPE_descriptor_t *td = NULL;
	void *s	 	 = NULL;
	char *tdName = NULL;

	asn_enc_rval_t er;
	bufferInfo_t   bi;
	VALUE vTD;
    VALUE encoded;

	/*
	 * 1. Find type descriptor associated with class
	 */
	td = asn1_get_td_from_schema(class);
	if (td == NULL) {
		rb_raise(rb_eException, "Can't find type descriptor");
	}

	/*
	 * 2. Create C struct equivalent of ruby object
	 */
	s  = enstruct_sequence(td, class, v);

	/*
	 * 3. Perform encoding to specified serialization format (i.e. BER, DER, PER or XML)
	 */
	bi.buffer = (char *)malloc(512);
	bi.offset = 0;
	bi.length = 512;

	er = td->der_encoder(td, s, 0, 0, (asn_app_consume_bytes_f *)consumeBytes, (void *)&bi);
	if (er.encoded == -1)
	{
		rb_raise(rb_eException, "Can't encode type");
	}

	encoded = rb_str_new(bi.buffer, bi.offset);

	return encoded;
}

void *
enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE v)
{
	char  *str;
	char  *holding_struct;
	int	  length;
	VALUE encoded;
	VALUE tmpStr = rb_str_new2("");

	int i;

	/*
	 * 2. Create a new C struct we'll use to hold data data
	 */
	holding_struct = calloc(1, td->container_size);
	if (holding_struct == NULL) {
		rb_raise(rb_eException, "Can't calloc memory");
	}

	/*
	 * 3. Go encoding
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		char *member_struct;

		/*
		 * XXXXX - add explanation
		 */
		if (member->flags & ATF_POINTER)
		{
			/* Need to create a new one */
			rb_raise(rb_eException, "No code to create new");
		}
		else
		{
			member_struct = holding_struct + member->memb_offset;
		}

		enstruct_member(v, member, member_struct);
	}

	/*
	 * 4. Return encoded string to caller
	 */

	/*
	 * Extract string
	 */
	/*
	if (!(Qtrue == rb_obj_is_kind_of(v, T_STRING)))
	{
		rb_raise(rb_eException, "Not a string");
	}
	*/

    str    = RSTRING_PTR(v);
	length = RSTRING_LEN(v);

	return holding_struct;
}


/*
 * enstruct_member
 * Returns C struct equivalent of corresponding ruby object
 */
char *
enstruct_member(VALUE v, asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->generated == 0)
	{
		return enstruct_primitive(v, member, member_struct);
	}
	else
	{
		rb_raise(rb_eException, "Can't (yet) enstruct complex structures");
	}	
}


/*
 * enstruct_primitive
 */
char *
enstruct_primitive(VALUE v, asn_TYPE_member_t *member, char *member_struct)
{
	asn_TYPE_descriptor_t	*element_td = member->type;

	if (element_td->base_type == ASN1_TYPE_INTEGER)
	{
		asn1_enstruct_integer(v, member, (void *)member_struct);
		/* rb_str_cat(tmpStr, "INTEGER\n", 8); */
	}
	else if (element_td->base_type == ASN1_TYPE_IA5String)
	{
		asn1_enstruct_ia5string(v, member, (void *)member_struct);
		/* rb_str_cat(tmpStr, "IA5String\n", 10); */
	}

	return member_struct;
}


asn_TYPE_descriptor_t *
asn1_get_td_from_schema(VALUE class)
{
	asn_TYPE_descriptor_t *td = NULL;
	char	*tdName = NULL;
	VALUE	 vTD;
	int		 length;

	/*
	 * 1. Find type descriptor associated with class
	 */
	vTD	   = rb_const_get(class, rb_intern("ASN1_TYPE"));
	length = RSTRING_LEN(vTD);
	tdName = RSTRING_PTR(vTD);

	return get_type_descriptor(tdName);
}


VALUE
decode_sequence(VALUE class, VALUE encoder, VALUE bit_string)
{
	asn_TYPE_descriptor_t *td = NULL;
	asn_codec_ctx_t context;
	SimpleSequence_t **ssp;
	SimpleSequence_t *ss;

	void *st = NULL;
	char *str;
	int	  length;

	VALUE v;

	td = asn1_get_td_from_schema(class);
	if (td == NULL) {
		rb_raise(rb_eException, "Can't find type descriptor");
	}

	/*
	 * 1. Retrive BER string and its length.
	 */
	str    = RSTRING_PTR(bit_string);
	length = RSTRING_LEN(bit_string);

	/*
	 * 2. Decode BER to asn1c internal format
	 */
	context.max_stack_size = 0;
	td->ber_decoder(&context, td, (void **)&st, (void *)str, length, 0);

	/*
	 * 3. Convert from asn1c structure to ruby object
	 */
	v = destruct_sequence(class, (char *)st);

	/* 3. Hand to ruby */
	return v; /* rb_str_new(st->buf, st->size); */
}

VALUE
asn1_enstruct_integer(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int   result;
	VALUE memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));

	/*
	if (!(Qtrue == rb_obj_is_kind_of(v, T_FIXNUM))) {
		rb_raise(rb_eException, "Not an integer");
	}
	*/

	result = asn_long2INTEGER((INTEGER_t *)container, NUM2LONG(memb));
	/* if (result) Check result */

	return Qnil;
}

/*
 * Note: assuming OCTET_STRING is constant length
 */
VALUE
asn1_enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int    result;
	VALUE  memb   = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
    char  *str    = RSTRING_PTR(memb);
	int    length = RSTRING_LEN(memb);

	OCTET_STRING_t *os = NULL;

	/*
	if (!(Qtrue == rb_obj_is_kind_of(v, T_STRING))) {
		rb_raise(rb_eException, "Not a string");
	}
	*/

	os = OCTET_STRING_new_fromBuf(member->type, (const char *)str, length);
	memcpy(container, (void *)os, sizeof(OCTET_STRING_t));

	/* if (result) Check result */

	return Qnil;
}


/*
 * destruct_sequence
 */
VALUE
destruct_sequence(VALUE schema_class, char *buffer)
{
	char *symbol;
	int	  i;
	VALUE args = rb_ary_new2(0);

	asn_TYPE_descriptor_t *td = asn1_get_td_from_schema(schema_class);

	VALUE ary   = rb_ary_new();
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);
	(void)rb_ary_push(ary, class);
	(void)rb_ary_push(ary, v);

	/*
	 *
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		VALUE args;
		VALUE member_val = INT2FIX(1);

		char *member_struct;
		char *setter = setter_name_from_member_name(member->name);

		/*
		 * XXXXX - add explanation
		 */
		if (member->flags & ATF_POINTER)
		{
			/* Need to create a new one */
			rb_raise(rb_eException, "No code to de_struct ATF_POINTER");
		}
		else
		{
			member_struct = buffer + member->memb_offset;
		}

		member_val = destruct_member(member, member_struct);
		rb_funcall(v, rb_intern(setter), 1, member_val);

		free(setter);
	}

	return v;
}


VALUE
destruct_member(asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->generated == 0)
	{
		return destruct_primitive(member, member_struct);
	}
	else
	{
		return Qnil; /* XXXXX */
		/* Constructed type */
	}
}


VALUE
destruct_primitive(asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->base_type == ASN1_TYPE_INTEGER)
	{
		return asn1_destruct_integer(member_struct);
		/* rb_str_cat(tmpStr, "INTEGER\n", 8); */
	}
	else if (member->type->base_type == ASN1_TYPE_IA5String)
	{
		asn1_destruct_ia5string(member_struct);
		/* rb_str_cat(tmpStr, "IA5String\n", 10); */
	}
}


VALUE
asn1_destruct_integer(char *member_struct)
{
	long val;
	int  ret;

	INTEGER_t *integer = (INTEGER_t *)member_struct;
	ret = asn_INTEGER2long(integer, &val);

	return INT2FIX(val);
}


VALUE
asn1_destruct_ia5string(char *member_struct)
{
	IA5String_t *ia5string = (IA5String_t *)member_struct;

	return rb_str_new(ia5string->buf, ia5string->size);
}


static char *setter_name_from_member_name(char *name)
{
	int   setter_len = strlen(name) + 2;
	char *setter     = (char *)calloc(setter_len, sizeof(char));

	(void)strcat(setter, name);
	(void)strcat(setter, "=");

	return setter;
}
