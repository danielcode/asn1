ASN.1 for Ruby
==============

Currently early stage proof-of-concept.

Works by wrapping C produced by asn1c.

Creates a separate class for each ASN.1 type.

Supports subset of primitive types and these structured types:
* SEQUENCE
* SEQUENCE OF
* CHOICE

Requires modified version of asn1c, available here:
https://github.com/danielcode/asn1c

Status
------
| ASN.1 Type        | Status      | Ruby Class            |
|-------------------|-------------|-----------------------|
| ANY               |             |                       |
| BIT_STRING        |             |                       |
| BMPString         |             |                       |
| BOOLEAN           |             | Asn1::Type::Boolean   |
| ENUMERATED        |             |                       |
| GeneralString     |             |                       |
| GeneralizedTime   |             |                       |
| GraphicString     |             |                       |
| IA5String         |             | Asn1::Type::IA5String |
| INTEGER           |             | Asn1::Type::Integer   |
| ISO646String      |             |                       |
| NULL              |             | Asn1::Type::Null      |
| NumericString     |             |                       |
| OBJECT_IDENTIFIER |             |                       |
| OCTET_STRING      |             |                       |
| ObjectDescriptor  |             |                       |
| PrintableString   |             |                       |
| REAL              |             | Asn1::Type::Real      |
| RELATIVE-OID      |             |                       |
| T61String         |             |                       |
| TeletexString     |             |                       |
| UTCTime           |             |                       |
| UTF8String        |             |                       |
| UniversalString   |             |                       |
| VideotexString    |             |                       |
| VisibleString     |             |                       |
| CHOICE            |             |                       |
| SEQUENCE          |             |                       |
| SEQUENCE_OF       |             |                       |
| SET               |             |                       |
| SET_OF            |             |                       |


Building
--------
* asn1c must be in your path

1. cd ext/asn1
2. asn1c -fall-defs-global -fwide-types Test.asn1
   * rm Makefile.am.sample
   * rm converter-sample.c
3. cd ../..
4. rake compile
5. rake install_gem

Using
-------
### Integer
    $ irb
    > require 'asn1/asn1'
    => true
    > encoded = Asn1::Integer.encode :ber, 1234567890
    => "\x02\x04I\x96\x02\xD2"
    > Asn1::Integer.decode :ber, encoded
    => 1234567890

### IA5String
    > encoded = Asn1::IA5String.encode :ber, 'Hello, world!'
    => "\x16\rHello, world!"
    > Asn1::IA5String.decode :ber, encoded
    => "Hello, world!"

### Sample Sequence
    > x = Asn1::Type::SimpleSequence.new
    => #<Asn1::Type::SimpleSequence:0x00000801ca1b20>
    > x.simpleString = "Hello, world!"
    => "Hello, world!"
    > x.simpleInt = 1
    => 1
    > y = Asn1::Schema::SimpleSequence.encode :der, x
    => "0\x12\x02\x01\x01\x16\rHello, world!"
    > Asn1::Schema::SimpleSequence.decode :der, y
    => #<Asn1::Type::SimpleSequence:0x00000801c6b728 @simpleString="Hello, world!", @simpleInt=1>

    > require 'base64'
    => true
    > puts Base64.encode64(y)
    MBICAQEWDUhlbGxvLCB3b3JsZCE=

Now, try decoding the base64 encoded string here: http://www.lapo.it/asn1js/


Classes
-------

Exclusions
----------
* Doesn't handle ambiguous CHOICE elements
* Doesn't handle anonymous SEQUENCEs
* Currently no consideration of Information Object Classes
