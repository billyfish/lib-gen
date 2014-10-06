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

#include <proto/exec.h>
#include <proto/dos.h>

#include "function_definition.h"
#include "debugging_utils.h"
#include "memory.h"
#include "utils.h"

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

	DB (KPRINTF ("%s %ld - Tokenizing \"%s\"\n", __FILE__, __LINE__, prototype_s));

	if (fd_p)
		{
			const char *opening_bracket_p = strchr (prototype_s, '(');
			const char *closing_bracket_p = strrchr (prototype_s, ')');

			//DB (KPRINTF ("%s %ld - opening_bracket \"%s\"\n", __FILE__, __LINE__, opening_bracket_p ? opening_bracket_p : "NULL"));

			if (opening_bracket_p && closing_bracket_p)
				{
					struct Parameter *param_p = ParseParameter (prototype_s, opening_bracket_p - 1);

					if (param_p)
						{
							const char *start_p = opening_bracket_p + 1;
							const char *end_p = strchr (start_p, ',')l
							BOOL loop_flag = (end_p != NULL);

							fd_p -> fd_definition_p = param_p;

							success_flag = TRUE;

							/* Get all of the parameters before each comma */
							while (loop_flag && success_flag)
								{
									DB (KPRINTF ("%s %ld - Tokenizing loop %s", __FILE__, __LINE__, start_p));

									if (IsParameterFunctionPointer (start_p, end_p))
										{

										}
									else
										{
											param_p = ParseParameter (start_p, end_p);
										}


									if (param_p)
										{
											if (AddParameterAtBack (fd_p, param_p))
												{
													start_p = end_p + 1;

													if (start_p < closing_bracket_p)
														{
															end_p = strchr (start_p, ',');

															if (!end_p)
																{
																	loop_flag = FALSE;
																}
														}
													else
														{
															loop_flag = FALSE;
														}
												}
											else
												{
													success_flag = FALSE;
												}
										}
									else
										{
											success_flag = FALSE;
										}
								}


							if (success_flag)
								{
									//const char *start_p = (end_p != NULL) ? end_p + 1 : opening_bracket_p + 1;
									end_p = strchr (start_p, ')');

									/* Get the final parameter before the closing braacket */
									if (end_p)
										{
											param_p = ParseParameter (start_p, end_p);

											if (param_p)
												{
													success_flag = AddParameterAtBack (fd_p, param_p);
												}
											else
												{
													success_flag = FALSE;
												}
										}
									else
										{
											success_flag = FALSE;
										}
								}
						}

				}		/* if (opening_bracket_p) */

		}

	if (!success_flag)
		{
			FreeFunctionDefinition (fd_p);
			fd_p = NULL;
		}

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
	struct FunctionDefinition *fd_p = (struct FunctionDefinition *) AllocMemory (sizeof (struct FunctionDefinition));

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

			FreeMemory (fd_p);
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

	FreeMemory (fd_p);
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
			if (IDOS->FPrintf (out_p, "struct %s *Self, ", library_s) >= 0)
				{
					struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);
					struct ParameterNode *final_node_p = (struct ParameterNode *) IExec->GetTail (fd_p -> fd_args_p);

					success_flag = TRUE;

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

					if (success_flag)
						{
							success_flag = WriteParameterAsSource (out_p, final_node_p -> pn_param_p);

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

