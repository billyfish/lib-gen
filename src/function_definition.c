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
#include <stdlib.h>
#include <ctype.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "function_definition.h"
#include "debugging_utils.h"
#include "utils.h"


#ifdef _DEBUG
#define FUNCTION_DEFINITIONS_DEBUG (1)
#endif


static BOOL WriteIncludes (BPTR out_p, CONST_STRPTR header_name_s,  CONST_STRPTR lib_name_s);


static BPTR GetSourceFileHandle (const struct FunctionDefinition *fn_def_p, CONST_STRPTR library_s, CONST_STRPTR output_dir_s, char file_suffix, STRPTR *output_filename_ss);

static BOOL WriteLibraryFunctionDefinitionVariant (BPTR out_p, CONST CONST_STRPTR library_s, const struct FunctionDefinition * const fd_p, CONST CONST_STRPTR type_prefix_s, CONST CONST_STRPTR type_suffix_s, CONST CONST_STRPTR function_prefix_s, CONST CONST_STRPTR function_suffix_s, CONST CONST_STRPTR self_type_s);



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

	LEAVE ();
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

	LEAVE ();
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

int8 TokenizeFunctionPrototype (struct FunctionDefinition **fn_def_pp, const char *prototype_s, CONST_STRPTR filename_s, const int32 line_number)
{
	int8 res = -1;
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

			if (success_flag)
				{
					*fn_def_pp = fd_p;
					res = 1;
				}
			else
				{
					FreeFunctionDefinition (fd_p);
				}

		}		/* if (fd_p) */


	LEAVE ();
	return res;
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

	LEAVE ();
	return i;
}


struct FunctionDefinition *AllocateFunctionDefinition (CONST CONST_STRPTR filename_s, const int32 line_number)
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
					fd_p -> fd_header_filename_s = filename_s;
					fd_p -> fd_line_number = line_number;


					return fd_p;
				}

			IExec->FreeVec (fd_p);
		}

	LEAVE ();
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

	LEAVE ();
}


void FreeFunctionDefinitionNode (struct FunctionDefinitionNode *node_p)
{
	FreeFunctionDefinition (node_p -> fdn_function_def_p);
	IExec->FreeVec (node_p);

	LEAVE ();
}



BOOL AddParameterAtFront (struct FunctionDefinition *fd_p, struct Parameter *param_p)
{
	struct ParameterNode *node_p = AllocateParameterNode (param_p);

	if (node_p)
		{
			IExec->AddHead (fd_p -> fd_args_p, (struct Node *) node_p);

			return TRUE;
		}

	LEAVE ();

	return FALSE;
}


BOOL AddParameterAtBack (struct FunctionDefinition *fd_p, struct Parameter *param_p)
{
	struct ParameterNode *node_p = AllocateParameterNode (param_p);
	BOOL b = FALSE;

	if (node_p)
		{
			IExec->AddTail (fd_p -> fd_args_p, (struct Node *) node_p);

			if (GetVerbosity () >= VB_LOUD)
				{
					IDOS->FPrintf (IDOS->Output (), "Added %s - %s to %s\n", param_p -> pa_type_s, param_p -> pa_name_s, fd_p -> fd_definition_p -> pa_name_s);
				}

			b = TRUE;
		}

	LEAVE ();

	return b;
}


BOOL PrintFunctionDefinition (BPTR out_p, const struct FunctionDefinition * const fn_p)
{
	ENTER ();

	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "FUNCTION:\n") >= 0)
		{
			if (PrintParameter (out_p, fn_p -> fd_definition_p))
				{
					if (IDOS->FPrintf (out_p, "\nPARAMS:\n") >= 0)
						{
							success_flag = PrintParameterList (out_p, fn_p -> fd_args_p);
							
							IDOS->FPrintf (out_p, "\n");
						}
				}
		}

	LEAVE ();

	return success_flag;
}



