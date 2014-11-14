#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#if defined _DEBUG && defined _MSC_VER
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#else
	#include <stdlib.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	MF_DEEP_COPY,
	MF_SHALLOW_COPY,
	MF_SHADOW_USE,
	MF_ALREADY_FREED
} MEM_FLAG;

/*
void *AllocMemory (size_t size);

void FreeMemory (void *mem_p);

void *AllocMemoryArray (size_t num, size_t size);
*/

#ifdef __cplusplus
}
#endif


#endif		/* ifndef MEMORY_H */
