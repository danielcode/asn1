/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDGE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "NULL.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_NULL;


/******************************************************************************/
/* encode_null																  */
/******************************************************************************/
VALUE
encode_null(VALUE class, VALUE encoder, VALUE v)
{
	void  *s		= enstruct_object(v, &asn_DEF_NULL, NULL);
	VALUE  encoded	= asn1_encode_object(&asn_DEF_NULL, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_null																  */
/******************************************************************************/
VALUE
decode_null(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_NULL, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_NULL, container);

	return val;
}
