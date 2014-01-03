/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "REAL.h"

/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
VALUE encode_real(VALUE class, VALUE encoder, VALUE v);
VALUE decode_real(VALUE class, VALUE encoder, VALUE byte_string);

extern VALUE  asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object);
extern void  *asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string);

extern asn_TYPE_descriptor_t asn_DEF_REAL;


/******************************************************************************/
/* encode_real																  */
/* Convert a ruby FLOAT to the nominated encoding.							  */
/******************************************************************************/
VALUE
encode_real(VALUE class, VALUE encoder, VALUE v)
{
	double val;
	REAL_t st;	/* Shouldn't need to know about internals */
	st.buf  = NULL;
	st.size = 0;

	/*
	 * Validate
	 */
	if (!TYPE(v) == T_FLOAT)
	{
		rb_raise(rb_eException, "Not a float");
	}

	/*
	 * Extract number
	 */
    val = NUM2DBL(v);

	/*
	 * Convert to ASN structure
	 */
	(void)asn_double2REAL(&st, val);

	return asn1_encode_object(&asn_DEF_REAL, encoder, &st);
}


/******************************************************************************/
/* decode_real																  */
/******************************************************************************/
VALUE
decode_real(VALUE class, VALUE encoder, VALUE byte_string)
{
	double result;
	REAL_t *st = NULL;

	st = (REAL_t *)asn1_decode_object(&asn_DEF_REAL, encoder, byte_string);

	/* 3. Convert to number */
	(void)asn_REAL2double(st, &result);
	
	/* 4. Hand to ruby */
	return rb_float_new(result);
}
