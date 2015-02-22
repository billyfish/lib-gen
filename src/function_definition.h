/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File:
**
** Date: 20-09-2014 22:57:05
**
************************************************************

*/

#ifndef FUNCTION_DEFINITION_H
#define FUNCTION_DEFINITION_H

#include <dos/dos.h>
#include <exec/lists.h>

#include "parameter.h"


struct FunctionDefinition
{
	STRPTR fd_filename_s;
	uint32 fd_line_number;
	struct Parameter *fd_definition_p;
	struct List *fd_args_p;
};


struct FunctionDefinitionNode
{
	struct Node fdn_node;
	struct FunctionDefinition *fdn_function_def_p;
};




#ifdef __cplusplus
extern "C" {
#endif


uint32 GetFunctionDefinitionsListSize (struct List * const list_p);

void ClearFunctionDefinitionList (struct List *function_definitions_p);

struct FunctionDefinition *AllocateFunctionDefinition (void);

void FreeFunctionDefinition (struct FunctionDefinition *fd_p);


void FreeFunctionDefinitionNode (struct FunctionDefinitionNode *node_p);


BOOL AddParameterAtFront (struct FunctionDefinition *fd_p, struct Parameter *param_p);

BOOL AddParameterAtBack (struct FunctionDefinition *fd_p, struct Parameter *param_p);

struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s);


BOOL PrintFunctionDefinition (BPTR out_p, const struct FunctionDefinition * const fn_p);

BOOL WriteLibraryFunctionImplementation (BPTR out_p, const struct FunctionDefinition * const fd_p, CONST_STRPTR interface_struct_s);


const char *FindParameterEnd (const char *start_p, BOOL function_flag);


#ifdef __cplusplus
}
#endif




#endif		/* ifndef FUNCTION_DEFINITION_H */

