/******************************************************************************/
/* Include files															  */
/******************************************************************************/
#define ENSTRUCT_C 1

#include <stdlib.h>
#include <assert.h>
#include <ruby.h>

#include "asn_application.h"
#include "expr_types.h"

#include "constr_TYPE.h"
#include "INTEGER.h"
#include "REAL.h"
#include "BOOLEAN.h"
#include "NULL.h"
#include "GeneralizedTime.h"
#include "OCTET_STRING.h"
#include "IA5String.h"
#include "UTF8String.h"
#include "NumericString.h"
#include "BMPString.h"
#include "PrintableString.h"
#include "TeletexString.h"
#include "constr_CHOICE.h"
#include "asn_SEQUENCE_OF.h"

#include "enstruct.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
void enstruct_object_to_memory(VALUE v, asn_TYPE_descriptor_t *td, void *container);

void *enstruct_choice_value(VALUE value, asn_TYPE_descriptor_t *td, void *container, int id);

static char *create_holding_struct(int size);
static int	 get_id_of_choice(asn_TYPE_descriptor_t *td, VALUE choice);
VALUE		 get_choice_value_sym(VALUE choice);
VALUE		 get_choice_value(VALUE choice, VALUE sym);
VALUE		 get_optional_value(VALUE v, asn_TYPE_member_t *member);
void		 set_presentation_value(char *holding_struct, int pres_offset, int pres_size, int id);
int	 		 is_undefined(VALUE v, int base_type);


/******************************************************************************/
/* Externals																  */
/******************************************************************************/


/******************************************************************************/
/* enstruct_object															  */
/* Returns C struct equivalent of corresponding ruby object					  */
/******************************************************************************/
void *
enstruct_object(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	void *holding_struct = container;

	if (container == NULL)
	{
		holding_struct = create_holding_struct(td->container_size);
	}

	enstruct_object_to_memory(v, td, holding_struct);

	return holding_struct;
}


/******************************************************************************/
/* enstruct_object_to_memory												  */
/******************************************************************************/
void
enstruct_object_to_memory(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	assert(container != NULL);
	
	switch(td->base_type)
	{
		case ASN1_TYPE_INTEGER :
			enstruct_integer(v, td, container);
			break;

		case ASN1_TYPE_IA5String :
			enstruct_ia5string(v, td, container);
			break;

		case ASN1_TYPE_UTF8String :
			enstruct_utf8string(v, td, container);
			break;

		case ASN1_TYPE_NumericString :
			enstruct_numericstring(v, td, container);
			break;

		case ASN1_TYPE_UniversalString :
			enstruct_universalstring(v, td, container);
			break;

		case ASN1_TYPE_VisibleString :
			enstruct_visiblestring(v, td, container);
			break;

		case ASN1_TYPE_BMPString :
			enstruct_bmpstring(v, td, container);
			break;

		case ASN1_TYPE_PrintableString :
			enstruct_printablestring(v, td, container);
			break;

		case ASN1_TYPE_TeletexString :
			enstruct_teletexstring(v, td, container);
			break;

		case ASN1_TYPE_GeneralizedTime :
			enstruct_generalizedtime(v, td, container);
			break;

		case ASN1_TYPE_REAL :
			enstruct_real(v, td, container);
			break;

		case ASN1_TYPE_BOOLEAN :
			enstruct_boolean(v, td, container);
			break;

		case ASN1_TYPE_NULL :
			enstruct_null(v, td, container);
			break;

		case ASN1_TYPE_SEQUENCE :
			enstruct_sequence(v, td, container);
			break;

		case ASN1_TYPE_SEQUENCE_OF :
			enstruct_sequence_of(v, td, container);
			break;

		case ASN1_TYPE_CHOICE :
			enstruct_choice(v, td, container);
			break;

		case ASN1_TYPE_ENUMERATED :
			enstruct_enumerated(v, td, container);
			break;

		default :
			rb_raise(rb_eStandardError, "Can't enstruct base type");
			break;
	}

	return;
}


/******************************************************************************/
/* INTEGER																	  */
/******************************************************************************/
void *
enstruct_integer(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	long  val = NUM2LONG(v);
	int   result;

	if (TYPE(v) != T_FIXNUM)
	{
		rb_raise(rb_eException, "Not an integer");
	}

	result = asn_long2INTEGER((INTEGER_t *)container, val);

	return container;
}


/******************************************************************************/
/* REAL																		  */
/******************************************************************************/
void *
enstruct_real(VALUE v, asn_TYPE_descriptor_t *td, void *container)
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

	return container;
}


/******************************************************************************/
/* BOOLEAN																	  */
/******************************************************************************/
void *
enstruct_boolean(VALUE v, asn_TYPE_descriptor_t *td, void *container)
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

	return container;
}


/******************************************************************************/
/* NULL																		  */
/******************************************************************************/
void *
enstruct_null(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	NULL_t	null = 0;
	
	if (TYPE(v) != T_NIL)
	{
		rb_raise(rb_eStandardError, "Not nil");
	}

	memcpy(container, (void *)&null, sizeof(NULL_t));

	return container;
}


/******************************************************************************/
/* IA5String																  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_ia5string(VALUE v, asn_TYPE_descriptor_t *td, void *container)
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

	os = OCTET_STRING_new_fromBuf(td, (const char *)str, length);
	memcpy(container, (void *)os, sizeof(OCTET_STRING_t));

	return container;
}


/******************************************************************************/
/* UTF8String																  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_utf8string(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* NumericString															  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_numericstring(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* UniversalString															  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_universalstring(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* VisibleString															  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_visiblestring(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* BMPString															  	  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_bmpstring(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* PrintableString															  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_printablestring(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* TeletexString															  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_teletexstring(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* GeneralizedTime															  */
/* Note: assuming OCTET_STRING is constant length							  */
/******************************************************************************/
void *
enstruct_generalizedtime(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	return enstruct_ia5string(v, td, container);
}


