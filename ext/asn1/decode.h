#ifndef DECODE_H
#define DECODE_H 1
extern VALUE decode_int(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_real(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_boolean(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_null(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_ia5string(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_utf8string(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_numericstring(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_universalstring(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_visiblestring(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_bmpstring(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_printablestring(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_teletexstring(VALUE class, VALUE encoder, VALUE byte_string);

extern VALUE decode_sequence(VALUE class, VALUE encoder, VALUE sequence);
extern VALUE decode_sequence_of(VALUE class, VALUE encoder, VALUE sequence);
extern VALUE decode_choice(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE decode_enumerated(VALUE class, VALUE encoder, VALUE byte_string);
#endif
