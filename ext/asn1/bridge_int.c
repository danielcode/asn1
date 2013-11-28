#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "INTEGER.h"

/*
 * Forward declarations
 */
VALUE encode_int(VALUE class, VALUE encoder, VALUE v);
VALUE decode_int(VALUE class, VALUE encoder, VALUE byte_string);

extern VALUE  asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object);
extern void  *asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string);

extern asn_TYPE_descriptor_t asn_DEF_INTEGER;

/*
 * encode_int
 * Convert a ruby FIXNUM to the nominated encoding.
 */
VALUE
encode_int(VALUE class, VALUE encoder, VALUE v)
{
	INTEGER_t st;
	st.buf  = NULL;
	st.size = 0;

	/*
	 * Validate
	 */
	/*
	if (!TYPE(v) == T_FIXNUM)
	{
		rb_raise(rb_eException, "Not an integer");
	}
	*/

	/*
	 * Extract number
	 */
    long val = FIX2LONG(v);

	/*
	 * Convert to ASN structure
	 */
	(void)asn_long2INTEGER(&st, val);

	return asn1_encode_object(&asn_DEF_INTEGER, encoder, &st);
}

VALUE
decode_int(VALUE class, VALUE encoder, VALUE byte_string)
{
	long result;
	INTEGER_t *st = NULL; /* malloc(sizeof (INTEGER_t)); */

	st = (INTEGER_t *)asn1_decode_object(&asn_DEF_INTEGER, encoder, byte_string);

	/* 3. Convert to number */
	(void)asn_INTEGER2long(st, &result);
	
	/* 4. Hand to ruby */
	return INT2FIX(result);
}
