/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "UniversalString.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_UniversalString;


/******************************************************************************/
/* encode_universalstring												  	  */
/******************************************************************************/
VALUE
encode_universalstring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_UniversalString, NULL);
	check_constraints(&asn_DEF_UniversalString, s);
	return asn1_encode_object(&asn_DEF_UniversalString, encoder, s);
}


/******************************************************************************/
/* decode_universalstring												  	  */
/******************************************************************************/
VALUE
decode_universalstring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_UniversalString, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_UniversalString, container);

	return val;
}
