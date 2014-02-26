/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "BIT_STRING.h"

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
/* encode_bitstring															  */
/******************************************************************************/
VALUE
encode_bitstring(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_BIT_STRING, NULL);
	check_constraints(&asn_DEF_BIT_STRING, s);
	return asn1_encode_object(&asn_DEF_BIT_STRING, encoder, s);
}


/******************************************************************************/
/* decode_bitstring													 		  */
/******************************************************************************/
VALUE
decode_bitstring(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_BIT_STRING, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_BIT_STRING, container);

	return val;
}
