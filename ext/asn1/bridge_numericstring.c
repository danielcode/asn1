/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "NumericString.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_NumericString;


/******************************************************************************/
/* encode_numericstring													  	  */
/******************************************************************************/
VALUE
encode_numericstring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_NumericString, NULL);
	check_constraints(&asn_DEF_NumericString, s);
	return asn1_encode_object(&asn_DEF_NumericString, encoder, s);
}


/******************************************************************************/
/* decode_numericstring													  	  */
/******************************************************************************/
VALUE
decode_numericstring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_NumericString, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_NumericString, container);

	return val;
}
