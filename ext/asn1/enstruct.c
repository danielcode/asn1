
/*
 *
 */
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

/*
 * Forward declarations
 */
char  *enstruct_member(VALUE v, asn_TYPE_member_t *member, char *member_struct);
char  *enstruct_primitive(VALUE v, asn_TYPE_member_t *member, char *member_struct);
void  *enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE v);
VALUE  asn1_enstruct_integer(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  asn1_enstruct_real(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  asn1_enstruct_boolean(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  asn1_enstruct_null(VALUE v, asn_TYPE_member_t *member, void *container);
VALUE  asn1_enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container);


/*
 * Externals
 */
extern asn_TYPE_descriptor_t *asn1_get_td_from_schema(VALUE class);


/*
 * enstruct_member
 * Returns C struct equivalent of corresponding ruby object
 */
char *
enstruct_member(VALUE v, asn_TYPE_member_t *member, char *member_struct)
{
	if (member->type->generated == 0)
	{
		return enstruct_primitive(v, member, member_struct);
	}
	else
	{
		rb_raise(rb_eException, "Can't (yet) enstruct complex structures");
	}	
}


/*
 * enstruct_primitive
 */
char *
enstruct_primitive(VALUE v, asn_TYPE_member_t *member, char *member_struct)
{
	asn_TYPE_descriptor_t	*element_td = member->type;

	switch(element_td->base_type)
	{
		case ASN1_TYPE_INTEGER :
			asn1_enstruct_integer(v, member, (void *)member_struct);
			break;

		case ASN1_TYPE_IA5String :
			asn1_enstruct_ia5string(v, member, (void *)member_struct);
			break;

		case ASN1_TYPE_REAL :
			asn1_enstruct_real(v, member, (void *)member_struct);
			break;

		case ASN1_TYPE_BOOLEAN :
			asn1_enstruct_boolean(v, member, (void *)member_struct);
			break;

		case ASN1_TYPE_NULL :
			asn1_enstruct_null(v, member, (void *)member_struct);
			break;

		default :
			rb_raise(rb_eStandardError, "Can't enstruct base type");
			break;
	}

	return member_struct;
}


/******************************************************************************/
/* INTEGER                                                                    */
/******************************************************************************/
VALUE
asn1_enstruct_integer(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int   result;
	VALUE memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
	long  val  = NUM2LONG(memb);

	/*
	if (!(TYPE(v) == T_FIXNUM)) {
		rb_raise(rb_eException, "Not an integer");
	}
	*/

	result = asn_long2INTEGER((REAL_t *)container, val);

	return Qnil;
}


/******************************************************************************/
/* REAL                                                                       */
/******************************************************************************/
VALUE
asn1_enstruct_real(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int		result;
	VALUE	memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
	double	val  = NUM2DBL(memb);

	/*
	if (!(TYPE(v) == T_FIXNUM)) {
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
asn1_enstruct_boolean(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int		result;
	VALUE	memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
	
	if (TYPE(memb) == T_FALSE)
	{
		*((BOOLEAN_t *)container) = 0;
	}
	else if (TYPE(memb) == T_TRUE)
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
asn1_enstruct_null(VALUE v, asn_TYPE_member_t *member, void *container)
{
	NULL_t	null = 0;
	VALUE	memb = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
	
	if (TYPE(memb) != T_NIL)
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
asn1_enstruct_ia5string(VALUE v, asn_TYPE_member_t *member, void *container)
{
	int    result;
	VALUE  memb   = rb_funcall(v, rb_intern(member->name), 0, rb_ary_new2(0));
    char  *str    = RSTRING_PTR(memb);
	int    length = RSTRING_LEN(memb);

	OCTET_STRING_t *os = NULL;

	/*
	if (!(TYPE(v) == T_STRING)) {
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
void *
enstruct_sequence(asn_TYPE_descriptor_t *td, VALUE class, VALUE v)
{
	char  *str;
	char  *holding_struct;
	int	  length;
	VALUE encoded;
	VALUE tmpStr = rb_str_new2("");

	int i;

	/*
	 * 2. Create a new C struct we'll use to hold data data
	 */
	holding_struct = calloc(1, td->container_size);
	if (holding_struct == NULL) {
		rb_raise(rb_eException, "Can't calloc memory");
	}

	/*
	 * 3. Go encoding
	 */
	for (i = 0; i < td->elements_count; i++)
	{
		asn_TYPE_member_t *member = (asn_TYPE_member_t *)&td->elements[i];

		char *member_struct;

		/*
		 * XXXXX - add explanation
		 */
		if (member->flags & ATF_POINTER)
		{
			/* Need to create a new one */
			rb_raise(rb_eException, "No code to create new");
		}
		else
		{
			member_struct = holding_struct + member->memb_offset;
		}

		enstruct_member(v, member, member_struct);
	}

    str    = RSTRING_PTR(v);
	length = RSTRING_LEN(v);

	return holding_struct;
}