BOOL WriteFunctionDefinitionFunctionName (BPTR out_p, CONST CONST_STRPTR library_s, const struct FunctionDefinition * const fd_p)
{
	ENTER ();

	LEAVE ();
	return (IDOS->FPrintf (out_p, "%s%s", library_s, fd_p -> fd_definition_p -> pa_name_s) >= 0);
}



 BOOL WriteLibraryFunctionDefinition (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR function_prefix_s, const struct FunctionDefinition * const fd_p)
{
	BOOL success_flag;
	STRPTR self_type_s = NULL;
	
	ENTER ();
	
	self_type_s = ConcatenateStrings ("struct ", library_s);
	
	if (self_type_s)
		{
			success_flag = WriteLibraryFunctionDefinitionVariant (out_p, library_s, fd_p, NULL, NULL, function_prefix_s, NULL, self_type_s);
			
			IExec->FreeVec (self_type_s);
		}
	
	LEAVE ();
	
	return success_flag;
}


BOOL WriteInterfaceFunctionDefinition (BPTR out_p, CONST CONST_STRPTR library_s,  CONST CONST_STRPTR interface_s, CONST CONST_STRPTR prefix_s, const struct FunctionDefinition * const fd_p)
{
	BOOL success_flag = FALSE;

	STRPTR include_filename_s = NULL;
	
	ENTER ();
	
	include_filename_s = GetSourceFilename (library_s, fd_p -> fd_header_filename_s, 'h');

	if (include_filename_s)
		{
			STRPTR self_type_s = ConcatenateStrings ("struct ", interface_s);
			
			if (self_type_s)
				{
					STRPTR function_prefix_s = ConcatenateStrings ("VARARGS68K ", prefix_s);
					
					if (function_prefix_s)
						{
							success_flag = WriteLibraryFunctionDefinitionVariant (out_p, interface_s, fd_p, NULL /*"extern"*/, NULL, function_prefix_s, NULL, self_type_s);
						
							IExec->FreeVec (function_prefix_s);
						}
						
					IExec->FreeVec (self_type_s);
				}
				
			IExec->FreeVec (include_filename_s);
		}
			
	LEAVE ();
	
	return success_flag;
}


BOOL WriteInterfaceHeaderDefinition (BPTR out_p, CONST CONST_STRPTR library_s, const struct FunctionDefinition * const fd_p)
{
	BOOL success_flag = FALSE;
	STRPTR self_type_s = NULL;
	
	ENTER ();

	
	self_type_s = ConcatenateStrings ("struct ", library_s);
	
	if (self_type_s)
		{
			if (WriteLibraryFunctionDefinitionVariant (out_p, library_s, fd_p, NULL, "APICALL (*", NULL, ")", self_type_s))
				{
					if (IDOS->FPrintf (out_p, ";\n") >= 0)
						{
							success_flag = TRUE;	
						}
				}
			
			IExec->FreeVec (self_type_s);
		}
	
	LEAVE ();
	
	return success_flag;
}



