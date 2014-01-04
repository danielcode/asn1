/******************************************************************************/
/* Externals																  */
/******************************************************************************/
#if !defined(UNSTRUCT_C)
extern VALUE unstruct_member(asn_TYPE_member_t *member, char *member_struct);
extern VALUE unstruct_sequence(VALUE schema_class, char *buffer);
extern VALUE unstruct_primitive(asn_TYPE_member_t *member, char *member_struct);

extern VALUE unstruct_integer(  char *member_struct);
extern VALUE unstruct_real(     char *member_struct);
extern VALUE unstruct_boolean(  char *member_struct);
extern VALUE unstruct_null(     char *member_struct);
extern VALUE unstruct_ia5string(char *member_struct);

extern VALUE unstruct_sequence(VALUE schema_class,    char *buffer);
extern VALUE unstruct_sequence_of(VALUE schema_class, char *buffer);
extern VALUE unstruct_choice(VALUE schema_class,      char *buffer);
extern VALUE unstruct_enumerated(VALUE schema_class,  char *buffer);
#endif
