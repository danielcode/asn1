/******************************************************************************/
/* Include Files															  */
/******************************************************************************/
#include <stdlib.h>
#include <ruby.h>

#include "asn_application.h"
#include "bridge.h"

/******************************************************************************/
/* Defines																	  */
/******************************************************************************/
#define BUFSIZE 1024


/******************************************************************************/
/* Externals																  */
/******************************************************************************/
extern asn_TYPE_descriptor_t asn_DEF_REAL;


/******************************************************************************/
/* check_constraints														  */
/* Convert a ruby FLOAT to the nominated encoding.							  */
/******************************************************************************/
void
check_constraints(asn_TYPE_descriptor_t *td, void *s)
{
	int	   ret;
	size_t buflen = BUFSIZE;

	char *buffer = (char *)calloc(buflen, sizeof(char));
	if (buffer == NULL)
	{
		rb_raise(rb_eException, "Can't allocate buffer to check constraints");
	}

	ret = asn_check_constraints(td, s, buffer, &buflen);
	if (ret == -1)
	{
		/* Don't leak memory */
		VALUE exception = rb_exc_new2(rb_eException, buffer);
		free(buffer);
		rb_exc_raise(exception);
		return;
	}

	free(buffer);
}
