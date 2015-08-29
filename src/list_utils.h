#ifndef LIST_UTILS_H
#define LIST_UTILS_H

#include <exec/lists.h>
#include <exec/types.h>

void FreeList (struct List *list_p);

uint32 GetListSize (struct List * const list_p);

BOOL SortList (struct List *list_p, int (*compare_fn) (const void *v0_p, const void *v1_p));


#endif