static BOOL WriteLibraryFunctionDefinitionVariant (BPTR out_p, CONST CONST_STRPTR library_s, const struct FunctionDefinition * const fd_p, CONST CONST_STRPTR type_prefix_s, CONST CONST_STRPTR type_suffix_s, CONST CONST_STRPTR function_prefix_s, CONST CONST_STRPTR function_suffix_s, CONST CONST_STRPTR self_type_s)
{
	BOOL success_flag = FALSE;

	ENTER ();

	if (IDOS->FPrintf (out_p, "%s %s %s", type_prefix_s ? type_prefix_s : "", fd_p -> fd_definition_p -> pa_type_s, type_suffix_s ? type_suffix_s : "") >= 0)
		{
			if (WriteFunctionDefinitionFunctionName (out_p, function_prefix_s, fd_p))
				{
					if (IDOS->FPrintf (out_p, " %s (", function_suffix_s ? function_suffix_s : "") >= 0)
						{
							struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);
							struct ParameterNode *final_node_p = (struct ParameterNode *) IExec->GetTail (fd_p -> fd_args_p);

							if (curr_node_p)
								{
									if (IDOS->FPrintf (out_p, "%s *Self", self_type_s) >= 0)
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

																}		/* if (success_flag) */

														}		/* while ((curr_node_p != final_node_p) && success_flag) */

												}		/* if (curr_node_p != final_node_p) */
											else
												{
													success_flag = (IDOS->FPrintf (out_p, ", ", library_s) >= 0);
												}

											if (success_flag)
												{
													if (strcmp (final_node_p -> pn_param_p -> pa_type_s, "void") != 0)
														{
															success_flag = WriteParameterAsSource (out_p, final_node_p -> pn_param_p);
														}
												}		/* if (success_flag) */

										}		/* if (IDOS->FPrintf (out_p, "struct %s *Self", library_s) >= 0) */

								}		/* if (curr_node_p) */
							else
								{
									success_flag = TRUE;
								}

						}		/* if (IDOS->FPrintf (out_p, " (") >= 0) */
					else
						{

						}

				}		/* if (WriteFunctionDefinitionFunctionName (out_p, library_s, fd_p)) */
			else
				{

				}

		}		/* if (IDOS->FPrintf (out_p, "%s ", fd_p -> fd_definition_p -> pa_type_s) >= 0) */
	else
		{

		}


	if (success_flag)
		{
			success_flag = (IDOS->FPrintf (out_p, ")") >= 0);
		}

	LEAVE ();

	return success_flag;
}


BOOL WriteLibraryFunctionImplementation (BPTR out_p, const struct FunctionDefinition * const fd_p, CONST_STRPTR library_s, CONST_STRPTR prefix_s)
{
	BOOL success_flag = FALSE;

	ENTER ();

	DB (KPRINTF ("%s %ld - WriteLibraryFunctionImplementation for  %s\n", __FILE__, __LINE__, fd_p -> fd_definition_p -> pa_name_s));

	if (WriteLibraryFunctionDefinition (out_p, library_s, prefix_s, fd_p))
		{
			success_flag = (IDOS->FPrintf (out_p, "\n{\n\t") >= 0);

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
									struct ParameterNode *final_node_p = (struct ParameterNode *) IExec->GetTail (fd_p -> fd_args_p);
									struct ParameterNode * curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);

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
											if (final_node_p)
												{
													success_flag = (IDOS->FPrintf (out_p, "%s", final_node_p -> pn_param_p -> pa_name_s) >= 0);
												}
											
											if (success_flag)
												{
													success_flag =  (IDOS->FPrintf (out_p, ");\n}\n\n") >= 0);
												}
										}
								}
						}
				}

		}

	DB (KPRINTF ("%s %ld - WriteLibraryFunctionImplementation for %s = %ld\n", __FILE__, __LINE__, fd_p -> fd_definition_p -> pa_name_s, success_flag));

	LEAVE ();
	return success_flag;
}


BOOL AddFunctionDefinitionToList (struct FunctionDefinition *fd_p, struct List *functions_list_p)
{
	ENTER ();

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

	LEAVE ();

	return success_flag;
}


int CompareFunctionDefinitionNodes (const void *v0_p, const void *v1_p)
{
	struct FunctionDefinition *fd0_p = ((struct FunctionDefinitionNode *) v0_p) -> fdn_function_def_p;
	struct FunctionDefinition *fd1_p = ((struct FunctionDefinitionNode *) v1_p) -> fdn_function_def_p;

	ENTER ();

	int res = strcmp (fd0_p -> fd_header_filename_s, fd1_p -> fd_header_filename_s);

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

	LEAVE ();
	return res;
}


void ClearFunctionDefinitionsList (struct List *function_defs_p)
{
	struct FunctionDefinitionNode *curr_node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_defs_p);
	struct FunctionDefinitionNode *next_node_p = NULL;

	ENTER ();

	while (curr_node_p != NULL)
		{
			next_node_p = (struct FunctionDefinitionNode *) IExec->GetSucc (& (curr_node_p -> fdn_node));
			FreeFunctionDefinitionNode (curr_node_p);
			curr_node_p = next_node_p;
		}

	LEAVE ();
}



