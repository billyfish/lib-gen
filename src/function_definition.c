/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File:
**
** Date: 20-09-2014 23:00:22
**
************************************************************

*/

#include <string.h>
#include <ctype.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "function_definition.h"
#include "debugging_utils.h"
#include "utils.h"


#ifdef _DEBUG
#define FUNCTION_DEFINITIONS_DEBUG (101)
#endif


void UnitTest (const char *prototype_s)
{
	struct FunctionDefinition *fd_p = NULL;
	
	IDOS->Printf ("*** %s ***\n", prototype_s);

	fd_p = TokenizeFunctionPrototype (prototype_s);

	if (fd_p)
		{
			PrintFunctionDefinition (IDOS->Output (), fd_p);
			FreeFunctionDefinition (fd_p);
		}

	IDOS->Printf ("\n");
}


const char *Scroll (const char *start_p, BOOL scroll_past_whitespace_flag)
{
	/* scroll past any whitespace */
	while ((*start_p != '\0') && ((isspace (*start_p) != 0) == scroll_past_whitespace_flag))
		{
			++ start_p;
		}
		
	if (*start_p != '\0')
		{
			return start_p;
		}
	else
		{
			return NULL;
		}
}


struct Parameter *GetParameterFromFunctionPointer (const char **definition_start_pp, const char *opening_bracket_p)
{
	const char *start_p = opening_bracket_p;
	const char *end_p = NULL;
	struct Parameter *param_p = NULL;
	
	/* scroll past any whitespace */
	Scroll (start_p, TRUE);
		
	/* Is it a function pointer? */
	if ((start_p != NULL) && (*start_p == '*'))
		{
			/* scroll past any whitespace */
			Scroll (start_p, TRUE);
			
			if (start_p)
				{
					BOOL success_flag = ((isalpha (*start_p) != 0) || (*start_p == '_'));
					const char *data_p = start_p + 1;
					BOOL loop_flag = ((isalnum (*data_p) != 0) || (*data_p == '_'));
					
					while (loop_flag && success_flag)
						{
							++ data_p;
							
							if (*data_p == '\0')
								{
									success_flag = FALSE;
								}
							else if (*data_p == ')')
								{
									end_p = data_p - 1;
									loop_flag = FALSE;
								} 
							else if (isspace (*data_p) != 0)
								{
									const char *temp_p = data_p;
									
									Scroll (temp_p, TRUE);
									
									if (temp_p && (*temp_p == ')'))
										{
											end_p = data_p - 1;
											success_flag = TRUE;
										}
									else
										{
											success_flag = FALSE;
										}
										
									loop_flag = FALSE;
								}
							else
								{
									loop_flag = ((isalnum (*data_p) != 0) || (*data_p == '_'));
								}
						
						}		/* while (loop_flag && success_flag) */
					
					if (success_flag)
						{
							const char *definition_start_p = *definition_start_pp;
							/* start_p -> end_p is the param name, we now extract the type */
							size_t def_length = strlen (definition_start_p);
							size_t name_length = end_p - start_p;
							size_t type_length = def_length - name_length ;
							
							char *type_s = (char *) IExec->AllocVecTags (type_length, TAG_DONE);
							
							if (type_s)
								{
									char *temp_p = type_s;
									size_t l = start_p - definition_start_p;
									char *name_s = CopyToNewString (start_p, end_p, FALSE);
									
									if (name_s)
										{
											memcpy (temp_p, definition_start_p, l);
											temp_p += l;
										
											strcpy (temp_p, end_p + 1);		
											
											param_p = AllocateParameter	(type_s, name_s);		
											
											if (!param_p)
												{
													IExec->FreeVec (name_s);
													IExec->FreeVec (type_s);
												}						
										}
									else
										{
											IExec->FreeVec (type_s);	
										}
									 
								}		/* if (type_s) */
							
						}		/* if (success_flag) */
					
				}		/* if (start_p) */
			
		}

	
	return param_p;
}


