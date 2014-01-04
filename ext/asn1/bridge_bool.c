/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDGE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "BOOLEAN.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"

/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_BOOLEAN;

/******************************************************************************/
/* encode_bool																  */
/* Convert a ruby FIXNUM to the nominated encoding.							  */
/******************************************************************************/
VALUE
encode_boolean(VALUE class, VALUE encoder, VALUE v)
{
	void  *s		= enstruct_object(v, &asn_DEF_BOOLEAN, NULL);
	VALUE  encoded	= asn1_encode_object(&asn_DEF_BOOLEAN, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_bool																  */
/******************************************************************************/
VALUE
decode_boolean(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_BOOLEAN, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_BOOLEAN, container);

	return val;
}