BOOL WriteSourceForAllFunctionDefinitions (struct List *fn_defs_p, CONST_STRPTR output_dir_s, CONST_STRPTR library_s, CONST_STRPTR prefix_s)
{
	ENTER ();

	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (fn_defs_p);
	CONST_STRPTR current_filename_s = "";
	BPTR output_f = ZERO;

	while (node_p && success_flag)
		{
			struct FunctionDefinition *fn_def_p = node_p -> fdn_function_def_p;

			/* are we changing the source file that we are writing to? */
			if (strcmp (current_filename_s, fn_def_p -> fd_header_filename_s) != 0)
				{
					STRPTR output_filename_s = NULL;
					
					if (output_f)
						{
							if (IDOS->FClose (output_f) == 0)
								{
									IDOS->Printf ("Error closing %s", current_filename_s);
								}

							output_f = ZERO;
						}

					success_flag = FALSE;
					
					output_f = GetSourceFileHandle (fn_def_p, library_s, output_dir_s, 'c', &output_filename_s);

					if (output_f)
						{
							STRPTR filename_s = GetSourceFilename (library_s, fn_def_p -> fd_header_filename_s, 'h');
							
							if (filename_s)
								{
									if (IDOS->FPrintf (output_f, "#include \"%s\"\n\n", filename_s) >= 0)
										{
											success_flag = TRUE;
										}
									else
										{
											IDOS->Printf ("Error writing includes to %s", fn_def_p -> fd_header_filename_s);
										}
										
									IExec->FreeVec (filename_s);
								}
							else
								{
									IDOS->Printf ("Failed to get header filename from \"%s\" for source includes", fn_def_p -> fd_header_filename_s);
								}
					
							current_filename_s = fn_def_p -> fd_header_filename_s;
						}
					else
						{
							IDOS->Printf ("Error opening %s\n\n", output_filename_s);
						}

					if (output_filename_s)
						{
							IExec->FreeVec (output_filename_s);
						}
					
				}		/* if (strcmp (current_filename_s, fn_def_p -> fd_filename_s) != 0) */

			if (success_flag)
				{
					success_flag  = WriteSourceForFunctionDefinition (fn_def_p, output_f, library_s, prefix_s);

					if (success_flag)
						{
							node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
						}
					else
						{
							IDOS->Printf ("Error writing source for %s", fn_def_p -> fd_header_filename_s);
						}
				}
		}

	if (output_f)
		{
			if (IDOS->FClose (output_f) == 0)
				{
					IDOS->Printf ("Error closing %s", current_filename_s);
				}
		}

	LEAVE ();
	return success_flag;
}


BOOL WriteInlineMacroForFunctionDeclaration (struct FunctionDefinition *fn_def_p, BPTR output_f, CONST_STRPTR interface_name_s)
{
	BOOL success_flag = TRUE;

	if (interface_name_s)
		{
			success_flag = (IDOS->FPrintf (output_f, " %s->", interface_name_s) >= 0);
		}
	else
		{
			success_flag = (IDOS->FPrintf (output_f, "#define ") >= 0);		
		}
		
	if (success_flag)
		{
			if (IDOS->FPrintf (output_f, "%s(", fn_def_p -> fd_definition_p -> pa_name_s) >= 0)
				{
					struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (fn_def_p -> fd_args_p);
					struct ParameterNode *final_node_p = (struct ParameterNode *) IExec->GetTail (fn_def_p -> fd_args_p);
		
					if (curr_node_p)
						{
							while ((curr_node_p != final_node_p) && success_flag)
								{
									if (interface_name_s)
										{
											success_flag = (IDOS->FPrintf (output_f, "(%s), ", curr_node_p -> pn_param_p -> pa_name_s) >= 0);
										}
									else
										{
											success_flag = (IDOS->FPrintf (output_f, "%s, ", curr_node_p -> pn_param_p -> pa_name_s) >= 0);
										}
										
									if (success_flag)
										{
											curr_node_p = (struct ParameterNode *) IExec->GetSucc ((struct Node *) curr_node_p);
										}
		
								}		/* while ((curr_node_p != final_node_p) && success_flag) */
		
							if (success_flag)
								{
									if (final_node_p)
										{
											if (interface_name_s)
												{
													success_flag = (IDOS->FPrintf (output_f, "(%s)", curr_node_p -> pn_param_p -> pa_name_s) >= 0);
												}
											else
												{
													success_flag = (IDOS->FPrintf (output_f, "%s", curr_node_p -> pn_param_p -> pa_name_s) >= 0);
												}									
										}	
								}
		
						}		/* if (curr_node_p) */
				}
				
			if (success_flag)
				{
					success_flag = (IDOS->FPrintf (output_f, ")") >= 0);
					
						if (interface_name_s && success_flag)
							{
								success_flag = (IDOS->FPrintf (output_f, "\n") >= 0);
							}
				}
		}
			
	return success_flag;
}

