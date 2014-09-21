#include <stdlib.h>

#include "memory.h"


void *AllocMemory (size_t size)
{
	return malloc (size);
}


void FreeMemory (void *mem_p)
{
	free (mem_p);
}


void *AllocMemoryArray (size_t num, size_t size)
{
	return calloc (num, size);
}

