#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "NULL.h"

/*
 * Forward declarations
 */
VALUE encode_null(VALUE class, VALUE encoder, VALUE v);
VALUE decode_null(VALUE class, VALUE encoder, VALUE byte_string);

extern VALUE  asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object);
extern void  *asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string);

extern asn_TYPE_descriptor_t asn_DEF_NULL;

/*
 * encode_null
 */
VALUE
encode_null(VALUE class, VALUE encoder, VALUE v)
{
	NULL_t st = 0;

	/*
	 * Validate
	 */
	if (!(TYPE(v) == T_NIL))
	{
		rb_raise(rb_eStandardError, "Not nil");
		return Qnil;
	}

	return asn1_encode_object(&asn_DEF_NULL, encoder, &st);
}


VALUE
decode_null(VALUE class, VALUE encoder, VALUE byte_string)
{
	NULL_t *st = NULL;

	st = (NULL_t *)asn1_decode_object(&asn_DEF_NULL, encoder, byte_string);

	return Qnil;
}