BOOL WriteInlineMacrosForAllFunctionDeclarations (struct List *fn_defs_p, BPTR output_f, CONST_STRPTR interface_name_s)
{
	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (fn_defs_p);
	
	ENTER ();
	
	while (node_p && success_flag)
		{
			struct FunctionDefinition *fn_def_p = node_p -> fdn_function_def_p;

			if (WriteInlineMacroForFunctionDeclaration (fn_def_p, output_f, NULL))
				{
					if (WriteInlineMacroForFunctionDeclaration (fn_def_p, output_f, interface_name_s))
						{
							node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
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
	
	LEAVE ();
	
	return success_flag;
}


BOOL WriteSourceForAllFunctionDeclarations (struct List *fn_defs_p, CONST_STRPTR output_dir_s, CONST_STRPTR library_s, CONST_STRPTR prefix_s)
{
	ENTER ();

	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (fn_defs_p);
	CONST_STRPTR current_filename_s = "";
	STRPTR current_include_guard_s = NULL;
	BPTR output_f = ZERO;

	while (node_p && success_flag)
		{
			struct FunctionDefinition *fn_def_p = node_p -> fdn_function_def_p;

			/* are we changing the source file that we are writing to? */
			if (strcmp (current_filename_s, fn_def_p -> fd_header_filename_s) != 0)
				{
					STRPTR output_filename_s = NULL;
					
					if (output_f)
						{
							if (IDOS->FPrintf (output_f, "\n\n#endif\t/* #ifndef %s */\n\n", current_include_guard_s) < 0)
								{
									IDOS->Printf ("error writing closing include guard \"%s\" to %s", current_include_guard_s, current_filename_s);
								}
								
							if (IDOS->FClose (output_f) == 0)
								{
									IDOS->Printf ("Error closing %s", current_filename_s);
								}

							output_f = ZERO;
							IExec->FreeVec (current_include_guard_s);
						}

					success_flag = FALSE;

					output_f = GetSourceFileHandle (fn_def_p, library_s, output_dir_s, 'h', &output_filename_s);

					if (output_f)
						{
							current_include_guard_s = GetUpperCaseString (fn_def_p -> fd_header_filename_s);
							
							if (current_include_guard_s)
								{
									ReplaceChars (current_include_guard_s, ".:/ ", '_');									
									
									if (IDOS->FPrintf (output_f, "#ifndef %s\n#define %s\n\n\n", current_include_guard_s, current_include_guard_s)>= 0)
										{
											if (WriteIncludes (output_f, fn_def_p -> fd_header_filename_s, library_s))
												{
													success_flag = TRUE;
												}
											else
												{
													IDOS->Printf ("Error writing includes to  %s", fn_def_p -> fd_header_filename_s);
												}											
										}
									else
										{
											IDOS->Printf ("error writing closing include guard \"%s\" to %s", current_include_guard_s, current_filename_s);
										}
										
									current_filename_s = fn_def_p -> fd_header_filename_s;
								}
							else
								{
									IDOS->Printf ("Failed to get include guard for %s", fn_def_p -> fd_header_filename_s);
								}
						}
					else
						{
							IDOS->Printf ("Error opening %s\n\n", output_filename_s);
						}

					if (output_filename_s)
						{
							IExec->FreeVec (output_filename_s);
						}

				}		/* if (strcmp (current_filename_s, fn_def_p -> fd_filename_s) != 0) */

			if (success_flag)
				{
					STRPTR interface_s = GetInterfaceName (library_s);
					
					if (interface_s)
						{
							//success_flag  = WriteSourceForFunctionDeclaration (fn_def_p, output_f, library_s, prefix_s);
							success_flag = WriteInterfaceFunctionDefinition (output_f, library_s, interface_s, prefix_s, fn_def_p);
							
							if (success_flag)
								{
									if (IDOS->FPrintf (output_f, ";\n\n") >= 0)
										{
											node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
										}
									else
										{
											
										}	
								}
							else
								{
									IDOS->Printf ("Error writing source for %s", fn_def_p -> fd_header_filename_s);
								}	
								
							IExec->FreeVec (interface_s);
						}
					

				}
		}


	if (current_include_guard_s)
		{
			if (output_f)
				{
					if (IDOS->FPrintf (output_f, "\n\n#endif\t/* #ifndef %s */\n\n", current_include_guard_s) < 0)
						{
							IDOS->Printf ("error writing closing include guard \"%s\" to %s", current_include_guard_s, current_filename_s);
						}
				}
			
			IExec->FreeVec (current_include_guard_s);	
		}

	if (output_f)
		{
			if (IDOS->FClose (output_f) == 0)
				{
					IDOS->Printf ("Error closing %s", current_filename_s);
				}
		}
		


	LEAVE ();
	return success_flag;
}



BOOL WriteAllInterfaceFunctionDefinitions (struct List *fn_defs_p, BPTR out_p, CONST CONST_STRPTR interface_s)
{
	ENTER ();

	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (fn_defs_p);

	while (node_p && success_flag)
		{
			struct FunctionDefinition *fn_def_p = node_p -> fdn_function_def_p;
			
			success_flag  = WriteInterfaceHeaderDefinition (out_p, interface_s, fn_def_p);

			if (success_flag)
				{
					node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
				}
			else
				{
					IDOS->Printf ("Error writing source for %s", fn_def_p -> fd_header_filename_s);
				}
			
		}
		
		
	LEAVE ();
	return success_flag;
}


static BPTR GetSourceFileHandle (const struct FunctionDefinition *fn_def_p, CONST_STRPTR library_s, CONST_STRPTR output_dir_s, char file_suffix, STRPTR *output_filename_ss)
{
	ENTER ();

	BPTR src_f = ZERO;

	/* Get the filename */
	STRPTR filename_s = GetSourceFilename (library_s, fn_def_p -> fd_header_filename_s, file_suffix);


	if (filename_s)
		{
			STRPTR full_name_s = NULL;;

			*output_filename_ss = filename_s;

			/* Make the full filename */
			/* @TODO Make sure output dir already exists */
			full_name_s = MakeFilename (output_dir_s, filename_s);

			if (full_name_s)
				{
					src_f = IDOS->FOpen (full_name_s, MODE_NEWFILE, 0);

					if (!src_f)
						{
							DB (KPRINTF ("%s %ld - Failed to open %s\n", __FILE__, __LINE__, full_name_s));
						}

					IExec->FreeVec (full_name_s);
				}		/* if (full_name_s) */
			else
				{
					DB (KPRINTF ("%s %ld - Failed to make filename from %s and %s\n", __FILE__, __LINE__, output_dir_s, filename_s));
				}
				
				
		}		/* if (filename_s) */
	else
		{
			DB (KPRINTF ("%s %ld - Failed to get local filename from %s\n", __FILE__, __LINE__, fn_def_p -> fd_header_filename_s));
		}

	LEAVE ();
	return src_f;
}


BOOL WriteSourceForFunctionDefinition (const struct FunctionDefinition *fn_def_p, BPTR output_f, CONST_STRPTR library_s, CONST_STRPTR prefix_s)
 {
	BOOL success_flag = FALSE;
	STRPTR interface_s = NULL;
	
	ENTER ();

	interface_s = GetInterfaceName (library_s);
	
	if (interface_s)
		{
			if (WriteLibraryFunctionImplementation (output_f, fn_def_p, interface_s, prefix_s))
				{
					success_flag = TRUE;
				}
			else
				{
					DB (KPRINTF ("%s %ld - Failed to write implementation to %s\n", __FILE__, __LINE__, fn_def_p -> fd_header_filename_s));
				}
				
			IExec->FreeVec (interface_s);
		}
	else
		{
			DB (KPRINTF ("%s %ld - GetInterfaceName failed for library \"%s\" in  %s\n", __FILE__, __LINE__, library_s, fn_def_p -> fd_header_filename_s));
		}

	LEAVE ();
	return success_flag;
}


BOOL WriteSourceForFunctionDeclaration (const struct FunctionDefinition *fn_def_p, BPTR output_f, CONST_STRPTR library_s, CONST_STRPTR prefix_s)
 {
	BOOL success_flag = FALSE;
	STRPTR interface_s = NULL;
	
	ENTER ();

	interface_s = GetInterfaceName (library_s);
	
	if (interface_s)
		{
			if (WriteLibraryFunctionDefinition (output_f, interface_s, prefix_s, fn_def_p))
				{
					success_flag = TRUE;
				}
			else
				{
					DB (KPRINTF ("%s %ld - Failed to write declaration to %s\n", __FILE__, __LINE__, fn_def_p -> fd_header_filename_s));
				}
				
			IExec->FreeVec (interface_s);
		}
	else
		{
			DB (KPRINTF ("%s %ld - GetInterfaceName failed for library \"%s\" in  %s\n", __FILE__, __LINE__, library_s, fn_def_p -> fd_header_filename_s));
		}

	LEAVE ();
	return success_flag;
}



static BOOL WriteIncludes (BPTR out_p, CONST_STRPTR header_name_s, CONST_STRPTR lib_name_s)
{
	BOOL success_flag = FALSE;
	CONST_STRPTR local_name_s = NULL;

	ENTER ();

	local_name_s = IDOS->FilePart (header_name_s);
	
	if (IDOS->FPrintf (out_p, "#include \"%s\"\n\n", local_name_s) > 0)
		{
			if (IDOS->FPrintf (out_p, "#include \"proto/%s.h\"\n\n", lib_name_s) > 0)
				{
					success_flag = TRUE;
				}
		}
	
	
	LEAVE ();
	
	return success_flag;
}





BOOL WriteFunctionDefinitionListIncludes (BPTR out_p, struct List *function_definitions_list_p, CONST CONST_STRPTR prefix_s, CONST CONST_STRPTR suffix_s)
{
	ENTER ();

	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_definitions_list_p);
	CONST_STRPTR current_filename_s = "";
	
	while ((node_p != NULL) && success_flag)
		{
			CONST_STRPTR next_filename_s = node_p -> fdn_function_def_p -> fd_header_filename_s;
			
			if (strcmp (current_filename_s, next_filename_s) != 0)
				{
					CONST_STRPTR local_name_s = IDOS->FilePart (next_filename_s);
					
					if ((!prefix_s) || (IDOS->FPrintf (out_p, prefix_s) >= 0))
						{
							if (IDOS->FPrintf (out_p, "%s", local_name_s) >= 0)
								{
									if (suffix_s && (IDOS->FPrintf (out_p, suffix_s) < 0))
										{
											success_flag = FALSE;
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
						
					current_filename_s = next_filename_s;
				} 

			if (success_flag)
				{
					node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
				}
		}

	LEAVE ();
	return success_flag;
}