BOOL GetNextParameter (const char **start_pp, struct Parameter **param_pp, BOOL function_flag)
{
	BOOL success_flag = FALSE;
	BOOL loop_flag = TRUE;
	struct Parameter *param_p = NULL;
	const char *data_p = *start_pp;
	uint8 bracket_count = 0;


	#if FUNCTION_DEFINITIONS_DEBUG >= 1
	DB (KPRINTF ("%s %ld - getting param1  \"%s\"\n", __FILE__, __LINE__, data_p));
	#endif

	Scroll (data_p, TRUE);

	#if FUNCTION_DEFINITIONS_DEBUG >= 1
	DB (KPRINTF ("%s %ld - getting param2  \"%s\"\n", __FILE__, __LINE__, data_p));
	#endif

	
	if (data_p)
		{
			while (loop_flag && !success_flag)
				{				
					const char c = *data_p;
		
					#if FUNCTION_DEFINITIONS_DEBUG >= 1
					DB (KPRINTF ("%s %ld - data  \"%c\"\n", __FILE__, __LINE__, c));
					#endif
									
					if (isspace (c) == 0)
						{
							switch (*data_p)
								{						
									case '(':
										if (function_flag)
											{										
												success_flag = TRUE;											
												loop_flag = FALSE;
											}
										else
											{
												param_p  = GetParameterFromFunctionPointer (start_pp, data_p);
												
												
											//struct Parameter *GetParameterFromFunctionPointer (const char *definition_start_p, const char *opening_bracket_p)
												loop_flag = FALSE; 
											}
										break;
				
									case ')':
										
										
										-- bracket_count;
										break;
				
									case ',':
										if (bracket_count == 0)
											{
												success_flag = TRUE;
											}
										break;
				
									default:
										break;
								}		/* switch (data_p) */	
												
						}		/* if (isspace (c) == 0) */
		
					if (!success_flag)
						{
							++ data_p;
							
							loop_flag = (*data_p != '\0');
						}
				}		
		}		



	if (success_flag && !param_p)
		{
			param_p = GetNormalParameter (*start_pp, data_p - 1);
			
			if (param_p)
				{
					*param_pp = param_p;
					*start_pp = data_p + 1;
				}
			else
				{
					success_flag = FALSE;
				}
		}


	return success_flag;
}



/*
	<type> <function> (<type> <name>, ....);

 function can have dereferences before it thast should be stored
 as part of the type. Param Types can be function pointers when

 first bracket separates the function from its parameters.
 Each string should be trimmed of whitepace where possible.


 1. find opening bracket and split the string on it.
 2. find the start and end of the function name


*/
struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s)
{
	BOOL success_flag = FALSE;
	struct FunctionDefinition *fd_p = AllocateFunctionDefinition ();
		
	#if FUNCTION_DEFINITIONS_DEBUG >= 1
	DB (KPRINTF ("%s %ld - Tokenizing \"%s\"\n", __FILE__, __LINE__, prototype_s));
	#endif
	
	if (fd_p)
		{
			const char *start_p = prototype_s;
			BOOL function_flag = TRUE;
			BOOL loop_flag = TRUE;
			struct Parameter *param_p = NULL;
			success_flag = TRUE;
			
			while (loop_flag && success_flag)
				{						
					if (GetNextParameter (&start_p, &param_p, function_flag))
						{
							if (function_flag)
								{
									fd_p -> fd_definition_p = param_p;
									function_flag = FALSE;
								}
							else
								{
									success_flag = AddParameterAtBack (fd_p, param_p);
								}
								
							++ start_p;
						}
					else
						{
							success_flag = FALSE;
						}
				
				}		/* while (start_p) */		

			if (!success_flag)
				{
					FreeFunctionDefinition (fd_p);
					fd_p = NULL;
				}

		}		/* if (fd_p) */


	return fd_p;
}


uint32 GetFunctionDefinitionsListSize (struct List * const list_p)
{
	uint32 i = 0;
	struct FunctionDefinitionNode *node_p;
	const BOOL verbose_flag = GetVerboseFlag ();

	for (node_p = (struct FunctionDefinitionNode *) IExec->GetHead (list_p); node_p != NULL; node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p))
		{
			++ i;

			if (verbose_flag)
				{
					PrintParameter (IDOS->Output (), node_p -> fdn_function_def_p -> fd_definition_p);
					IDOS->FPrintf (IDOS->Output (), "\n");
				}
		}

	return i;
}


struct FunctionDefinition *AllocateFunctionDefinition (void)
{
	struct FunctionDefinition *fd_p = (struct FunctionDefinition *) IExec->AllocVecTags (sizeof (struct FunctionDefinition), TAG_DONE);

	if (fd_p)
		{
			struct List *params_p = (struct List *) IExec->AllocSysObjectTags (ASOT_LIST,
				ASOLIST_Type, PT_PARAMETER,
				TAG_DONE);

			if (params_p)
				{
					fd_p -> fd_definition_p = NULL;
					fd_p -> fd_args_p = params_p;

					return fd_p;
				}

			IExec->FreeVec (fd_p);
		}

	return NULL;
}


