/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "OCTET_STRING.h"

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
/* encode_octetstring													  	  */
/******************************************************************************/
VALUE
encode_octetstring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_OCTET_STRING, NULL);
	check_constraints(&asn_DEF_OCTET_STRING, s);
	return asn1_encode_object(&asn_DEF_OCTET_STRING, encoder, s);
}


/******************************************************************************/
/* decode_octetstring													  	  */
/******************************************************************************/
VALUE
decode_octetstring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_OCTET_STRING, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_OCTET_STRING, container);

	return val;
}
