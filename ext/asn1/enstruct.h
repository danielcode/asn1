#ifndef ENSTRUCT_H
#define ENSTRUCT_H 1

/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern void  enstruct_member(VALUE v, asn_TYPE_member_t *member, char *container);

extern void *enstruct_object(VALUE v,			asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_primitive(VALUE v,		asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_integer(VALUE v,			asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_real(VALUE v,				asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_boolean(VALUE v,			asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_null(VALUE v,				asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_ia5string(VALUE v,		asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_utf8string(VALUE v,		asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_numericstring(VALUE v,	asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_universalstring(VALUE v,	asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_visiblestring(VALUE v,	asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_bmpstring(VALUE v,		asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_printablestring(VALUE v,	asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_teletexstring(VALUE v,	asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_enumerated(VALUE v,		asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_sequence(VALUE v,			asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_sequence_of(VALUE v,		asn_TYPE_descriptor_t *td, void *container);
extern void *enstruct_choice(VALUE v,			asn_TYPE_descriptor_t *td, void *container);
#endif
