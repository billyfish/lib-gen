#ifndef STRING_LISTNODE_H
#define STRING_LISTNODE_H

#include <dos/dos.h>

#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>

enum FdNodeTypes
{
	NT_FUNCTION_TYPE = 240,
	NT_FUNCTION_NAME,
	NT_PARAM_TYPE,
	NT_PRAM_NAME,
	NT_FUNCTION_POINTER,
	NT_OPENING_BRACKET,
	NT_CLOSING_BRACKET,
	NT_DEREFERENCE
};


#ifdef __cplusplus
extern "C" 
{
#endif

struct Node *AllocateStringNode (CONST_STRPTR start_p, CONST_STRPTR end_p, uint8 node_type);


void FreeStringListNode (struct Node *node_p);

struct List *AllocateStringList (void);

void FreeStringList (struct List *list_p);

void PrintNodeNameToFileHandle (BPTR file_p, struct Node *node_p);

void PrintStringListToFileHandle (BPTR file_p, struct List *list_p);


BOOL AddStringNode (struct List *tokens_p, CONST_STRPTR start_p, CONST_STRPTR end_p, uint8 node_type);



#ifdef __cplusplus
}
#endif


#endif		/* #ifndef STRING_LISTNODE_H */

