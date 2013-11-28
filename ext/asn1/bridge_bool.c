#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "BOOLEAN.h"

/*
 * Forward declarations
 */
VALUE encode_boolean(VALUE class, VALUE encoder, VALUE v);
VALUE decode_boolean(VALUE class, VALUE encoder, VALUE byte_string);

extern VALUE  asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object);
extern void  *asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string);

extern asn_TYPE_descriptor_t asn_DEF_BOOLEAN;

/*
 * encode_bool
 * Convert a ruby FIXNUM to the nominated encoding.
 */
VALUE
encode_boolean(VALUE class, VALUE encoder, VALUE v)
{
	int bool;
	BOOLEAN_t b;

	/*
	 * Validate
	 */
	/*
	if (!(TYPE(v) == T_TRUE || TYPE(v) == T_FALSE))
	{
		rb_raise(rb_eException, "Not a boolean class");
	}
	*/

	/*
	 * Extract number
	 */
	if (TYPE(v) == T_TRUE)
	{
		bool = 1;
	}
	else
	{
		bool = 0;
	}

	return asn1_encode_object(&asn_DEF_BOOLEAN, encoder, &bool);
}


VALUE
decode_boolean(VALUE class, VALUE encoder, VALUE byte_string)
{
	BOOLEAN_t *bool;
	int result;

	bool = (BOOLEAN_t *)asn1_decode_object(&asn_DEF_BOOLEAN, encoder, byte_string);

	if (*bool == 0)
	{
		return Qfalse;
	}

	return Qtrue;
}
