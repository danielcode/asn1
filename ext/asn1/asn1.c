#include <ruby.h>

extern VALUE traverse_type(VALUE class, VALUE name);
extern VALUE define_type(VALUE schema_root, VALUE type_root, char *descriptor_symbol);

extern VALUE encode_int(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_int(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE encode_real(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_real(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE encode_boolean(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_boolean(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE encode_null(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_null(VALUE class, VALUE encoder, VALUE byte_string);
extern VALUE encode_ia5string(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_ia5string(VALUE class, VALUE encoder, VALUE byte_string);


static VALUE asn1_test(VALUE mod)
{
  return rb_str_new2("Hello, world!");
}

void Init_asn1()
{
	VALUE mAsn1, mError, cAsn1, cSchema, cType;
	VALUE cEncoderTypeError;
	VALUE cInteger, cReal, cBoolean, cNull, cIA5String;
	VALUE cUndefined;

	VALUE cNewType;

	mAsn1 = rb_define_module("Asn1");
	rb_define_singleton_method(mAsn1, "asn1_test", asn1_test, 0);

	/*
	 * Asn1::Asn1 Class
	 */
	cAsn1 = rb_define_class_under(mAsn1, "Asn1", rb_cObject);
	rb_define_singleton_method(cAsn1, "traverse_type",  traverse_type, 1);
	rb_define_singleton_method(cAsn1, "define_classes", define_type, 2);

	/* ************************************************************************** */
	/* Errors         															  */
	/* ************************************************************************** */
	mError			  = rb_define_module_under(mAsn1, "Error");
	cEncoderTypeError = rb_define_class_under(mError, "EncoderTypeError", rb_eStandardError);

	/* ************************************************************************** */
	/* Schemas 																	  */
	/* ************************************************************************** */
	cSchema = rb_define_class_under(mAsn1, "Schema", rb_cObject);

	/* ************************************************************************** */
	/* Types   																	  */
	/* ************************************************************************** */
	/* mType = rb_define_module_under(mAsn1, "Type"); */
	cType = rb_define_class_under(mAsn1, "Type", rb_cObject);

	/* ************************************************************************** */
	/* Structured Types															  */
	/* ************************************************************************** */
	cNewType = define_type(cSchema, cType, "asn_DEF_SimpleSequence");

	/* ************************************************************************** */
	/* Primitive Types															  */
	/* ************************************************************************** */

	/*
	 * Integer
	 */
	cInteger = rb_define_class_under(mAsn1, "Integer", rb_cObject);
 	rb_define_const(cInteger, "TEST", rb_str_new2("anything"));

	rb_define_singleton_method(cInteger, "encode", encode_int, 2);
	rb_define_singleton_method(cInteger, "decode", decode_int, 2);

	/*
	 * Real
	 */
	cReal = rb_define_class_under(mAsn1, "Real", rb_cObject);

	rb_define_singleton_method(cReal, "encode", encode_real, 2);
	rb_define_singleton_method(cReal, "decode", decode_real, 2);

	/*
	 * Boolean
	 */
	cBoolean = rb_define_class_under(mAsn1, "Boolean", rb_cObject);

	rb_define_singleton_method(cBoolean, "encode", encode_boolean, 2);
	rb_define_singleton_method(cBoolean, "decode", decode_boolean, 2);
	
	/*
	 * Null
	 */
	cNull = rb_define_class_under(mAsn1, "Null", rb_cObject);

	rb_define_singleton_method(cNull, "encode", encode_null, 2);
	rb_define_singleton_method(cNull, "decode", decode_null, 2);

	/*
	 * IA5String
	 */
	cIA5String = rb_define_class_under(mAsn1, "IA5String", rb_cObject);

	rb_define_singleton_method(cIA5String, "encode", encode_ia5string, 2);
	rb_define_singleton_method(cIA5String, "decode", decode_ia5string, 2);

	/* ************************************************************************** */
	/* Other          															  */
	/* ************************************************************************** */
}
