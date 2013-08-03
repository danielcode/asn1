#!/bin/sh
cc -I. -fPIC -shared -o asn_codecs_prim.so asn_codecs_prim.c
cc -I. -fPIC -shared -o constraints.so constraints.c
cc -I. -fPIC -shared -o OCTET_STRING.so OCTET_STRING.c
cc -I. -fPIC -shared -o INTEGER.so INTEGER.c
cc -I. -fPIC -shared -o NativeInteger.so NativeInteger.c
cc -I. -fPIC -shared -o UTF8String.so UTF8String.c
cc -I. -fPIC -shared -o constr_SEQUENCE.so constr_SEQUENCE.c
cc -I. -fPIC -shared -o TestType.so TestType.c
cc -I. -fPIC -shared -o Test2Type.so Test2Type.c
cc -I. -fPIC -shared -o TestSequence.so TestSequence.c
cc -I. -fPIC -shared -o AnonSequence.so AnonSequence.c
cc -I. -fPIC -shared -o test.o test.c
cc -I. -o test test.c -lc
