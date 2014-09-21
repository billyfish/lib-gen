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
	struct Parameter *fd_definition_p;
	struct List *fd_args_p;
};


struct FunctionDefinitionNode
{
	struct Node fdn_node;
	struct FunctionDefinition *fdn_function_def_p;
};


/**
 * All of the functions defined in a given header file.
 */
struct HeaderDefinitions
{
	/** The name of the file. */
	STRPTR hd_filename_s;
	
	/** 
	 * List of FunctionDefinitionNodes from this file.
	 */
	struct List hd_function_definitions;
};

struct HeaderDefinitionsNode 
{
	struct Node hdn_node;
	struct HeaderDefinitions *hdn_defs_p;
};



#ifdef __cplusplus
extern "C" {
#endif


struct HeaderDefinitions *AllocateHeaderDefinitions (STRPTR filename_s);

void FreeHeaderDefinitions (struct HeaderDefinitions *header_defs_p);


struct List *AllocateFunctionDefinitionsList (void);

void FreeFunctionDefinitionsList (struct List *fds_p);

BOOL AddFunctionDefinitionToList (struct List *list_p, struct FunctionDefinition *fd_p);

struct FunctionDefinition *AllocateFunctionDefinition (void);

void FreeFunctionDefinition (struct FunctionDefinition *fd_p);

BOOL AddParameterAtFront (struct FunctionDefinition *fd_p, struct Parameter *param_p);

BOOL AddParameterAtBack (struct FunctionDefinition *fd_p, struct Parameter *param_p);

struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s);


BOOL PrintFunctionDefinition (BPTR out_p, const struct FunctionDefinition * const fn_p);




#ifdef __cplusplus
}
#endif




#endif		/* ifndef FUNCTION_DEFINITION_H */

