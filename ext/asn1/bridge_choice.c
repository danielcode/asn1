/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#define BRIDE_C 1

#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "expr_types.h"

#include "bridge.h"
#include "enstruct.h"
#include "unstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);


/******************************************************************************/
/* encode_choice															  */
/******************************************************************************/
VALUE
encode_choice(VALUE class, VALUE encoder, VALUE v)
{
	asn_TYPE_descriptor_t *td = NULL;
	void *s = NULL;
	VALUE encoded;

	/*
	 * 1. Find type descriptor associated with class
	 */
	td = asn1_get_td_from_schema(class);
	if (td == NULL) {
		rb_raise(rb_eException, "Can't find type descriptor");
	}

	/*
	 * 2. Create C struct equivalent of ruby object
	 */
	s  = enstruct_object(v, td, (void *)NULL);

	/*
	 * 3. Perform encoding to specified serialization format (i.e. BER, DER, PER or XML)
	 */
	encoded	= asn1_encode_object(td, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_choice															  */
/******************************************************************************/
VALUE
decode_choice(VALUE class, VALUE encoder, VALUE byte_string)
{
	asn_TYPE_descriptor_t *td = NULL;

	void *st = NULL;
	char *str;
	int	  length;

	VALUE v;

	td = asn1_get_td_from_schema(class);
	if (td == NULL) {
		rb_raise(rb_eException, "Can't find type descriptor");
	}

	st = asn1_decode_object(td, encoder, byte_string);

	/*
	 * Convert from asn1c structure to ruby object
	 */
	v = unstruct_choice(class, (char *)st);

	/*
	 * Hand to ruby
	 */
	return v;
}
