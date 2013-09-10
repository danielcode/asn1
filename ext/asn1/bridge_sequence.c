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
VALUE  asn1_enstruct_integer(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  asn1_enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container);

asn_TYPE_descriptor_t *asn1_get_td_from_class(VALUE class);

extern asn_TYPE_descriptor_t *get_type_descriptor(const char *symbol);
extern int	consumeBytes(const void *buffer, size_t size, void *application_specific_key);

VALUE
encode_sequence(VALUE class, VALUE encoder, VALUE v)
{
	asn_TYPE_descriptor_t *td = NULL;
	void *s	 	 = NULL;
	char *tdName = NULL;

	bufferInfo_t bi;
	VALUE		 vTD;
    VALUE		 encoded;

	SimpleSequence_t *ss;

	/*
	 * 1. Find type descriptor associated with class
	 */
	td = asn1_get_td_from_class(class);
	if (td == NULL) {
		rb_raise(rb_eException, "Can't find type descriptor");
	}

	/*
	* Extract number
	*/
	/*
	if (!(Qtrue == rb_obj_is_kind_of(v, T_FIXNUM)))
	{
		rb_raise(rb_eException, "Not an integer");
	}
	*/

	s  = enstruct_sequence(td, class, v);
	ss = (SimpleSequence_t *)s;

	/*
	 * Convert to ASN structure
	 */

	bi.buffer = (char *)malloc(512);
	bi.offset = 0;
	bi.length = 512;

	td->der_encoder(td, s, 0, 0, (asn_app_consume_bytes_f *)consumeBytes, (void *)&bi);

	encoded = rb_str_new(bi.buffer, bi.offset);

	return encoded;
}

void *
enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE v)
{
	char		 *str;
	char		 *holding_struct;
	int			  length;
	VALUE		  encoded;
	VALUE		  tmpStr = rb_str_new2("");

	int i;

	/*
	 * 2. Create a new C struct we'll use to hold data data
	 */
	holding_struct = calloc(1, td->container_size);
	if (holding_struct == NULL) {
		rb_raise(rb_eException, "Can't malloc memory");
	}

	/*
	 * 3. Go encoding
	 */
	for (i = 0; i < td->elements_count; i++) {
		asn_TYPE_member_t		*members	= (asn_TYPE_member_t *)td->elements;
		asn_TYPE_descriptor_t	*element_td = members[i].type;

		char *member_struct;

		/*
		 * Work out where to store data
		 */
		if (members[i].flags & ATF_POINTER) {
			/* Need to create a new one */
			rb_raise(rb_eException, "No code to create new");
		} else {
			member_struct = holding_struct + members[i].memb_offset;
		}

		/*
		 *
		 */
		if (element_td->base_type == ASN1_TYPE_INTEGER) {
			asn1_enstruct_integer(v, &(members[i]), (void *)member_struct);
			rb_str_cat(tmpStr, "INTEGER\n", 8);
		} else if (element_td->base_type == ASN1_TYPE_IA5String) {
			asn1_enstruct_ia5string(v, &(members[i]), (void *)member_struct);
			rb_str_cat(tmpStr, "IA5String\n", 10);
		}
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


asn_TYPE_descriptor_t *
asn1_get_td_from_class(VALUE class)
{
	asn_TYPE_descriptor_t *td = NULL;
	char	*tdName = NULL;
	VALUE	 vTD;
	int		 length;

	/*
	 * 1. Find type descriptor associated with class
	 */
	vTD	   = rb_const_get(class, rb_intern("ASN_TYPE"));
	length = RSTRING_LEN(vTD);
	tdName = RSTRING_PTR(vTD);

	return get_type_descriptor(tdName);
}


VALUE
decode_sequence(VALUE class, VALUE encoder, VALUE bit_string)
{
	asn_TYPE_descriptor_t *td = NULL;
	asn_codec_ctx_t context;
	char		*str;
	int			 length;
	void        *st = NULL;
	SimpleSequence_t **ssp;
	SimpleSequence_t *ss;

	td = asn1_get_td_from_class(class);
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
	/* st = calloc(1, td->container_size); */
	context.max_stack_size = 0;
	td->ber_decoder(&context, td, (void **)&st, (void *)str, length, 0);

	ssp = (SimpleSequence_t **)st;
	ss  = (SimpleSequence_t *)*ssp;

	/* 3. Hand to ruby */
	return Qnil; /* rb_str_new(st->buf, st->size); */
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

VALUE
asn1_enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int    result;
	VALUE  memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
    char  *str    = RSTRING_PTR(memb);
	int    length = RSTRING_LEN(memb);

	/*
	if (!(Qtrue == rb_obj_is_kind_of(v, T_STRING))) {
		rb_raise(rb_eException, "Not a string");
	}
	*/

	result = OCTET_STRING_fromBuf((OCTET_STRING_t *)container, (const char *)str, length);
	/* if (result) Check result */

	return Qnil;
}

