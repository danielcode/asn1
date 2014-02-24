/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "VisibleString.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_VisibleString;


/******************************************************************************/
/* encode_visiblestring														  	  */
/******************************************************************************/
VALUE
encode_visiblestring(VALUE class, VALUE encoder, VALUE v)
{
	void  *s = enstruct_object(v, &asn_DEF_VisibleString, NULL);
	check_constraints(&asn_DEF_VisibleString, s);
	return asn1_encode_object(&asn_DEF_VisibleString, encoder, s);
}


/******************************************************************************/
/* decode_visiblestring														  	  */
/******************************************************************************/
VALUE
decode_visiblestring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_VisibleString, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_VisibleString, container);

	return val;
}
