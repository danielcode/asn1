/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
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
#include "constr_CHOICE.h"
#include "asn_SEQUENCE_OF.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
VALUE unstruct_member(asn_TYPE_member_t *member, char *member_struct);
VALUE unstruct_sequence(VALUE schema_class, char *buffer);
VALUE unstruct_primitive(asn_TYPE_member_t *member, char *member_struct);

VALUE unstruct_integer(  char *member_struct);
VALUE unstruct_real(     char *member_struct);
VALUE unstruct_boolean(  char *member_struct);
VALUE unstruct_null(     char *member_struct);
VALUE unstruct_ia5string(char *member_struct);

VALUE unstruct_sequence(VALUE schema_class,    char *buffer);
VALUE unstruct_sequence_of(VALUE schema_class, char *buffer);
VALUE unstruct_choice(VALUE schema_class,      char *buffer);
VALUE unstruct_enumerated(VALUE schema_class,  char *buffer);

void  unstruct_struct_to_value(asn_TYPE_member_t *member, VALUE v, char *buffer);

int			 get_presentation_value(char *buffer, int offset, int pres_size);
static char *setter_name_from_member_name(char *name);


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);
extern VALUE instance_of_undefined(void); 


/******************************************************************************/
/* unstruct_member															  */
/******************************************************************************/
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


/******************************************************************************/
/* unstruct_primitive														  */
/******************************************************************************/
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
/* INTEGER																	  */
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
/* REAL																		  */
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
/* BOOLEAN																	  */
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
/* NULL																		  */
/******************************************************************************/
VALUE
unstruct_null(char *member_struct)
{
	return Qnil;
}


/******************************************************************************/
/* IA5String																  */
/******************************************************************************/
VALUE
unstruct_ia5string(char *member_struct)
{
	IA5String_t *ia5string = (IA5String_t *)member_struct;

	return rb_str_new(ia5string->buf, ia5string->size);
}


/******************************************************************************/
/* SEQUENCE																	  */
/******************************************************************************/
VALUE
unstruct_sequence(VALUE schema_class, char *buffer)
{
	VALUE args = rb_ary_new2(0);
	int	  i;

	asn_TYPE_descriptor_t *td = asn1_get_td_from_schema(schema_class);

	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	/*
	 *
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		unstruct_struct_to_value(member, v, buffer);
	}

	return v;
}


/******************************************************************************/
/* SEQUENCE_OF																  */
/******************************************************************************/
VALUE
unstruct_sequence_of(VALUE schema_class, char *buffer)
{
	VALUE  args	= rb_ary_new2(0);
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	asn_anonymous_sequence_ *sequence_of = (asn_anonymous_sequence_ *)buffer;
	asn_TYPE_descriptor_t	*td			 = asn1_get_td_from_schema(schema_class);
	asn_TYPE_member_t		*member		 = td->elements;

	int	  i;
	int	  length = sequence_of->count;

/*
	VALUE class_name = rb_funcall(class, rb_intern("to_s"), 0);
	char *check = RSTRING_PTR(class_name);
*/


	/*
	 *
	 */
	for (i = 0; i < length; i++)
	{
		VALUE element;
		char *element_buffer = (char *)sequence_of->array[i];
		element = unstruct_member(member, element_buffer);
		rb_funcall(v, rb_intern("push"), 1, element);
	}

	return v;
}


/******************************************************************************/
/* CHOICE																	  */
/******************************************************************************/
VALUE
unstruct_choice(VALUE schema_class, char *buffer)
{
	VALUE  args	  = rb_ary_new2(0);
	VALUE  choice;
	int	   index;

	asn_TYPE_descriptor_t  *td		  = asn1_get_td_from_schema(schema_class);
    asn_CHOICE_specifics_t *specifics = (asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t	   *member;

	index  = get_presentation_value(buffer, specifics->pres_offset, specifics->pres_size);
	member = (asn_TYPE_member_t *)&td->elements[index];

	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	/*
	 * asn1_choice_value
	 */
	unstruct_struct_to_value(member, v, buffer);

	choice = ID2SYM(rb_intern(member->name));
	(void)rb_funcall(v, rb_intern("asn1_choice_value="), 1, choice);

	return v;
}


/******************************************************************************/
/* EUMERATED																  */
/******************************************************************************/
VALUE
unstruct_enumerated(VALUE schema_class, char *buffer)
{
	long val;
	int  ret;

	VALUE args	= rb_ary_new2(0);
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	INTEGER_t *integer = (INTEGER_t *)buffer;
	ret = asn_INTEGER2long(integer, &val);

	(void)rb_funcall(v, rb_intern("value="), 1, LONG2FIX(val));

	return v;
}


/******************************************************************************/
/* unstruct_struct_to_value													  */
/******************************************************************************/
void
unstruct_struct_to_value(asn_TYPE_member_t *member, VALUE v, char *buffer)
{
	char  *setter = setter_name_from_member_name(member->name);
	char  *member_struct;
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


/******************************************************************************/
/* setter_name_from_member_name												  */
/******************************************************************************/
static char *
setter_name_from_member_name(char *name)
{
	int   setter_len = strlen(name) + 2;
	char *setter     = (char *)calloc(setter_len, sizeof(char));

	(void)strcat(setter, name);
	(void)strcat(setter, "=");

	return setter;
} 


/******************************************************************************/
/* get_presentation_value													  */
/* XXXXX - broken; doesn't account for differing pres_size values			  */
/******************************************************************************/
int
get_presentation_value(char *buffer, int offset, int pres_size)
{
    int *pres_ptr = (int *)(buffer + offset);
	int  index	  = *pres_ptr;

    return index - 1;
}
