#ifndef UTIL_H
#define UTIL_H 1

extern VALUE traverse_type(VALUE class, VALUE name);

typedef struct bufferInfo
{
	char    *buffer;
	int      offset;
	int      length;
} bufferInfo_t;

#endif
