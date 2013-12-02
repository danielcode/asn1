/*
 *
 */
#include <stdlib.h>
#include <ruby.h>
#include <assert.h>

#include "asn_application.h"
#include "expr_types.h"

#include "util.h"

#include "INTEGER.h"
#include "REAL.h"
#include "BOOLEAN.h"
#include "NULL.h"
#include "IA5String.h"
#include "OCTET_STRING.h"
#include "SimpleSequence.h"

/*
 * Forward declarations
 */
VALUE unstruct_member(asn_TYPE_member_t *member, char *member_struct);
VALUE unstruct_sequence(VALUE schema_class, char *buffer);
VALUE unstruct_primitive(asn_TYPE_member_t *member, char *member_struct);

VALUE unstruct_integer(  char *member_struct);
VALUE unstruct_real(     char *member_struct);
VALUE unstruct_boolean(  char *member_struct);
VALUE unstruct_null(     char *member_struct);
VALUE unstruct_ia5string(char *member_struct);

static char *setter_name_from_member_name(char *name);

/*
 * Externals
 */
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);
extern VALUE instance_of_undefined(void); 



VALUE
unstruct_member(asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->generated == 0)
	{
		return unstruct_primitive(member, member_struct);
	}
	else
	{
		return Qnil; /* XXXXX */
		/* Constructed type */
	}
}


VALUE
unstruct_primitive(asn_TYPE_member_t *member, char *member_struct)
{
	VALUE v;

	switch(member->type->base_type)
	{
		case ASN1_TYPE_INTEGER :
			v = unstruct_integer(member_struct);
			break;

		case ASN1_TYPE_IA5String :
			v = unstruct_ia5string(member_struct);
			break;

		case ASN1_TYPE_REAL :
			v = unstruct_real(member_struct);
			break;

		case ASN1_TYPE_BOOLEAN :
			v = unstruct_boolean(member_struct);
			break;

		case ASN1_TYPE_NULL :
			v = unstruct_null(member_struct);
			break;

		default :
			rb_raise(rb_eStandardError, "Can't unstruct base type");
			break;
	}

	return v;
}


/******************************************************************************/
/* INTEGER                                                                    */
/******************************************************************************/
VALUE
unstruct_integer(char *member_struct)
{
	long val;
	int  ret;

	INTEGER_t *integer = (INTEGER_t *)member_struct;
	ret = asn_INTEGER2long(integer, &val);

	return INT2FIX(val);
}


/******************************************************************************/
/* REAL                                                                       */
/******************************************************************************/
VALUE
unstruct_real(char *member_struct)
{
	double	val;
	int		ret;

	REAL_t *real = (REAL_t *)member_struct;
	ret = asn_REAL2double(real, &val);

	return rb_float_new(val);
}


/******************************************************************************/
/* BOOLEAN                                                                    */
/******************************************************************************/
VALUE
unstruct_boolean(char *member_struct)
{
	BOOLEAN_t *bool = (BOOLEAN_t *)member_struct;

	if (*bool == 0)
	{
		return Qfalse;
	}

	return Qtrue;
}


/******************************************************************************/
/* NULL                                                                    */
/******************************************************************************/
VALUE
unstruct_null(char *member_struct)
{
	return Qnil;
}


/******************************************************************************/
/* IA5String                                                                  */
/******************************************************************************/
VALUE
unstruct_ia5string(char *member_struct)
{
	IA5String_t *ia5string = (IA5String_t *)member_struct;

	return rb_str_new(ia5string->buf, ia5string->size);
}

/******************************************************************************/
/* SEQUENCE                                                                   */
/******************************************************************************/
VALUE
unstruct_sequence(VALUE schema_class, char *buffer)
{
	SimpleSequence_t *ss = (SimpleSequence_t *)buffer;
	char *symbol;
	int	  i;
	VALUE args = rb_ary_new2(0);

	asn_TYPE_descriptor_t *td = asn1_get_td_from_schema(schema_class);

	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	/*
	 *
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		char  *member_struct;
		char  *setter = setter_name_from_member_name(member->name);
		VALUE  member_val;

		/*
		 * XXXXX - add explanation
		 */
		if (member->flags & ATF_POINTER)
		{
			char **p1 = (char **)(buffer + member->memb_offset);
			member_struct = *p1;
		}
		else
		{
			member_struct = buffer + member->memb_offset;
		}

		if (member_struct == NULL)
		{
			if (member->type->base_type == ASN1_TYPE_NULL)
			{
				assert(member->optional);
				rb_funcall(v, rb_intern(setter), 1, instance_of_undefined());
			}
			else
			{
				rb_funcall(v, rb_intern(setter), 1, Qnil);
			}
		}
		else
		{
			member_val = unstruct_member(member, member_struct);
			rb_funcall(v, rb_intern(setter), 1, member_val);
		}

		free(setter);
	}

	return v;
}

static char *setter_name_from_member_name(char *name)
{
	int   setter_len = strlen(name) + 2;
	char *setter     = (char *)calloc(setter_len, sizeof(char));

	(void)strcat(setter, name);
	(void)strcat(setter, "=");

	return setter;
}
