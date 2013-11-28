#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "IA5String.h"

#include "util.h"

/*
 * Forward declarations
 */
VALUE encode_ia5string(VALUE class, VALUE encoder, VALUE v);
VALUE decode_ia5string(VALUE class, VALUE encoder, VALUE byte_string);

extern int	consumeBytes(const void *buffer, size_t size, void *application_specific_key);

/* XXXXX - wrong.  Should be in reference to IA5String, not OCTET STRING. */
extern OCTET_STRING_t *OCTET_STRING_new_fromBuf(asn_TYPE_descriptor_t *td, const char *str, int len);

extern asn_TYPE_descriptor_t asn_DEF_IA5String;

VALUE
encode_ia5string(VALUE class, VALUE encoder, VALUE v)
{
	char		 *str;
	int			  length;
	bufferInfo_t  bi;
	IA5String_t	 *st;
	VALUE		  encoded;

	bi.buffer = (char *)malloc(512);
    bi.offset = 0;
    bi.length = 512;

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

	/*
	 * Convert to ASN structure
	 */
	st = (IA5String_t *)OCTET_STRING_new_fromBuf(&asn_DEF_IA5String, str, length);
	asn_DEF_IA5String.der_encoder(&asn_DEF_IA5String, (void *)st, 0, 0, (asn_app_consume_bytes_f *)consumeBytes,
								  (void *)&bi);

	encoded = rb_str_new(bi.buffer, bi.offset);

	return encoded;
}

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
