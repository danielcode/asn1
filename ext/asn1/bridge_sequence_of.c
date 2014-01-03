/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "expr_types.h"

#include "util.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
VALUE  encode_sequence_of(VALUE class, VALUE encoder, VALUE v);
VALUE  decode_sequence_of(VALUE class, VALUE encoder, VALUE byte_string);


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern VALUE  asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object);
extern void  *asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string);
extern void  *enstruct_sequence_of(asn_TYPE_descriptor_t *td, VALUE class, VALUE v);
extern VALUE  unstruct_sequence_of(VALUE schema_class, char *buffer);

extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);


/******************************************************************************/
/* encode_sequence_of														  */
/******************************************************************************/
VALUE
encode_sequence_of(VALUE class, VALUE encoder, VALUE v)
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
	s  = enstruct_sequence_of(td, class, v);

	/*
	 * 3. Perform encoding to specified serialization format (i.e. BER, DER, PER or XML)
	 */
	encoded	= asn1_encode_object(td, encoder, s);

	return encoded;
}


/******************************************************************************/
/* decode_sequence															  */
/******************************************************************************/
VALUE
decode_sequence_of(VALUE class, VALUE encoder, VALUE byte_string)
{
	asn_TYPE_descriptor_t *td = NULL;

	VALUE  v;
	void  *st = NULL;
	int	   length;

	td = asn1_get_td_from_schema(class);
	if (td == NULL) {
		rb_raise(rb_eException, "Can't find type descriptor");
	}

	st = asn1_decode_object(td, encoder, byte_string);

	/*
	 * Convert from asn1c structure to ruby object
	 */
	v = unstruct_sequence_of(class, (char *)st);

	/*
	 * Hand to ruby
	 */
	return v;
}
