#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *AllocMemory (size_t size);

void FreeMemory (void *mem_p);

void *AllocMemoryArray (size_t num, size_t size);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef MEMORY_H */