void FreeFunctionDefinition (struct FunctionDefinition *fd_p)
{
	if (fd_p -> fd_definition_p)
		{
			FreeParameter (fd_p -> fd_definition_p);
		}

	FreeParameterList (fd_p -> fd_args_p);

	IExec->FreeVec (fd_p);
}


void FreeFunctionDefinitionNode (struct FunctionDefinitionNode *node_p)
{
	FreeFunctionDefinition (node_p -> fdn_function_def_p);
	IExec->FreeVec (node_p);
}



BOOL AddParameterAtFront (struct FunctionDefinition *fd_p, struct Parameter *param_p)
{
	struct ParameterNode *node_p = AllocateParameterNode (param_p);

	if (node_p)
		{
			IExec->AddHead (fd_p -> fd_args_p, (struct Node *) node_p);

			return TRUE;
		}

	return FALSE;

}


BOOL AddParameterAtBack (struct FunctionDefinition *fd_p, struct Parameter *param_p)
{
	struct ParameterNode *node_p = AllocateParameterNode (param_p);

	if (node_p)
		{
			IExec->AddTail (fd_p -> fd_args_p, (struct Node *) node_p);

			if (GetVerboseFlag ())
				{
					IDOS->FPrintf (IDOS->Output (), "Added %s - %s to %s\n", param_p -> pa_type_s, param_p -> pa_name_s, fd_p -> fd_definition_p -> pa_name_s);
				}

			return TRUE;
		}

	return FALSE;
}


BOOL PrintFunctionDefinition (BPTR out_p, const struct FunctionDefinition * const fn_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "FUNCTION:\n") >= 0)
		{
			if (PrintParameter (out_p, fn_p -> fd_definition_p))
				{
					if (IDOS->FPrintf (out_p, "\nPARAMS:\n") >= 0)
						{
							success_flag = PrintParameterList (out_p, fn_p -> fd_args_p);
						}
				}
		}

	return success_flag;
}


BOOL WriteLibraryFunctionImplementation (BPTR out_p, const struct FunctionDefinition * const fd_p, CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "%s %s%s (", fd_p -> fd_definition_p -> pa_type_s, library_s, fd_p -> fd_definition_p -> pa_name_s) >= 0)
		{
			struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);
			struct ParameterNode *final_node_p = (struct ParameterNode *) IExec->GetTail (fd_p -> fd_args_p);

			if (IDOS->FPrintf (out_p, "struct %s *Self", library_s) >= 0)
				{
					if (curr_node_p != final_node_p)
						{
							success_flag = (IDOS->FPrintf (out_p, ", ", library_s) >= 0);

							while ((curr_node_p != final_node_p) && success_flag)
								{
									success_flag = WriteParameterAsSource (out_p, curr_node_p -> pn_param_p);

									if (success_flag)
										{
											success_flag = (IDOS->FPrintf (out_p, ", ") >= 0);

											if (success_flag)
												{
													curr_node_p = (struct ParameterNode *) IExec->GetSucc ((struct Node *) curr_node_p);
												}
										}
								}
						}
					else
						{
							success_flag = TRUE;
						}

					if (success_flag)
						{
							if (strcmp (final_node_p -> pn_param_p -> pa_type_s, "void") != 0)
								{
									success_flag = WriteParameterAsSource (out_p, final_node_p -> pn_param_p);
								}

							if (success_flag)
								{
									success_flag = (IDOS->FPrintf (out_p, ")\n{\n\t") >= 0);

									if (success_flag)
										{
											if (strcmp (fd_p -> fd_definition_p -> pa_type_s, "void") != 0)
												{
													success_flag = (IDOS->FPrintf (out_p, "return ") >= 0);
												}

											if (success_flag)
												{
													success_flag = (IDOS->FPrintf (out_p, "%s (", fd_p -> fd_definition_p -> pa_name_s) >= 0);

													if (success_flag)
														{
															curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);

															while ((curr_node_p != final_node_p) && success_flag)
																{
																	success_flag = (IDOS->FPrintf (out_p, "%s, ", curr_node_p -> pn_param_p -> pa_name_s) >= 0);

																	if (success_flag)
																		{
																			curr_node_p = (struct ParameterNode *) IExec->GetSucc ((struct Node *) curr_node_p);
																		}
																}


															if (success_flag)
																{
																	success_flag =  (IDOS->FPrintf (out_p, "%s);\n}\n\n", final_node_p -> pn_param_p -> pa_name_s) >= 0);
																}
														}
												}
										}
								}
						}
				}
		}

	return success_flag;
}

