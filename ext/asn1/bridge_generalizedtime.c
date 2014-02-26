/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "GeneralizedTime.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* encode_generalizedtime													  */
/******************************************************************************/
VALUE
encode_generalizedtime(VALUE class, VALUE encoder, VALUE v)
{
	void *s = enstruct_object(v, &asn_DEF_GeneralizedTime, NULL);
	check_constraints(&asn_DEF_GeneralizedTime, s);
	return asn1_encode_object(&asn_DEF_GeneralizedTime, encoder, s);
}


/******************************************************************************/
/* decode_generalizedtime													  */
/******************************************************************************/
VALUE
decode_generalizedtime(VALUE class, VALUE encoder, VALUE byte_string)
{
	void *container = asn1_decode_object(&asn_DEF_GeneralizedTime, encoder, byte_string);
	VALUE val = unstruct_object(&asn_DEF_GeneralizedTime, container);
	
	return val;
}
