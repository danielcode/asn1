/*
 *
 */

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <ruby.h>

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
/* Include files                                                              */
/******************************************************************************/
char  *enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE v);
void  *enstruct_member(VALUE v,    asn_TYPE_member_t *member, void *container);
void  *enstruct_primitive(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  enstruct_integer(VALUE v,   asn_TYPE_member_t *member, void *container);
VALUE  enstruct_real(VALUE v,      asn_TYPE_member_t *member, void *container);
VALUE  enstruct_boolean(VALUE v,   asn_TYPE_member_t *member, void *container);
VALUE  enstruct_null(VALUE v,      asn_TYPE_member_t *member, void *container);
VALUE  enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container);

void	 enstruct_value_to_struct(VALUE v, asn_TYPE_member_t *member, char *container);
char	*enstruct_choice_value(asn_TYPE_descriptor_t *td, int id, VALUE value);

static char *create_holding_struct(int size);
static int	 get_id_of_choice(asn_TYPE_descriptor_t *td, VALUE choice);
VALUE		 get_choice_value_sym(VALUE choice);
VALUE		 get_choice_value(VALUE choice, VALUE sym);
void		 set_presentation_value(char *holding_struct, int pres_offset, int pres_size, int id);


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);
extern int		is_undefined(VALUE v, int base_type);
extern VALUE	get_optional_value(VALUE v, asn_TYPE_member_t *member);


/*
 * enstruct_member
 * Returns C struct equivalent of corresponding ruby object
 */
void *
enstruct_member(VALUE v, asn_TYPE_member_t *member, void *container)
{
	if (member->type->generated == 0)
	{
		return enstruct_primitive(v, member, container);
	}
	else
	{
		rb_raise(rb_eException, "Can't (yet) enstruct complex structures");
	}	
}


/*
 * enstruct_primitive
 */
void *
enstruct_primitive(VALUE v, asn_TYPE_member_t *member, void *container)
{
	asn_TYPE_descriptor_t	*element_td = member->type;

	switch(element_td->base_type)
	{
		case ASN1_TYPE_INTEGER :
			enstruct_integer(v, member, container);
			break;

		case ASN1_TYPE_IA5String :
			enstruct_ia5string(v, member, container);
			break;

		case ASN1_TYPE_REAL :
			enstruct_real(v, member, container);
			break;

		case ASN1_TYPE_BOOLEAN :
			enstruct_boolean(v, member, container);
			break;

		case ASN1_TYPE_NULL :
			enstruct_null(v, member, container);
			break;

		default :
			rb_raise(rb_eStandardError, "Can't enstruct base type");
			break;
	}

	return container;
}


/******************************************************************************/
/* INTEGER                                                                    */
/******************************************************************************/
VALUE
enstruct_integer(VALUE v, asn_TYPE_member_t *member, void *container)
{
	long  val = NUM2LONG(v);
	int   result;

	if (TYPE(v) != T_FIXNUM)
	{
		rb_raise(rb_eException, "Not an integer");
	}

	result = asn_long2INTEGER((REAL_t *)container, val);

	return Qnil;
}


/******************************************************************************/
/* REAL                                                                       */
/******************************************************************************/
VALUE
enstruct_real(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int		result;
	double	val  = NUM2DBL(v);

	/*
	if (!(TYPE(v) == T_FIXNUM))
	{
		rb_raise(rb_eException, "Not an integer");
	}
	*/

	result = asn_double2REAL((REAL_t *)container, val);

	return Qnil;
}

/******************************************************************************/
/* BOOLEAN                                                                    */
/******************************************************************************/
VALUE
enstruct_boolean(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int		result;
	
	if (TYPE(v) == T_FALSE)
	{
		*((BOOLEAN_t *)container) = 0;
	}
	else if (TYPE(v) == T_TRUE)
	{
		*((BOOLEAN_t *)container) = 1;
	}
	else
	{
		rb_raise(rb_eStandardError, "Not a boolean class");
	}

	return Qnil;
}

/******************************************************************************/
/* NULL                                                                       */
/******************************************************************************/
VALUE
enstruct_null(VALUE v, asn_TYPE_member_t *member, void *container)
{
	NULL_t	null = 0;
	
	if (TYPE(v) != T_NIL)
	{
		rb_raise(rb_eStandardError, "Not nil");
	}

	memcpy(container, (void *)&null, sizeof(NULL_t));

	return Qnil;
}

/******************************************************************************/
/* IA5String                                                                  */
/* Note: assuming OCTET_STRING is constant length                             */
/******************************************************************************/
VALUE
enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container)
{
    char  *str    = RSTRING_PTR(v);
	int    length = RSTRING_LEN(v);
	int    result;

	OCTET_STRING_t *os = NULL;

	/*
	if (!(TYPE(v) == T_STRING))
	{
		rb_raise(rb_eException, "Not a string");
	}
	*/

	os = OCTET_STRING_new_fromBuf(member->type, (const char *)str, length);
	memcpy(container, (void *)os, sizeof(OCTET_STRING_t));

	return Qnil;
}

/******************************************************************************/
/* SEQUENCE                                                                   */
/* Encode elements of sequence												  */
/* XXXXX - assert that TD is a sequence.                                      */
/* XXXXX - class might not be required.                                       */
/******************************************************************************/
char *
enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE sequence)
{
	char *holding_struct = create_holding_struct(td->container_size);

	asn_TYPE_member_t *member = NULL;

	char  *container;
	VALUE  v;
	int	   i;

	for (i = 0; i < td->elements_count; i++)
	{
		member	  = (asn_TYPE_member_t *)&td->elements[i];
		v 		  = get_optional_value(sequence, member);
		container =  holding_struct + member->memb_offset;

		enstruct_value_to_struct(v, member, container);
	}

	return holding_struct;
}


