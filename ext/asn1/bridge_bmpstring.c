/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "BMPString.h"

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
/* encode_bmpstring														  	  */
/******************************************************************************/
VALUE
encode_bmpstring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_BMPString, NULL);
	check_constraints(&asn_DEF_BMPString, s);
	return asn1_encode_object(&asn_DEF_BMPString, encoder, s);
}


/******************************************************************************/
/* decode_bmpstring														  	  */
/******************************************************************************/
VALUE
decode_bmpstring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_BMPString, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_BMPString, container);

	return val;
}
