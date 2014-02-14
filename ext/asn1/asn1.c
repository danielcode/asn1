/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <ruby.h>

/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern VALUE traverse_type(VALUE class, VALUE name);
extern VALUE define_type_from_ruby(VALUE class, VALUE schema_root, VALUE schema_base, VALUE type_root, VALUE symbol);


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
extern VALUE encode_utf8string(VALUE class, VALUE encoder, VALUE v);
extern VALUE decode_utf8string(VALUE class, VALUE encoder, VALUE byte_string);


/******************************************************************************/
/* Init_asn1																  */
/******************************************************************************/
void Init_asn1()
{
	VALUE mAsn1, mError, cAsn1, cSchema, cType;
	VALUE cEncoderTypeError;
	VALUE cInteger, cReal, cBoolean, cNull, cIA5String, cUTF8String;
	VALUE cUndefined;

	VALUE cNewType, cTestChoice, cTestIntSequence, cTestEnumerated, cTestNested;

	VALUE cSymbolToSchema = rb_hash_new();;

	mAsn1 = rb_define_module("Asn1");

	/*
	 * Asn1::Asn1 Class
	 */
	cAsn1 = rb_define_class_under(mAsn1, "Asn1", rb_cObject);
	rb_define_singleton_method(cAsn1, "traverse_type",  traverse_type, 1);


	/******************************************************************************/
	/* Errors         															  */
	/******************************************************************************/
	mError			  = rb_define_module_under(mAsn1, "Error");
	cEncoderTypeError = rb_define_class_under(mError, "EncoderTypeError", rb_eStandardError);


	/******************************************************************************/
	/* Schemas 																	  */
	/******************************************************************************/
	cSchema = rb_define_class_under(mAsn1, "Schema", rb_cObject);
	rb_define_const(cSchema, "SYMBOL_TO_SCHEMA", cSymbolToSchema);
	rb_define_singleton_method(cSchema, "define_type", define_type_from_ruby, 4);


	/******************************************************************************/
	/* Types   																	  */
	/******************************************************************************/
	/* mType = rb_define_module_under(mAsn1, "Type"); */
	cType = rb_define_class_under(mAsn1, "Type", rb_cObject);


	/******************************************************************************/
	/* Structured Types															  */
	/******************************************************************************/
	/*
	cNewType		 = define_type(cSchema, cSchema, cType, "asn_DEF_SimpleSequence");
	cTestChoice		 = define_type(cSchema, cSchema, cType, "asn_DEF_TestChoice");
	cTestIntSequence = define_type(cSchema, cSchema, cType, "asn_DEF_TestIntSequence");
	cTestEnumerated	 = define_type(cSchema, cSchema, cType, "asn_DEF_TestEnumerated");
	cTestNested		 = define_type(cSchema, cSchema, cType, "asn_DEF_TestNested");
	*/


	/******************************************************************************/
	/* Primitive Types															  */
	/******************************************************************************/

	/*
	 * Integer
	 */
	cInteger = rb_define_class_under(cType, "Integer", rb_cObject);

	rb_define_singleton_method(cInteger, "encode", encode_int, 2);
	rb_define_singleton_method(cInteger, "decode", decode_int, 2);

	/*
	 * Real
	 */
	cReal = rb_define_class_under(cType, "Real", rb_cObject);

	rb_define_singleton_method(cReal, "encode", encode_real, 2);
	rb_define_singleton_method(cReal, "decode", decode_real, 2);

	/*
	 * Boolean
	 */
	cBoolean = rb_define_class_under(cType, "Boolean", rb_cObject);

	rb_define_singleton_method(cBoolean, "encode", encode_boolean, 2);
	rb_define_singleton_method(cBoolean, "decode", decode_boolean, 2);
	
	/*
	 * Null
	 */
	cNull = rb_define_class_under(cType, "Null", rb_cObject);

	rb_define_singleton_method(cNull, "encode", encode_null, 2);
	rb_define_singleton_method(cNull, "decode", decode_null, 2);

	/*
	 * IA5String
	 */
	cIA5String = rb_define_class_under(cType, "IA5String", rb_cObject);

	rb_define_singleton_method(cIA5String, "encode", encode_ia5string, 2);
	rb_define_singleton_method(cIA5String, "decode", decode_ia5string, 2);

	/*
	 * UTF8String
	 */
	cUTF8String = rb_define_class_under(cType, "UTF8String", rb_cObject);

	rb_define_singleton_method(cUTF8String, "encode", encode_utf8string, 2);
	rb_define_singleton_method(cUTF8String, "decode", decode_utf8string, 2);
}
