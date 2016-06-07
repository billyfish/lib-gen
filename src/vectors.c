/*

************************************************************
**
** Created by: CodeBench 0.42 (14.10.2013)
**
** Project: libgen
**
** File: 
**
** Date: 07-06-2016 23:26:57
**
************************************************************

*/

#include <proto/dos.h>



BOOL WriteVectorsFile (BPTR vector_file_p, CONST CONST_STRPTR library_s, struct List *function_defs_p)
{
	/* Add the system includes */
	if (IDOS->FPrintf (vector_file_p, "#include <exec/exec.h>\n#include <exec/interfaces.h>\n#include <exec/types.h>\n\n") >= 0)
		{
			/* Add our library includes */
			
			/* Declare our library functions */
			
			
			/* Declare the library vectors */
			
		}		
	
	return FALSE;
}


BOOL WriteLibraryIncludes (BPTR vector_file_p)
{
	
	
}


BOOL WriteFunctionDeclarations (BPTR vector_file_p, struct List *function_defs_p)
{
	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (fn_defs_p);
	
	while (node_p && success_flag)
		{
			struct FunctionDefinition *fn_def_p = node_p -> fdn_function_def_p;
			
			if (WriteFunctionDeclaration (vector_file_p, fn_def_p))
				{
					node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
				}
			else
				{
					success_flag = FALSE;
				}
		}
			
	return success_flag;
} 


static BOOL WriteFunctionDeclaration (BPTR vector_file_p, const struct FunctionDefinition * const function_def_p)
{
	if (WriteLibraryFunctionDefinition (vector_file_p, function_def_p))
		{
			if (IDOS->FPrintf (vector_file_p, ";\n") >= 0)
				{
					return TRUE;	
				}
		}
		
	return FALSE;
}
