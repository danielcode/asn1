/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "ISO646String.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/


/******************************************************************************/
/* encode_iso646string													  	  */
/******************************************************************************/
VALUE
encode_iso646string(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_ISO646String, NULL);
	check_constraints(&asn_DEF_ISO646String, s);
	return asn1_encode_object(&asn_DEF_ISO646String, encoder, s);
}


/******************************************************************************/
/* decode_iso646string													  	  */
/******************************************************************************/
VALUE
decode_iso646string(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_ISO646String, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_ISO646String, container);

	return val;
}
