/*
 *
 */
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "expr_types.h"

#include "util.h"

#include "INTEGER.h"
#include "IA5String.h"
#include "OCTET_STRING.h"
#include "SimpleSequence.h"

/*
 * Forward declarations
 */
VALUE destruct_member(asn_TYPE_member_t *member, char *member_struct);
VALUE destruct_sequence(VALUE schema_class, char *buffer);
VALUE destruct_primitive(asn_TYPE_member_t *member, char *member_struct);
VALUE asn1_destruct_integer(char *member_struct);
VALUE asn1_destruct_ia5string(char *member_struct);

static char *setter_name_from_member_name(char *name);

/*
 * Externals
 */
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);


/*
 * destruct_sequence
 */
VALUE
destruct_sequence(VALUE schema_class, char *buffer)
{
	char *symbol;
	int	  i;
	VALUE args = rb_ary_new2(0);

	asn_TYPE_descriptor_t *td = asn1_get_td_from_schema(schema_class);

	VALUE ary   = rb_ary_new();
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);
	(void)rb_ary_push(ary, class);
	(void)rb_ary_push(ary, v);

	/*
	 *
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		VALUE args;
		VALUE member_val = INT2FIX(1);

		char *member_struct;
		char *setter = setter_name_from_member_name(member->name);

		/*
		 * XXXXX - add explanation
		 */
		if (member->flags & ATF_POINTER)
		{
			/* Need to create a new one */
			rb_raise(rb_eException, "No code to de_struct ATF_POINTER");
		}
		else
		{
			member_struct = buffer + member->memb_offset;
		}

		member_val = destruct_member(member, member_struct);
		rb_funcall(v, rb_intern(setter), 1, member_val);

		free(setter);
	}

	return v;
}


VALUE
destruct_member(asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->generated == 0)
	{
		return destruct_primitive(member, member_struct);
	}
	else
	{
		return Qnil; /* XXXXX */
		/* Constructed type */
	}
}


VALUE
destruct_primitive(asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->base_type == ASN1_TYPE_INTEGER)
	{
		return asn1_destruct_integer(member_struct);
		/* rb_str_cat(tmpStr, "INTEGER\n", 8); */
	}
	else if (member->type->base_type == ASN1_TYPE_IA5String)
	{
		asn1_destruct_ia5string(member_struct);
		/* rb_str_cat(tmpStr, "IA5String\n", 10); */
	}
}


VALUE
asn1_destruct_integer(char *member_struct)
{
	long val;
	int  ret;

	INTEGER_t *integer = (INTEGER_t *)member_struct;
	ret = asn_INTEGER2long(integer, &val);

	return INT2FIX(val);
}


VALUE
asn1_destruct_ia5string(char *member_struct)
{
	IA5String_t *ia5string = (IA5String_t *)member_struct;

	return rb_str_new(ia5string->buf, ia5string->size);
}


static char *setter_name_from_member_name(char *name)
{
	int   setter_len = strlen(name) + 2;
	char *setter     = (char *)calloc(setter_len, sizeof(char));

	(void)strcat(setter, name);
	(void)strcat(setter, "=");

	return setter;
}
