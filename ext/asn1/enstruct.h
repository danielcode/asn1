#if !defined(ENSTRUCT_C)
/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern void *enstruct_object(VALUE v,           asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_member(VALUE v,      asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_primitive(VALUE v,   asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_integer(VALUE v,     asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_real(VALUE v,        asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_boolean(VALUE v,     asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_null(VALUE v,        asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_ia5string(VALUE v,   asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_enumerated(VALUE v,  asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_sequence(VALUE v,    asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_sequence_of(VALUE v, asn_TYPE_descriptor_t *td, void *container);
#endif
