/*
 *
 */

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdlib.h>
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
#include "SimpleSequence.h"

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


/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
extern	asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);
int		is_undefined(VALUE v, int base_type);
VALUE	get_optional_value(VALUE v, asn_TYPE_member_t *member);



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
/******************************************************************************/
char *
enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE sequence)
{
	char  *str;
	char  *holding_struct;
	int	  length;
	VALUE encoded;

	int i;

	/*
	 * Create a new C struct we'll use to hold data data
	 */
	holding_struct = calloc(1, td->container_size);
	if (holding_struct == NULL)
	{
		rb_raise(rb_eException, "Can't calloc memory");
	}

	/*
	 * Go encoding
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];
		VALUE v = get_optional_value(sequence, member);
		char *container =  holding_struct + member->memb_offset;
		int   is_optional_and_absent = 0;

		/*
		 * XXXXX - add explanation
		 */
		if (member->flags & ATF_POINTER)
		{
			char *val = NULL;

			if (member->optional && is_undefined(v, member->type->base_type))
			{
				*((char **)container) = NULL;
				continue; 
			}

			val = calloc(1, member->type->container_size);
			*((char **)container) = val;
			enstruct_member(v, member, val);
		}
		else
		{
			enstruct_member(v, member, container);
		}
	}

	return holding_struct;
}
