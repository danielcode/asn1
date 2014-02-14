/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDGE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "UTF8String.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_UTF8String;


/******************************************************************************/
/* encode_utf8string													  	  */
/******************************************************************************/
VALUE
encode_utf8string(VALUE class, VALUE encoder, VALUE v)
{
	void  *s		= enstruct_object(v, &asn_DEF_UTF8String, NULL);
	VALUE  encoded  = asn1_encode_object(&asn_DEF_UTF8String, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_utf8string														  	  */
/******************************************************************************/
VALUE
decode_utf8string(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_UTF8String, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_UTF8String, container);

	return val;
}