/******************************************************************************/
/* ENUMERATED																  */
/******************************************************************************/
void *
enstruct_enumerated(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	VALUE val = rb_funcall(v, rb_intern("value"), 0, rb_ary_new2(0));
	enstruct_integer(val, NULL, container);

	return container;
}


/******************************************************************************/
/* SEQUENCE																	  */
/* Encode elements of sequence												  */
/* XXXXX - assert that TD is a sequence.									  */
/* XXXXX - class might not be required.										  */
/******************************************************************************/
void *
enstruct_sequence(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	asn_TYPE_member_t *member = NULL;

	char  *internal_container = NULL;
	VALUE  member_val;
	int	   i;

	for (i = 0; i < td->elements_count; i++)
	{
		member	   = (asn_TYPE_member_t *)&td->elements[i];
		member_val = get_optional_value(v, member);

		internal_container = (char *)container + member->memb_offset;

		enstruct_member(member_val, member, internal_container);
	}

	return container;
}


/******************************************************************************/
/* SEQUENCE_OF																  */
/* Encode items in a SEQUENCE_OF											  */
/* XXXXX - assert that TD is a sequence.									  */
/* XXXXX - assert sequence_of is an array (or enumerable)					  */
/* XXXXX - class might not be required.										  */
/* XXXXX - reference elements via "list"									  */
/******************************************************************************/
void *
enstruct_sequence_of(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	asn_TYPE_member_t *member = td->elements; /* Just one in this case */

	VALUE length_val = rb_funcall(v, rb_intern("length"), 0, rb_ary_new2(0));
	int   length	 = FIX2INT(length_val);
	VALUE  element_val, index;
	int	   i;

	assert(member->flags & ATF_POINTER); /* XXXXX - Appears to be the case */

	for (i = 0; i < length; i++)
	{
		char *element_container = NULL;

		index		= INT2FIX(i);
		element_val	= rb_funcall(v, rb_intern("[]"), 1, index);

		element_container = enstruct_object(element_val, member->type, NULL);
		asn_sequence_add(container, element_container);
	}

	return container;
}


/******************************************************************************/
/* CHOICE																	  */
/* XXXXX - Assert we have a CHOICE											  */
/******************************************************************************/
void *
enstruct_choice(VALUE v, asn_TYPE_descriptor_t *td, void *container)
{
	VALUE choice_sym, value;
	int	  id;
	
	choice_sym = get_choice_value_sym(v);
	id		   = get_id_of_choice(td, choice_sym);
	value	   = get_choice_value(v, choice_sym);

	/*
	 * Found the index of the member to encode.
	 */
	enstruct_choice_value(value, td, container, id);

	return container;
}


/******************************************************************************/
/* enstruct_choice_value													  */
/******************************************************************************/
void *
enstruct_choice_value(VALUE value, asn_TYPE_descriptor_t *td, void *container, int id)
{
	asn_CHOICE_specifics_t	*specifics = (asn_CHOICE_specifics_t *)td->specifics;
	asn_TYPE_member_t		*member	   = (asn_TYPE_member_t *)&td->elements[id];

	char *value_container = (char *)container + member->memb_offset;

	enstruct_member(value, member, value_container);
	set_presentation_value(container, specifics->pres_offset, specifics->pres_size, id + 1);

	return container;
}


/******************************************************************************/
/* enstruct_member															  */
/******************************************************************************/
void
enstruct_member(VALUE v, asn_TYPE_member_t *member, char *container)
{
	/*
	 * XXXXX - add explanation
	 */
	if (member->flags & ATF_POINTER)
	{
		char **real_container = (char **)container;
		char *member_container = NULL;

		if (member->optional && is_undefined(v, member->type->base_type))
		{
			*real_container = NULL;
		}
		else
		{
			member_container = calloc(1, member->type->container_size);
			*real_container = member_container;
			enstruct_object(v, member->type, member_container);
		}
	}
	else
	{
		enstruct_object(v, member->type, container);
	}
}


/******************************************************************************/
/* create_holding_struct													  */
/* Create a new C struct we'll use to hold data								  */
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
	 * Match 
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
/* Get the symbol of the choice's value.									  */
/******************************************************************************/
VALUE
get_choice_value_sym(VALUE choice)
{
	return rb_funcall(choice, rb_intern("asn1_choice_value"), 0, rb_ary_new2(0));
}


/******************************************************************************/
/* get_choice_value															  */
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


/******************************************************************************/
/* is_undefined																  */
/******************************************************************************/
int
is_undefined(VALUE v, int base_type)
{
	const char *c = rb_obj_classname(v);
	if (strcmp(c, "Asn1::Undefined") == 0)
	{
		return 1;
	}

	if ((TYPE(v) == T_NIL) && (base_type != ASN1_TYPE_NULL))
	{
		return 1;
	}


	return 0;
}


/******************************************************************************/
/* get_optional_value														  */
/******************************************************************************/
VALUE
get_optional_value(VALUE v, asn_TYPE_member_t *member)
{
	VALUE memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));

	if (is_undefined(v, member->type->base_type) && !member->optional)
	{
		rb_raise(rb_eStandardError, "Value absent, but not optional");
	}

	return memb;
}
