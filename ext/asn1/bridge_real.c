/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDGE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "REAL.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_REAL;


/******************************************************************************/
/* encode_real																  */
/* Convert a ruby FLOAT to the nominated encoding.							  */
/******************************************************************************/
VALUE
encode_real(VALUE class, VALUE encoder, VALUE v)
{
	void  *s		= enstruct_object(v, &asn_DEF_REAL, NULL);
	VALUE  encoded	= asn1_encode_object(&asn_DEF_REAL, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_real																  */
/******************************************************************************/
VALUE
decode_real(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_REAL, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_REAL, container);

	return val;
}
