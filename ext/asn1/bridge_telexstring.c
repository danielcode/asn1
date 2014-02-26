/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "TeletexString.h"

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
/* encode_teletexstring													  	  */
/******************************************************************************/
VALUE
encode_teletexstring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_TeletexString, NULL);
	check_constraints(&asn_DEF_TeletexString, s);
	return asn1_encode_object(&asn_DEF_TeletexString, encoder, s);
}


/******************************************************************************/
/* decode_teletexstring													 	  */
/******************************************************************************/
VALUE
decode_teletexstring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_TeletexString, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_TeletexString, container);

	return val;
}
