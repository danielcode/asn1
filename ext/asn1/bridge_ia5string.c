/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDGE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "IA5String.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_IA5String;


/******************************************************************************/
/* encode_ia5string														  	  */
/******************************************************************************/
VALUE
encode_ia5string(VALUE class, VALUE encoder, VALUE v)
{
	void  *s		= enstruct_object(v, &asn_DEF_IA5String, NULL);
	VALUE  encoded  = asn1_encode_object(&asn_DEF_IA5String, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_ia5string														  	  */
/******************************************************************************/
VALUE
decode_ia5string(VALUE class, VALUE encoder, VALUE byte_string)
{
	asn_codec_ctx_t context;
	char		*str;
	int			 length;
	IA5String_t	 *st = NULL;

	/* 1. Retrive BER string and its length. */
	str    = RSTRING_PTR(byte_string);
	length = RSTRING_LEN(byte_string);

	/* 2. Decode BER to asn1c internal format */
	asn_DEF_IA5String.ber_decoder(NULL, &asn_DEF_IA5String, (void **)&st, (void *)str, length, 0);

	/* 3. Hand to ruby */
	return rb_str_new(st->buf, st->size);
}
