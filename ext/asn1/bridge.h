/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern VALUE  asn1_encode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, void *object);
extern void  *asn1_decode_object(asn_TYPE_descriptor_t *td, VALUE encoder_v, VALUE byte_string);
extern void	  check_constraints(asn_TYPE_descriptor_t *d, void *s);
