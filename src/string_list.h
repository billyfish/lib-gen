#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <exec/lists.h>

#include "memory.h"


struct StringNode
{
	struct Node sn_node;
	STRPTR sn_value_s;
	MEM_FLAG sn_mem;
};


struct StringNode *AllocateStringNode (STRPTR value_s, MEM_FLAG mf);

BOOL AddStringToStringList (struct List *list_p, STRPTR str_p, const MEM_FLAG mem_flag);

void FreeStringNode (struct StringNode *node_p);



#endif		/* #ifndef STRING_LIST_H */
