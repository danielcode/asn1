#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "INTEGER.h"

#include "util.h"

/*
 * Forward declarations
 */
VALUE encode_int(VALUE class, VALUE encoder, VALUE v);
VALUE decode_int(VALUE class, VALUE encoder, VALUE bit_string);

extern int		consumeBytes(const void *buffer, size_t size, void *application_specific_key);

extern asn_TYPE_descriptor_t asn_DEF_INTEGER;

VALUE
encode_int(VALUE class, VALUE encoder, VALUE v)
{
	bufferInfo_t bi;
	INTEGER_t	 st;
	VALUE		 encoded;

	bi.buffer = (char *)malloc(512);
    bi.offset = 0;
    bi.length = 512;

	/*
	 * Extract number
	 */
	/*
	if (!(Qtrue == rb_obj_is_kind_of(v, T_FIXNUM)))
	{
		rb_raise(rb_eException, "Not an integer");
	}
	*/

    long val = FIX2LONG(v);

	/*
	 * Convert to ASN structure
	 */
	(void)asn_long2INTEGER(&st, val);
	asn_DEF_INTEGER.der_encoder(&asn_DEF_INTEGER, (void *)&st, 0, 0, (asn_app_consume_bytes_f *)consumeBytes,
								(void *)&bi);

	encoded = rb_str_new(bi.buffer, bi.offset);

	/* return INT2FIX(val + 1); */
	return encoded;
}

VALUE
decode_int(VALUE class, VALUE encoder, VALUE bit_string)
{
	char		*str;
	int			 length;
	INTEGER_t	 *st = malloc(sizeof (INTEGER_t));
	long		 result;

	/* 1. Retrive BER string and its length. */
	str    = RSTRING_PTR(bit_string);
	length = RSTRING_LEN(bit_string);

	/* 2. Decode BER to asn1c internal format */
	asn_DEF_INTEGER.ber_decoder(NULL, &asn_DEF_INTEGER, (void **)&st, (void *)str, length, 0);

	/* 3. Convert to number */
	(void)asn_INTEGER2long(st, &result);
	
	/* 4. Hand to ruby */
	return INT2FIX(result);
}
