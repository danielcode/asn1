#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>	/* For dlopen, etc. */

#include "asn_application.h"

/*
 * Defines
 */
#define NUM_SHARED_OBJECTS 11

#define PRINT_TABS(stream, level) \
	{ \
		int i; \
		for (i = 0; i < level; i++) \
		{ \
			fprintf(stream, "\t"); \
		} \
	}

/*
 * Forward declarations
 */
void   traverse_type(asn_TYPE_descriptor_t *td, int level);
void **load_shared_objects(const char *name[]);
void  *load_shared_object(const char *name, int mode);
void   unload_shared_objects(void **sos);

/*
 * Static
 */
static const char *shared_objects[] =
{
	"./asn_codecs_prim.so",
	"./constraints.so",
	"./OCTET_STRING.so",
	"./INTEGER.so",
	"./NativeInteger.so",
	"./UTF8String.so",
	"./constr_SEQUENCE.so",
	"./TestType.so",
	"./Test2Type.so",
	"./TestSequence.so",
	"./AnonSequence.so"
};

int
main(int argc, char **argv)
{
	asn_TYPE_descriptor_t *td = NULL;

	void  *sym = NULL;
	void **sos = load_shared_objects(shared_objects);

	/*
	 * Load all shared objects
	 */
	if (!sos)
	{
		exit(1);
	}


	/*
	 * Find an interesting symbol
	 */
	sym = dlsym(RTLD_DEFAULT, "asn_DEF_AnonSequence");
	if (!sym)
	{
		fprintf(stderr, "Can't find symbol asn_DEF_AnonSequence: %s\n", dlerror());
		unload_shared_objects(sos);
		free(sos);
	}
	td = (asn_TYPE_descriptor_t *)sym;

	/*
	 * Go type spelunking
	 */
	traverse_type(td, 0);

	/* fprintf(stderr, "Type name: %s\n", td->name); */

	unload_shared_objects(sos);
	free(sos);
	exit(0);
}


void
traverse_type(asn_TYPE_descriptor_t *td, int level)
{
	int i;

	PRINT_TABS(stdout, level);
	fprintf(stdout, "Type Name: %s\n", td->name);

	if (td->elements_count > 0)
	{
		PRINT_TABS(stdout, level);
		fprintf(stdout, "{\n");

		for (i = 0; i < td->elements_count; i++)
		{
			PRINT_TABS(stdout, level + 1);
			fprintf(stdout, "%s", td->elements[i].name);
			traverse_type(td->elements[i].type, level + 1);
		}

		fprintf(stdout, "}\n");
	}
}


void **
load_shared_objects(const char *name[])
{
	int   i;

	void **sos = calloc(NUM_SHARED_OBJECTS, sizeof(void *));
	if (!sos)
	{
		return NULL;
	}

	for (i = 0; i < NUM_SHARED_OBJECTS; i++)
	{
		sos[i] = NULL;

		sos[i] = load_shared_object(shared_objects[i], RTLD_LAZY | RTLD_GLOBAL);
		if (!sos[i])
		{
			fprintf(stderr, "Failed to open shared object %s: %s\n", shared_objects[i], dlerror());
			unload_shared_objects(sos);
			free(sos);
			return NULL;
		}
	}

	return sos;
}

void
unload_shared_objects(void **sos)
{
	int i;

	for (i = 0; i < NUM_SHARED_OBJECTS; i++)
	{
		if (sos[i] && dlclose(sos[i]))
		{
			fprintf(stderr, "Failed to close shared object %s: %s\n", shared_objects[i], dlerror());
		}
	}

	return;
}

void *
load_shared_object(const char *name, int mode)
{
	return dlopen(name, mode);
}
