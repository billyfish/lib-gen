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
#define FUNCTION_DEFINITIONS_DEBUG (1)
#endif


void UnitTest (const char *prototype_s)
{
	const char *start_p = prototype_s;
	const char *end_p = NULL;
	BOOL function_flag = TRUE;

	IDOS->Printf ("*** %s ***\n", prototype_s);

	while (start_p)
		{
		//	end_p = FindParameterEnd (start_p, function_flag);

			if (end_p)
				{
					if (function_flag)
						{
							function_flag = FALSE;
						}

					start_p = end_p + 1;
				}
			else
				{
					start_p = NULL;
				}
		}

	IDOS->Printf ("\n");
}


struct Parameter *GetNextParameter (const char **start_pp, BOOL function_flag)
{
	const char *start_p = *start_pp;
	const char *data_p = start_p;
	const char *end_p = NULL;
	const char *res_p = NULL;
	struct Parameter *param_p = NULL;
	uint8 function_pointer_status = 0;
	uint8 bracket_count = 0;

	while ((*data_p != '\0') && (!res_p))
		{
			switch (*data_p)
				{
					case '(':
						if (function_flag)
							{
								end_p = data_p - 1;
								res_p = data_p;
							}
						else
							{
								++ function_pointer_status;
								++ bracket_count;
							}
						break;

					case ')':
						switch (function_pointer_status)
							{
								case 0:
								case 2:
									{
										end_p = data_p - 1;
										res_p = data_p + 1;

										while (isspace (*res_p))
											{
												++ res_p;
											}

										if (*res_p == ';')
											{
												res_p = NULL;
											}
										else if (*res_p == ',')
											{
												++ res_p;

												while (isspace (*res_p))
													{
														++ res_p;
													}
											}
									}
									break;

								default:
									-- bracket_count;
									break;
							}
						break;

					case ',':
						if (bracket_count == 0)
							{
								end_p = data_p - 1;
								res_p = data_p + 1;
							}
						break;

					default:
						break;
				}		/* switch (data_p) */

			if (!res_p)
				{
					++ data_p;
				}
		}

	if (end_p)
		{
			const char *temp_p = end_p;

			#ifdef FUNCTION_DEFINITIONS_DEBUG
			char *param_s = NULL;
			#endif

			while (isspace (*temp_p))
				{
					-- temp_p;
				}

			while (isspace (*start_p))
				{
					++ start_p;
				}

			if (function_pointer_status == 0)
				{
					param_p = ParseParameter (start_p, temp_p);
				}
			else if (function_pointer_status == 2)
				{
					param_p = ParseFunctionPointerParameter (start_p, temp_p);
				}

			//IDOS->Printf ("fp status %lu\n", function_pointer_status);

			*start_pp = res_p;

			if (param_p)
				{
					/*
					BPTR out_p = IDOS->Output ();

					IDOS->FPrintf (out_p, "param: ");
					PrintParameter (out_p, param_p);
					IDOS->FPrintf (out_p, "\n");
					*/
				}
			else
				{
					IDOS->Printf ("Failed to extract parameter from \"%s\"\n", start_p);
				}

			#ifdef FUNCTION_DEFINITIONS_DEBUG
			param_s = CopyToNewString (start_p, end_p, FALSE);

			if (param_s)
				{
					DB (KPRINTF ("%s %ld - param \"%s\"\n", __FILE__, __LINE__, param_s));
					IExec->FreeVec (param_s);
				}
			#endif
		}

	return param_p;
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
struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s, CONST_STRPTR filename_s, const int32 line_number)
{
	BOOL success_flag = FALSE;
	struct FunctionDefinition *fd_p = AllocateFunctionDefinition (filename_s, line_number);

	DB (KPRINTF ("%s %ld - Tokenizing \"%s\"\n", __FILE__, __LINE__, prototype_s));

	if (fd_p)
		{
			const char *start_p = prototype_s;
			BOOL function_flag = TRUE;
			BOOL loop_flag = TRUE;
			struct Parameter *param_p = NULL;

			success_flag = TRUE;

			while (loop_flag && success_flag)
				{
					#ifdef FUNCTION_DEFINITIONS_DEBUG
					DB (KPRINTF ("%s %ld - Getting next param from \"%s\"\n", __FILE__, __LINE__, start_p));
					#endif

					param_p = GetNextParameter (&start_p, function_flag);

					if (param_p)
						{
							//IDOS->Printf ("\nGot next param!\n");
							//PrintParameter (IDOS->Output (), param_p);
							//IDOS->Printf ("\n");

							if (function_flag)
								{
									fd_p -> fd_definition_p = param_p;
									function_flag = FALSE;

									while (isspace (*start_p))
										{
											++ start_p;
										}

									if (*start_p == '(')
										{
											++ start_p;
										}
								}
							else
								{
									success_flag = AddParameterAtBack (fd_p, param_p);
								}
						}
					else
						{
							if (start_p)
								{
									//IDOS->Printf ("\nNo Param!\n");
									success_flag = FALSE;
								}
							else
								{
									loop_flag = FALSE;
								}
						}

					if (start_p)
						{
							//IDOS->Printf ("start_p: \"%s\"\n", start_p);

							while (isspace (*start_p))
								{
									++ start_p;
								}

							if (*start_p == ')')
								{
									loop_flag = FALSE;
								}
						}
					else
						{
							//IDOS->Printf ("\nstart_p = NULL!\n");
							loop_flag = FALSE;
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
	const enum Verbosity v = GetVerbosity ();

	for (node_p = (struct FunctionDefinitionNode *) IExec->GetHead (list_p); node_p != NULL; node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p))
		{
			++ i;

			if (v >= VB_LOUD)
				{
					PrintParameter (IDOS->Output (), node_p -> fdn_function_def_p -> fd_definition_p);
					IDOS->FPrintf (IDOS->Output (), "\n");
				}
		}

	return i;
}


struct FunctionDefinition *AllocateFunctionDefinition (CONST STRPTR filename_s, const int32 line_number)
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
					fd_p -> fd_filename_s = filename_s;
					fd_p -> fd_line_number = line_number;


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

			if (GetVerbosity () >= VB_LOUD)
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

			if (curr_node_p)
				{
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
								}
						}

				}		/* if (curr_node_p) */
			else
				{
					success_flag = TRUE;
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

	return success_flag;
}


BOOL AddFunctionDefinitionToList (struct FunctionDefinition *fd_p, struct List *functions_list_p)
{
	BOOL success_flag = FALSE;

	struct FunctionDefinitionNode *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
		ASONODE_Size, sizeof (struct FunctionDefinitionNode),
		TAG_DONE);

	if (node_p)
		{
			node_p -> fdn_function_def_p = fd_p;

			IExec->AddTail (functions_list_p, (struct Node *) node_p);
			success_flag = TRUE;
		}

	return success_flag;
}


int CompareFunctionDefinitionNodes (const void *v0_p, const void *v1_p)
{
	struct FunctionDefinition *fd0_p = ((struct FunctionDefinitionNode *) v0_p) -> fdn_function_def_p;
	struct FunctionDefinition *fd1_p = ((struct FunctionDefinitionNode *) v1_p) -> fdn_function_def_p;

	int res = strcmp (fd0_p -> fd_filename_s, fd1_p -> fd_filename_s);

	if (res == 0)
		{
			if (fd0_p -> fd_line_number > fd1_p -> fd_line_number)
				{
					res = 1;
				}
			else if (fd0_p -> fd_line_number < fd1_p -> fd_line_number)
				{
					res = -1;
				}
		}

	return res;
}
