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
	void *s = enstruct_object(v, &asn_DEF_INTEGER, NULL);
	check_constraints(&asn_DEF_INTEGER, s);
	return asn1_encode_object(&asn_DEF_INTEGER, encoder, s);
}


/******************************************************************************/
/* decode_int																  */
/******************************************************************************/
VALUE
decode_int(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_INTEGER, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_INTEGER, container);
	
	return val;
}
