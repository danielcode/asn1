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
#include "ENUMERATED.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
VALUE unstruct_object(asn_TYPE_descriptor_t *td,		char *container);

VALUE unstruct_integer(asn_TYPE_descriptor_t *td,		char *container);
VALUE unstruct_real(asn_TYPE_descriptor_t *td,			char *container);
VALUE unstruct_boolean(asn_TYPE_descriptor_t *td,		char *container);
VALUE unstruct_null(asn_TYPE_descriptor_t *td,			char *container);
VALUE unstruct_ia5string(asn_TYPE_descriptor_t *td,		char *container);

VALUE unstruct_sequence(asn_TYPE_descriptor_t *td,		char *container);
VALUE unstruct_sequence_of(asn_TYPE_descriptor_t *td,	char *container);
VALUE unstruct_choice(asn_TYPE_descriptor_t *td,		char *container);
VALUE unstruct_enumerated(asn_TYPE_descriptor_t *td,	char *container);

void  unstruct_member(VALUE v, asn_TYPE_member_t *member, char *buffer);

int	get_presentation_value(char *buffer, int offset, int pres_size);
static char *setter_name_from_member_name(char *name);


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern VALUE instance_of_undefined(void); 
extern VALUE get_schema_from_td_string(char *symbol);


/******************************************************************************/
/* unstruct_object															  */
/******************************************************************************/
VALUE
unstruct_object(asn_TYPE_descriptor_t *td, char *container)
{
	VALUE v;

	switch(td->base_type)
	{
		case ASN1_TYPE_INTEGER :
			v = unstruct_integer(td, container);
			break;

		case ASN1_TYPE_IA5String :
			v = unstruct_ia5string(td, container);
			break;

		case ASN1_TYPE_REAL :
			v = unstruct_real(td, container);
			break;

		case ASN1_TYPE_BOOLEAN :
			v = unstruct_boolean(td, container);
			break;

		case ASN1_TYPE_NULL :
			v = unstruct_null(td, container);
			break;

		case ASN1_TYPE_SEQUENCE :
			v = unstruct_sequence(td, container);
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
unstruct_integer(asn_TYPE_descriptor_t *td, char *container)
{
	long val;
	int  ret;

	INTEGER_t *integer = (INTEGER_t *)container;
	ret = asn_INTEGER2long(integer, &val);

	return INT2FIX(val);
}


/******************************************************************************/
/* REAL																		  */
/******************************************************************************/
VALUE
unstruct_real(asn_TYPE_descriptor_t *td, char *container)
{
	double	val;
	int		ret;

	REAL_t *real = (REAL_t *)container;
	ret = asn_REAL2double(real, &val);

	return rb_float_new(val);
}


/******************************************************************************/
/* BOOLEAN																	  */
/******************************************************************************/
VALUE
unstruct_boolean(asn_TYPE_descriptor_t *td, char *container)
{
	BOOLEAN_t *bool = (BOOLEAN_t *)container;

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
unstruct_null(asn_TYPE_descriptor_t *td, char *container)
{
	return Qnil;
}


/******************************************************************************/
/* IA5String																  */
/******************************************************************************/
VALUE
unstruct_ia5string(asn_TYPE_descriptor_t *td, char *container)
{
	IA5String_t *ia5string = (IA5String_t *)container;

	return rb_str_new(ia5string->buf, ia5string->size);
}


/******************************************************************************/
/* SEQUENCE																	  */
/******************************************************************************/
VALUE
unstruct_sequence(asn_TYPE_descriptor_t *td, char *container)
{
	VALUE schema_class = get_schema_from_td_string(td->symbol);

	VALUE args	= rb_ary_new2(0);
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);
	int	  i;

	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		unstruct_member(v, member, container);
	}

	return v;
}


/******************************************************************************/
/* SEQUENCE_OF																  */
/******************************************************************************/
VALUE
unstruct_sequence_of(asn_TYPE_descriptor_t *td, char *container)
{
	VALUE schema_class = get_schema_from_td_string(td->symbol);

	VALUE  args	= rb_ary_new2(0);
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	asn_anonymous_sequence_ *sequence_of = (asn_anonymous_sequence_ *)container;
	asn_TYPE_member_t		*member		 = td->elements;

	int	  i;
	int	  length = sequence_of->count;

	/*
	 *
	 */
	for (i = 0; i < length; i++)
	{
		char  *element_buffer = (char *)sequence_of->array[i];
		VALUE  element		  = unstruct_object(member->type, element_buffer);

		rb_funcall(v, rb_intern("push"), 1, element);
	}

	return v;
}


/******************************************************************************/
/* CHOICE																	  */
/******************************************************************************/
VALUE
unstruct_choice(asn_TYPE_descriptor_t *td, char *container)
{
	VALUE schema_class = get_schema_from_td_string(td->symbol);

	VALUE  args	  = rb_ary_new2(0);
	VALUE  choice;
	int	   index;

    asn_CHOICE_specifics_t *specifics = (asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t	   *member;

	index  = get_presentation_value(container, specifics->pres_offset, specifics->pres_size);
	member = (asn_TYPE_member_t *)&td->elements[index];

	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	/*
	 * asn1_choice_value
	 */
	unstruct_member(v, member, container);

	choice = ID2SYM(rb_intern(member->name));
	(void)rb_funcall(v, rb_intern("asn1_choice_value="), 1, choice);

	return v;
}


/******************************************************************************/
/* EUMERATED																  */
/******************************************************************************/
VALUE
unstruct_enumerated(asn_TYPE_descriptor_t *td, char *container)
{
	VALUE schema_class = get_schema_from_td_string(td->symbol);

	long val;
	int  ret;

	VALUE args	= rb_ary_new2(0);
	VALUE class = rb_const_get(schema_class, rb_intern("CANDIDATE_TYPE"));
	VALUE v     = rb_class_new_instance(0, &args, class);

	INTEGER_t *integer = (INTEGER_t *)container;
	ret = asn_INTEGER2long(integer, &val);

	(void)rb_funcall(v, rb_intern("value="), 1, LONG2FIX(val));

	return v;
}


/******************************************************************************/
/* unstruct_member													  */
/******************************************************************************/
void
unstruct_member(VALUE v, asn_TYPE_member_t *member, char *container)
{
	char  *setter = setter_name_from_member_name(member->name);
	char  *member_struct;
	VALUE  member_val;

	/*
	 * XXXXX - add explanation
	 */
	if (member->flags & ATF_POINTER)
	{
		char **p1 = (char **)(container + member->memb_offset);
		member_struct = *p1;
	}
	else
	{
		member_struct = container + member->memb_offset;
	}

	if (member_struct == NULL)
	{
		/*
		 * Deal with NULL and NULL OPTIONAL case
		 */
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
		member_val = unstruct_object(member->type, member_struct);
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
