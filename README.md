ASN.1 for Ruby

Currently early stage proof-of-concept.

Works by wrapping C produced by asn1c.

Demonstrated ability to encode:
o UTF8String
o INTEGER
o SEQUENCE with IA5String and INTEGER

Decoding of UTF8String and INTEGER works, but not SEQUENCEs.

Requires modified version of asn1c, available here:
https://github.com/danielcode/asn1c

Other instructions are ... absent for the moment.

I want to finish this - but I have other priorities.  If you'd also like to see it finished, drop me a note
and I'll see what I can do about putting more effort in.

