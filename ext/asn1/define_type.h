#ifndef DEFINE_TYPE_H
#define DEFINE_TYPE_H 1
extern VALUE get_schema_from_td_string(char *symbol);
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);
extern VALUE define_type_from_ruby(VALUE class, VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE symbol);
#endif
