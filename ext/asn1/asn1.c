/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <ruby.h>
#include "asn_application.h"
#include "util.h"
#include "define_type.h"
#include "encode.h"
#include "decode.h"

/******************************************************************************/
/* Init_asn1																  */
/******************************************************************************/
void Init_asn1()
{
	VALUE mAsn1, mError, cAsn1, cSchema, cType;
	VALUE cEncoderTypeError;
	VALUE cInteger, cReal, cBoolean, cNull, cIA5String, cUTF8String, cNumericString;
	VALUE cUniversalString, cVisibleString;
	VALUE cUndefined;

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

	/*
	 * NumericString
	 */
	cNumericString = rb_define_class_under(cType, "NumericString", rb_cObject);

	rb_define_singleton_method(cNumericString, "encode", encode_numericstring, 2);
	rb_define_singleton_method(cNumericString, "decode", decode_numericstring, 2);

	/*
	 * UniversalString
	 */
	cUniversalString = rb_define_class_under(cType, "UniversalString", rb_cObject);

	rb_define_singleton_method(cUniversalString, "encode", encode_universalstring, 2);
	rb_define_singleton_method(cUniversalString, "decode", decode_universalstring, 2);

	/*
	 * VisibleString
	 */
	cVisibleString = rb_define_class_under(cType, "VisibleString", rb_cObject);

	rb_define_singleton_method(cVisibleString, "encode", encode_visiblestring, 2);
	rb_define_singleton_method(cVisibleString, "decode", decode_visiblestring, 2);
}
