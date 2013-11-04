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
   * rm Makefile.am.sample
   * rm converter-sample.c
3. cd ../..
4. rake compile
5. rake install_gem

Using
-------
    $ irb
    > require 'asn1/asn1'
    => true
    > encoded = Asn1::Integer.encode :ber, 1234567890
    => "\x02\x04I\x96\x02\xD2"
    > Asn1::Integer.decode :ber, encoded
    => 1234567890

    > encoded = Asn1::IA5String.encode :ber, 'Hello, world!'
    => "\x16\rHello, world!"
    > Asn1::IA5String.decode :ber, encoded
    => "Hello, world!"

Classes
-------

### Asn1

#### Asn1::Integer

#### Asn1::IA5String