/******************************************************************************/
/* SEQUENCE_OF																  */
/* Encode items in a SEQUENCE_OF											  */
/* XXXXX - assert that TD is a sequence.                                      */
/* XXXXX - assert sequence_of is an array (or enumerable)					  */
/* XXXXX - class might not be required.                                       */
/******************************************************************************/
char *
enstruct_sequence_of(asn_TYPE_descriptor_t *td, VALUE class, VALUE sequence_of)
{
	char *holding_struct = create_holding_struct(td->container_size);
	asn_TYPE_member_t *member = td->elements; /* Just one in this case */
	VALUE length_val = rb_funcall(sequence_of, rb_intern("length"), 0, rb_ary_new2(0));
	int   length = FIX2INT(length_val);

	assert(member->flags & ATF_POINTER); /* XXXXX - Appears to be the case */

	VALUE  v, index;
	int	   i;

	for (i = 0; i < length; i++)
	{
		int fixval;
		char *element_container = NULL;

		index = INT2FIX(i);
		v 	  = rb_funcall(sequence_of, rb_intern("[]"), 1, index);
		fixval = FIX2INT(v);

		enstruct_value_to_struct(v, member, (char *)&element_container);
		asn_sequence_add(holding_struct, element_container);
	}

	return holding_struct;
}


/******************************************************************************/
/* CHOICE                                                                     */
/* XXXXX - Assert we have a CHOICE                                            */
/******************************************************************************/
char *
enstruct_choice(asn_TYPE_descriptor_t *td, VALUE class, VALUE choice)
{
	VALUE choice_sym, value;
	int	  id;
	
	choice_sym = get_choice_value_sym(choice);
	id		   = get_id_of_choice(td, choice_sym);
	value	   = get_choice_value(choice, choice_sym);

	/*
	 * Found the index of the member to encode.
	 */
	return enstruct_choice_value(td, id, value);
}


/******************************************************************************/
/* enstruct_choice_value													  */
/******************************************************************************/
char *
enstruct_choice_value(asn_TYPE_descriptor_t *td, int id, VALUE value)
{
	asn_TYPE_member_t		*member	   = (asn_TYPE_member_t *)&td->elements[id];
	asn_CHOICE_specifics_t	*specifics = (asn_CHOICE_specifics_t *)td->specifics;

	char *holding_struct  = create_holding_struct(specifics->struct_size);
	char *value_container = holding_struct + member->memb_offset;

	enstruct_value_to_struct(value, member, value_container);
	set_presentation_value(holding_struct, specifics->pres_offset, specifics->pres_size, id + 1);
}


/******************************************************************************/
/* enstruct_value_to_struct                                                   */
/******************************************************************************/
void
enstruct_value_to_struct(VALUE v, asn_TYPE_member_t *member, char *container)
{
	/*
	 * XXXXX - add explanation
	 */
	if (member->flags & ATF_POINTER)
	{
		char *val = NULL;

		if (member->optional && is_undefined(v, member->type->base_type))
		{
			*((char **)container) = NULL;
		}
		else
		{
			val = calloc(1, member->type->container_size);
			*((char **)container) = val;
			enstruct_member(v, member, val);
		}
	}
	else
	{
		enstruct_member(v, member, container);
	}
}


/******************************************************************************/
/* create_holding_struct                                                      */
/* Create a new C struct we'll use to hold data                               */
/******************************************************************************/
static char *
create_holding_struct(int size)
{
	char *holding_struct = calloc(1, size);
	if (holding_struct == NULL)
	{
		rb_raise(rb_eException, "Can't calloc memory");
	}

	return holding_struct;
}

/******************************************************************************/
/* get_id_of_choice															  */
/* Determine the identifier of the choice selected.							  */
/******************************************************************************/
static int
get_id_of_choice(asn_TYPE_descriptor_t *td, VALUE choice_value_sym)
{
	VALUE  choice_str;
	char  *choice_name;
	int	   i = 0, found = 0;

	choice_str = rb_funcall(choice_value_sym, rb_intern("to_s"), 0, rb_ary_new2(0));

	/*
	 * 2. Match 
	 */
	if (choice_str == Qnil)
	{
		return 0;
	}

	choice_name = RSTRING_PTR(choice_str);
	for (i = 0; i < td->elements_count && found == 0; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		if (strcmp(member->name, choice_name) == 0)
		{
			found = 1;
			break;
		}
	}

	if (found == 0)
	{
		rb_raise(rb_eException, "Can't find choice");
	}

	return i;
}


/******************************************************************************/
/* get_choice_value_sym														  */
/* Get the symbol of the choice's value.            						  */
/******************************************************************************/
VALUE
get_choice_value_sym(VALUE choice)
{
	return rb_funcall(choice, rb_intern("asn1_choice_value"), 0, rb_ary_new2(0));
}


/******************************************************************************/
/* get_choice_value    														  */
/* Get the value of the choice												  */
/******************************************************************************/
VALUE
get_choice_value(VALUE choice, VALUE sym)
{
	return rb_funcall(choice, SYM2ID(sym), 0, rb_ary_new2(0));
}


/******************************************************************************/
/* set_presentation_value													  */
/* XXXXX - broken; doesn't account for differing pres_size values			  */
/******************************************************************************/
void
set_presentation_value(char *holding_struct, int pres_offset, int pres_size, int id)
{
	int *pres_ptr = (int *)(holding_struct + pres_offset);

	*pres_ptr = id;
}
