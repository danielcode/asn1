/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDGE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "INTEGER.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_INTEGER;


/******************************************************************************/
/* encode_int																  */
/* Convert a ruby FIXNUM to the nominated encoding.							  */
/******************************************************************************/
VALUE
encode_int(VALUE class, VALUE encoder, VALUE v)
{
	void  *s	   = enstruct_object(v, &asn_DEF_INTEGER, NULL);
	VALUE  encoded = asn1_encode_object(&asn_DEF_INTEGER, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_int																  */
/******************************************************************************/
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
