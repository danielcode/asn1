/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>
#include <assert.h>

#include "asn_application.h"
#include "expr_types.h"

#include "util.h"
#include "unstruct.h"

#include "INTEGER.h"
#include "REAL.h"
#include "BOOLEAN.h"
#include "NULL.h"
#include "OCTET_STRING.h"
#include "IA5String.h"
#include "UTF8String.h"
#include "NumericString.h"
#include "UniversalString.h"
#include "VisibleString.h"
#include "BMPString.h"
#include "PrintableString.h"
#include "TeletexString.h"
#include "GeneralizedTime.h"
#include "BitString.h"
#include "ISO646String.h"
#include "constr_CHOICE.h"
#include "asn_SEQUENCE_OF.h"
#include "ENUMERATED.h"


/******************************************************************************/
/* Forward declarations														  */
/******************************************************************************/
VALUE instance_of_undefined(void); 

int	get_presentation_value(char *buffer, int offset, int pres_size);
static char *setter_name_from_member_name(char *name);

/******************************************************************************/
/* Externals																  */
/******************************************************************************/
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

		case ASN1_TYPE_OCTET_STRING :
			v = unstruct_octetstring(td, container);
			break;

		case ASN1_TYPE_GeneralizedTime :
			v = unstruct_generalizedtime(td, container);
			break;

		case ASN1_TYPE_IA5String :
			v = unstruct_ia5string(td, container);
			break;

		case ASN1_TYPE_UTF8String :
			v = unstruct_utf8string(td, container);
			break;

		case ASN1_TYPE_NumericString :
			v = unstruct_numericstring(td, container);
			break;

		case ASN1_TYPE_UniversalString :
			v = unstruct_universalstring(td, container);
			break;

		case ASN1_TYPE_VisibleString :
			v = unstruct_visiblestring(td, container);
			break;

		case ASN1_TYPE_BMPString :
			v = unstruct_bmpstring(td, container);
			break;

		case ASN1_TYPE_PrintableString :
			v = unstruct_printablestring(td, container);
			break;

		case ASN1_TYPE_TeletexString :
			v = unstruct_teletexstring(td, container);
			break;

		case ASN1_TYPE_BIT_STRING :
			v = unstruct_bitstring(td, container);
			break;

		case ASN1_TYPE_ISO646String :
			v = unstruct_iso646string(td, container);
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
/* OCTET STRING																  */
/******************************************************************************/
VALUE
unstruct_octetstring(asn_TYPE_descriptor_t *td, char *container)
{
	OCTET_STRING_t *octetstring = (OCTET_STRING_t *)container;

	return rb_str_new(octetstring->buf, octetstring->size);
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
/* UTF8String																  */
/******************************************************************************/
VALUE
unstruct_utf8string(asn_TYPE_descriptor_t *td, char *container)
{
	UTF8String_t *utf8string = (UTF8String_t *)container;

	return rb_str_new(utf8string->buf, utf8string->size);
}


/******************************************************************************/
/* NumericString															  */
/******************************************************************************/
VALUE
unstruct_numericstring(asn_TYPE_descriptor_t *td, char *container)
{
	NumericString_t *numericstring = (NumericString_t *)container;

	return rb_str_new(numericstring->buf, numericstring->size);
}


/******************************************************************************/
/* UniversalString															  */
/******************************************************************************/
VALUE
unstruct_universalstring(asn_TYPE_descriptor_t *td, char *container)
{
	UniversalString_t *universalstring = (UniversalString_t *)container;

	return rb_str_new(universalstring->buf, universalstring->size);
}


/******************************************************************************/
/* VisibleString															  */
/******************************************************************************/
VALUE
unstruct_visiblestring(asn_TYPE_descriptor_t *td, char *container)
{
	VisibleString_t *visiblestring = (VisibleString_t *)container;

	return rb_str_new(visiblestring->buf, visiblestring->size);
}


/******************************************************************************/
/* BMPString															  	  */
/******************************************************************************/
VALUE
unstruct_bmpstring(asn_TYPE_descriptor_t *td, char *container)
{
	BMPString_t *bmpstring = (BMPString_t *)container;

	return rb_str_new(bmpstring->buf, bmpstring->size);
}


/******************************************************************************/
/* PrintableString														  	  */
/******************************************************************************/
VALUE
unstruct_printablestring(asn_TYPE_descriptor_t *td, char *container)
{
	PrintableString_t *printablestring = (PrintableString_t *)container;

	return rb_str_new(printablestring->buf, printablestring->size);
}


/******************************************************************************/
/* TeletexString														  	  */
/******************************************************************************/
VALUE
unstruct_teletexstring(asn_TYPE_descriptor_t *td, char *container)
{
	TeletexString_t *teletexstring = (TeletexString_t *)container;

	return rb_str_new(teletexstring->buf, teletexstring->size);
}


/******************************************************************************/
/* GeneralizedTime														  	  */
/******************************************************************************/
VALUE
unstruct_generalizedtime(asn_TYPE_descriptor_t *td, char *container)
{
	GeneralizedTime_t *generalizedtime = (GeneralizedTime_t *)container;

	return rb_str_new(generalizedtime->buf, generalizedtime->size);
}


/******************************************************************************/
/* BitString																  */
/******************************************************************************/
VALUE
unstruct_bitstring(asn_TYPE_descriptor_t *td, char *container)
{
	BitString_t *bitstring = (BitString_t *)container;

	return rb_str_new(bitstring->buf, bitstring->size);
}


/******************************************************************************/
/* ISO646String																  */
/******************************************************************************/
VALUE
unstruct_iso646string(asn_TYPE_descriptor_t *td, char *container)
{
	ISO646String_t *iso646string = (ISO646String_t *)container;

	return rb_str_new(iso646string->buf, iso646string->size);
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


/******************************************************************************/
/* instance_of_undefined													  */
/******************************************************************************/
VALUE
instance_of_undefined(void)
{
	ID asn_module_id	  = rb_intern("Asn1");
    ID undefined_class_id = rb_intern("Undefined");

	VALUE asn_module	  = rb_const_get(rb_cObject, asn_module_id);
	VALUE undefined_class = rb_const_get(asn_module, undefined_class_id);

	return rb_funcall(undefined_class, rb_intern("instance"), 0, rb_ary_new2(0));
}
