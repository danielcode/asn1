#ifndef ENCODE_H
extern VALUE encode_sequence(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_sequence(VALUE class, VALUE encoder, VALUE sequence);
extern VALUE encode_sequence_of(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_sequence_of(VALUE class, VALUE encoder, VALUE sequence);
extern VALUE encode_choice(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_choice(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE encode_enumerated(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_enumerated(VALUE class, VALUE encoder, VALUE byte_string);
#endif
