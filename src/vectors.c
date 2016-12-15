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

#include <ctype.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "function_definition.h"
#include "debugging_utils.h"
#include "utils.h"


/******* BEGIN STATIC DECLARTAIONS ******/

static BOOL WriteFunctionDeclarations (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p);

static BOOL WriteFunctionDeclaration (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, const struct FunctionDefinition * const function_def_p);

static BOOL WriteVectorsArray (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p);

static BOOL WriteVectors (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p);

static BOOL WriteFunctionNameIntoArray (BPTR vector_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, const struct FunctionDefinition *function_def_p);

/******** END STATIC DECLARATIONS *******/


BOOL WriteVectorsFile (CONST CONST_STRPTR source_directory_s, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p)
{
	BOOL success_flag = FALSE;
	STRPTR vectors_header_s = NULL;
	
	ENTER ();
	
	vectors_header_s = MakeFilename (source_directory_s, "vectors.h");
	
	if (vectors_header_s)
		{
			BPTR vectors_f = IDOS->FOpen (vectors_header_s, MODE_NEWFILE, 0);
			
			if (vectors_f)
				{
					success_flag = WriteVectors (vectors_f, library_s, prefix_s, function_defs_p);
				
					if (IDOS->FClose (vectors_f) == 0)
						{
							
						}
						
				}		/* if (vectors_f) */
			
			IExec->FreeVec (vectors_header_s);
		}		/* if (vectors_header_s) */
	
	LEAVE ();
	return success_flag;
}



static BOOL WriteVectors (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p)
{
	ENTER ();

	BOOL success_flag = FALSE;

	/* Add the system includes */
	if (IDOS->FPrintf (vector_file_p, "#include <exec/exec.h>\n#include <exec/interfaces.h>\n#include <exec/types.h>\n") >= 0)
		{
			if (IDOS->FPrintf (vector_file_p, "#include <proto/%s.h>\n", library_s) >= 0)
				{
			
					/* Add our library includes */
					if (WriteFunctionDefinitionListIncludes (vector_file_p, function_defs_p, "#include \"", "\""))
						{
							if (IDOS->FPrintf (vector_file_p, "\n\n") >= 0)
								{
									/* Declare our library functions */
									if (WriteFunctionDeclarations (vector_file_p, library_s, prefix_s, function_defs_p))
										{
											/* Declare the library vectors */
											if (WriteVectorsArray (vector_file_p, library_s, prefix_s, function_defs_p))
												{
													success_flag = TRUE;
												}
										}
								}
						}
				}
		}

	LEAVE ();
	return success_flag;
}


static BOOL WriteFunctionDeclarations (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p)
{
	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p;
	STRPTR interface_s;	
	
	ENTER ();

	node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_defs_p);
	interface_s = GetInterfaceName (library_s);
	
	if (interface_s)
		{
		
			while (node_p && success_flag)
				{
					struct FunctionDefinition *fn_def_p = node_p -> fdn_function_def_p;
		
					if (IDOS->FPrintf (vector_file_p, "#include \"%s_%s\"", library_s, fn_def_p -> fd_header_filename_s) >= 0)
						{
							node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
						}
					else
						{
							success_flag = FALSE;
						}
					/*
					if (WriteFunctionDeclaration (vector_file_p, interface_s, prefix_s, fn_def_p))
						{
							node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
						}
					else
						{
							success_flag = FALSE;
						}
					*/
				}			
					
			IExec->FreeVec (interface_s);
		}


	if (success_flag)
		{
			success_flag = (IDOS->FPrintf (vector_file_p, "\n") >= 0);
		}



	LEAVE ();
	return success_flag;
}


static BOOL WriteFunctionDeclaration (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, const struct FunctionDefinition * const function_def_p)
{
	ENTER ();

	if (WriteInterfaceFunctionDefinition (vector_file_p, library_s, function_def_p))
		{
			if (IDOS->FPrintf (vector_file_p, ";\n") >= 0)
				{
					return TRUE;
				}
			else
				{
					DB (KPRINTF ("%s %ld - Failed to add semi-colon after %s\n", __FILE__, __LINE__, function_def_p -> fd_definition_p -> pa_name_s ));
				}
		}
	else
		{
			DB (KPRINTF ("%s %ld - WriteLibraryFunctionDefinition failed for %s\n", __FILE__, __LINE__, function_def_p -> fd_definition_p -> pa_name_s ));
		}

	LEAVE ();
	return FALSE;
}


static BOOL WriteVectorsArray (BPTR vector_file_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p)
{
	ENTER ();

	BOOL success_flag = TRUE;

	if (IDOS->FPrintf (vector_file_p, "STATIC CONST APTR %s_vectors [] = \n{\n\t%sObtain,\n\t%sRelease,\n\tNULL,\n\tNULL,\n", library_s, prefix_s, prefix_s) >= 0)
		{
			struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_defs_p);

			while (node_p && success_flag)
				{
					struct FunctionDefinition *function_def_p = node_p -> fdn_function_def_p;

					if (WriteFunctionNameIntoArray (vector_file_p, library_s, prefix_s, function_def_p))
						{
							node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
						}
					else
						{
							success_flag = FALSE;
						}

				}		/* while (node_p && success_flag) */


			if (success_flag)
				{
					success_flag = (IDOS->FPrintf (vector_file_p, "\t(APTR) -1\n};\n") >= 0);
				}		/* if (success_flag) */

		}		/* if (IDOS->FPrintf (vector_file_p, "STATIC CONST APTR %s_vectors [] = {\n", library_s) >= 0) */
	else
		{

		}

	LEAVE ();
	return success_flag;
}


static BOOL WriteFunctionNameIntoArray (BPTR vector_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, const struct FunctionDefinition *function_def_p)
{
	BOOL success_flag = FALSE;
	
	ENTER ();

	if (IDOS->FPrintf (vector_f, "\t") >= 0)
		{
			if (WriteFunctionDefinitionFunctionName (vector_f, prefix_s, function_def_p))
				{
					if (IDOS->FPrintf (vector_f, ",\n") >= 0)
						{
							success_flag = TRUE;
						}
					else
						{
							DB (KPRINTF ("%s %ld - Failed to add newline after %s\n", __FILE__, __LINE__, function_def_p -> fd_definition_p -> pa_name_s ));
						}
				}
			else
				{
					DB (KPRINTF ("%s %ld - Failed to write function name %s\n", __FILE__, __LINE__, function_def_p -> fd_definition_p -> pa_name_s ));
				}
		}
	else
		{
			DB (KPRINTF ("%s %ld - Failed to add tab before %s\n", __FILE__, __LINE__, function_def_p -> fd_definition_p -> pa_name_s ));
		}
				
	LEAVE ();
	
	return success_flag;
}


