ASN.1 for Ruby
==============

Currently early stage proof-of-concept.

Works by wrapping C produced by asn1c.

Demonstrated ability to encode:
* UTF8String
* INTEGER
* SEQUENCE with IA5String and INTEGER

Decoding of UTF8String and INTEGER works, but not SEQUENCEs.

Requires modified version of asn1c, available here:
https://github.com/danielcode/asn1c

Building
--------
* asn1c must be in your path

1. cd ext/asn1
2. asn1c Test.asn1
3. cd ../..
4. rake compile
5. rake install_gem

Other instructions are ... absent for the moment.
