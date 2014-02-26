#ifndef ENCODE_H
#define ENCODE_H 1
extern VALUE encode_int(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_real(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_boolean(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_null(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_generalizedtime(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_ia5string(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_utf8string(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_numericstring(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_universalstring(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_visiblestring(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_bmpstring(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_printablestring(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_teletexstring(VALUE class, VALUE encoder, VALUE v);

extern VALUE encode_sequence(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_sequence_of(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_choice(VALUE class, VALUE encoder, VALUE v);
extern VALUE encode_enumerated(VALUE class, VALUE encoder, VALUE v);
#endif
