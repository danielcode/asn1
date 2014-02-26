/******************************************************************************/
/* Externals																  */
/******************************************************************************/
#ifndef UNSTRUCT_H
#define UNSTRUCT_H 1

extern void  unstruct_member(VALUE v, asn_TYPE_member_t *member, char *buffer);

extern VALUE unstruct_object(asn_TYPE_descriptor_t *td,				char *container);
extern VALUE unstruct_integer(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_real(asn_TYPE_descriptor_t *td,				char *container);
extern VALUE unstruct_boolean(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_null(asn_TYPE_descriptor_t *td,				char *container);   
extern VALUE unstruct_octetstring(asn_TYPE_descriptor_t *td,		char *container);   
extern VALUE unstruct_generalizedtime(asn_TYPE_descriptor_t *td,	char *container);   
extern VALUE unstruct_ia5string(asn_TYPE_descriptor_t *td,			char *container);   
extern VALUE unstruct_utf8string(asn_TYPE_descriptor_t *td,			char *container);   
extern VALUE unstruct_numericstring(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_universalstring(asn_TYPE_descriptor_t *td,	char *container);
extern VALUE unstruct_visiblestring(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_bmpstring(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_printablestring(asn_TYPE_descriptor_t *td,	char *container);
extern VALUE unstruct_teletexstring(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_bitstring(asn_TYPE_descriptor_t *td,			char *container);

extern VALUE unstruct_sequence(asn_TYPE_descriptor_t *td,			char *container);
extern VALUE unstruct_sequence_of(asn_TYPE_descriptor_t *td,		char *container);
extern VALUE unstruct_choice(asn_TYPE_descriptor_t *td,				char *container);
extern VALUE unstruct_enumerated(asn_TYPE_descriptor_t *td,			char *container);

#endif
