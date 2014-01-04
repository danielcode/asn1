/******************************************************************************/
/* Externals																  */
/******************************************************************************/
#if !defined(UNSTRUCT_C)
extern VALUE unstruct_object(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_member(asn_TYPE_descriptor_t *td,			char *container);

extern VALUE unstruct_integer(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_real(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_boolean(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_null(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_ia5string(asn_TYPE_descriptor_t *td,		char *container);

extern VALUE unstruct_sequence(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_sequence_of(asn_TYPE_descriptor_t *td,	char *container);
extern VALUE unstruct_choice(asn_TYPE_descriptor_t *td, 		char *container);
extern VALUE unstruct_enumerated(asn_TYPE_descriptor_t *td,		char *container);
#endif
