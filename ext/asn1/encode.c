/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>
#include "asn_application.h"
#include "util.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
char  *encoder_string(VALUE encoder);
int	   consumeBytes(const void *buffer, size_t size, void *application_specific_key);

/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern int   consumeBytes(const void *buffer, size_t size, void *application_specific_key);


/******************************************************************************/
/* asn1_encode_object														  */
/******************************************************************************/
VALUE
asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object)
{
	VALUE encoded;
	bufferInfo_t	bi;
	asn_enc_rval_t	er;

	char *encoder = encoder_string(encoder_v);

	/*
	 * Setup buffer
	 */
	bi.buffer = (char *)malloc(512);
    bi.offset = 0;
    bi.length = 512;

	/*
	 * Encode
	 */
	if (strcmp(encoder, "der") == 0)
	{
		er = td->der_encoder(td, object, 0, 0, (asn_app_consume_bytes_f *)consumeBytes, (void *)&bi);
		if (er.encoded == -1)
		{
			rb_raise(rb_eException, "Can't encode type");
		}
	}
	else if (strcmp(encoder, "per") == 0)
	{
	}
	else if (strcmp(encoder, "xer") == 0)
	{
		xer_encode(td, object, XER_F_CANONICAL, (asn_app_consume_bytes_f *)consumeBytes, (void *)&bi);
		if (er.encoded == -1)
		{
			rb_raise(rb_eException, "Can't encode type");
		}
	}

	encoded = rb_str_new(bi.buffer, bi.offset);
	return encoded;
}


/******************************************************************************/
/* asn1_decode_object														  */
/******************************************************************************/
void *
asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string)
{
	asn_dec_rval_t	rval;
	asn_codec_ctx_t context;
	context.max_stack_size = 0;

	void *st = NULL;
	char *str, *encoder;
	int   length;

	encoder = encoder_string(encoder_v);

	/*
	 * 1. Retrive BER string and its length.
	 */
	str    = RSTRING_PTR(byte_string);
	length = RSTRING_LEN(byte_string);

	if (strcmp(encoder, "der") == 0 || strcmp(encoder, "ber") == 0)
	{
		rval = td->ber_decoder(&context, td, (void **)&st, (void *)str, length, 0);
		if (rval.code != RC_OK)
		{
			rb_raise(rb_eException, "Can't encode type");                                                               
		}
	}
	else if (strcmp(encoder, "per") == 0)                                                                             
	{
	}
	else if (strcmp(encoder, "xer") == 0)                                                                             
	{
		rval = xer_decode(0, td, (void **)&st, str, length);
		if (rval.code != RC_OK)
		{
			rb_raise(rb_eException, "Can't encode type");                                                               
		}
	}

	return st;
}


/******************************************************************************/
/* encoder_string															  */
/******************************************************************************/
char *
encoder_string(VALUE encoder)
{
	VALUE symbol_as_string = rb_funcall(encoder, rb_intern("to_s"), 0, rb_ary_new2(0));

	return RSTRING_PTR(symbol_as_string);
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
/* XXXXX - unused at 2014-02-23												  */
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
