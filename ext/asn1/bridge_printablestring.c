/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "PrintableString.h"

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
/* encode_printablestring												  	  */
/******************************************************************************/
VALUE
encode_printablestring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_PrintableString, NULL);
	check_constraints(&asn_DEF_PrintableString, s);
	return asn1_encode_object(&asn_DEF_PrintableString, encoder, s);
}


/******************************************************************************/
/* decode_printablestring												  	  */
/******************************************************************************/
VALUE
decode_printablestring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_PrintableString, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_PrintableString, container);

	return val;
}
