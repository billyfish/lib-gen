#include <ctype.h>
#include <string.h>

#ifdef AMIGA
#include <proto/exec.h>
#endif

#include "debugging_utils.h"

#include "memory.h"
#include "parameter.h"
#include "utils.h"


/**************************/

static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p);

/**************************/


static BOOL IsParameterFunctionPointer (const char *start_p, const char *end_p);



static BOOL IsParameterFunctionPointer (const char *start_p, const char *end_p)
{
	const char *data_p = start_p;
	BOOL is_param_function_pointer_flag = FALSE;

	while (data_p && (data_p < end_p))
		{
			data_p = strchr (data_p, '(');

			if (data_p)
				{
					if (* (data_p + 1) == '*')
						{
							is_param_function_pointer_flag = TRUE;

							/* Force exit from loop */
							data_p = NULL;
						}
					else
						{
							++ data_p;
						}

				}		/* if (data_p) */

		}		/* while (data_p && (data_p < end_p)) */

	return is_param_function_pointer_flag;
}


struct Parameter *ParseParameter (const char *start_p, const char *end_p)
{
	struct Parameter *param_p = NULL;
	const char *type_start_p = NULL;
	const char *type_end_p = NULL;
	const char *name_start_p = NULL;
	const char *name_end_p = end_p;
	uint8 array_count = 0;
	uint8 pointer_count = 0;
	uint8 loop_flag = TRUE;
	BOOL matched_flag = FALSE;

	/*
		Start from the end and work backwards
	*/
	while (loop_flag && !matched_flag)
		{
			const char c = *name_end_p;

			if (isspace (c))
				{

				}
			else if (c == ']')
				{
					/* find the opening array bracket */
					BOOL matched_flag = FALSE;

					while (loop_flag && !matched_flag)
						{
							-- name_end_p;

							if (*name_end_p == '[')
								{
									++ array_count;
									matched_flag = TRUE;
								}
							else
								{
									loop_flag = (start_p < name_end_p);
								}
						}

					if (matched_flag)
						{
							/* reset the matched flag */
							matched_flag = FALSE;
						}
					else
						{
							// error;
						}
				}
			else if (isalnum (c))
				{
					matched_flag = TRUE;
				}

			if (!matched_flag)
				{
					-- name_end_p;
					loop_flag = (start_p < name_end_p);
				}
		}

	/* Have we found the end of the name? */
	if (matched_flag)
		{
			/* Now scroll to the start of the name */
			name_start_p = name_end_p - 1;
			matched_flag = FALSE;
			loop_flag = TRUE;

			while (loop_flag && !matched_flag)
				{
					const char c = *name_start_p;

					if (isspace (c) || (c == '*'))
						{
							matched_flag = TRUE;
							++ name_start_p;
						}
					else
						{
							-- name_start_p;
							loop_flag = (start_p < name_start_p);
						}
				}

			/* Have we found the start of the name? */
			if (matched_flag)
				{
					/* scroll to the end of the type*/
					type_end_p = name_start_p - 1;
					matched_flag = FALSE;
					loop_flag = (type_end_p > start_p);

					while (loop_flag && !matched_flag)
						{
							const char c = *type_end_p;

							if (isspace (c))
								{
									-- type_end_p;
									loop_flag = (type_end_p > start_p);
								}
							else
								{
									matched_flag = TRUE;
								}
						}


					if (matched_flag)
						{
							/* scroll to the start of the type*/
							type_start_p = start_p;
							matched_flag = FALSE;
							loop_flag = (type_start_p < type_end_p);

							while (loop_flag && !matched_flag)
								{
									const char c = *type_start_p;

									if (isspace (c))
										{
											++ type_start_p;
											loop_flag = (type_start_p < type_end_p);
										}
									else
										{
											matched_flag = TRUE;
										}
								}

							if (matched_flag)
								{
									// success;
									char *name_s = CopyToNewString (name_start_p, name_end_p, FALSE);

									if (name_s)
										{
											char *type_s = CopyToNewString (type_start_p, type_end_p, FALSE);

											if (type_s)
												{
													param_p = AllocateParameter (type_s, name_s);
												}

										}
								}
						}



				}
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
struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s)
{
	BOOL success_flag = FALSE;
	struct FunctionDefinition *fd_p = AllocateFunctionDefinition ();

	DB (KPRINTF ("%s %ld - Tokenizing \"%s\"\n", __FILE__, __LINE__, prototype_s));

	if (fd_p)
		{
			const char *opening_bracket_p = strchr (prototype_s, '(');
			DB (KPRINTF ("%s %ld - opening_bracket \"%s\"\n", __FILE__, __LINE__, opening_bracket_p ? opening_bracket_p : "NULL"));

			if (opening_bracket_p)
				{
					struct Parameter *param_p = ParseParameter (prototype_s, opening_bracket_p - 1);

					if (param_p)
						{
							const char *start_p = opening_bracket_p + 1;
							const char *end_p = strchr (start_p, ',');
							BOOL loop_flag = (end_p != NULL);

							fd_p -> fd_definition_p = param_p;

							success_flag = TRUE;

							/* Get all of the parameters before each comma */
							while (loop_flag && success_flag)
								{
									param_p = ParseParameter (start_p, end_p);

									if (param_p)
										{
											if (AddParameterAtBack (fd_p, param_p))
												{
													start_p = end_p + 1;

													if (start_p < prototype_s)
														{
															end_p = strchr (start_p, ',');
															loop_flag = (end_p != NULL);
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
									const char *start_p = (end_p != NULL) ? end_p + 1 : opening_bracket_p + 1;
									const char *end_p = strchr (start_p, ')');

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



struct FunctionDefinition *AllocateFunctionDefinition (void)
{
	struct FunctionDefinition *fd_p = (struct FunctionDefinition *) AllocMemory (sizeof (struct FunctionDefinition));

	if (fd_p)
		{
			#ifdef AMIGA
			struct List *params_p = (struct List *) IExec->AllocSysObjectTags (ASOT_LIST,
				ASOLIST_Type, PT_PARAMETER,
				TAG_DONE);
			#else
			struct List *params_p = AllocateParameterList ();
			#endif

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


void FreeParameterList (struct List *params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) GET_HEAD (params_p);
	struct ParameterNode *next_node_p = NULL;

	while (curr_node_p)
		{
			next_node_p = (struct ParameterNode *) GET_SUCC (& (curr_node_p -> pn_node));

			FreeParameterNode (curr_node_p);

			curr_node_p = next_node_p;
		}

#ifdef AMIGA
	IExec->FreeSysObject (ASOT_LIST, params_p);
#else
	FreeList (params_p);
#endif
	
}


struct List *AllocateParameterList (void)
{
#ifdef AMIGA
	struct List *params_p = (struct List *) IExec->AllocSysObjectTags (ASOT_LIST,
		ASOLIST_Type, PT_PARAMETER,
		TAG_DONE);
#else
	struct List *params_p = AllocateList (NULL);
#endif
	
	return params_p;
}


void FreeParameterNode (struct ParameterNode *node_p)
{
	if (node_p -> pn_param_p)
		{
			FreeParameter (node_p -> pn_param_p);
		}

#ifdef AMIGA
	IExec->FreeSysObject (ASOT_NODE, node_p);
#else
	FreeMemory (node_p);
#endif
}


struct ParameterNode *AllocateParameterNode (struct Parameter *param_p)
{
#ifdef AMIGA
	struct ParameterNode *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
//		ASONODE_Type, PT_PARAMETER,
		TAG_DONE);
#else
	struct ParameterNode *node_p = (struct ParameterNode *) AllocMemory (sizeof (struct ParameterNode));
#endif
	
	if (node_p)
		{
			node_p -> pn_param_p = param_p;

			return node_p;
		}

	return NULL;
}



struct Parameter *AllocateParameter (char *type_s, char *name_s)
{
	struct Parameter *param_p = (struct Parameter *) AllocMemory (sizeof (struct Parameter));

	if (param_p)
		{
			param_p -> pa_name_s = name_s;
			param_p -> pa_type_s = type_s;

			return param_p;
		}

	return NULL;
}


void FreeParameter (struct Parameter *param_p)
{
	ClearParameter (param_p);
	FreeMemory (param_p);
}


void ClearParameter (struct Parameter *param_p)
{
	if (param_p -> pa_name_s)
		{
			FreeMemory (param_p -> pa_name_s);
		}

	if (param_p -> pa_type_s)
		{
			FreeMemory (param_p -> pa_type_s);
		}

}


BOOL SetParameterName (struct Parameter *param_p, const char *start_p, const char *end_p)
{
	return SetParameterValue (& (param_p -> pa_name_s), start_p, end_p);
}


BOOL SetParameterType (struct Parameter *param_p, const char *start_p, const char *end_p)
{
	return SetParameterValue (& (param_p -> pa_type_s), start_p, end_p);
}


BOOL AddParameterAtFront (struct FunctionDefinition *fd_p, struct Parameter *param_p)
{
	struct ParameterNode *node_p = AllocateParameterNode (param_p);

	if (node_p)
		{
			ADD_HEAD (fd_p -> fd_args_p, (struct Node *) node_p);

			return TRUE;
		}

	return FALSE;

}


BOOL AddParameterAtBack (struct FunctionDefinition *fd_p, struct Parameter *param_p)
{
	struct ParameterNode *node_p = AllocateParameterNode (param_p);

	if (node_p)
		{
			ADD_TAIL (fd_p -> fd_args_p, (struct Node *) node_p);

			return TRUE;
		}

	return FALSE;
}


static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p)
{
	BOOL success_flag = FALSE;
	size_t l;
	char *copy_s = NULL;

DB (KPRINTF ("%s %ld -  setting param value to: \"%s\" - \"%s\"\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));

	if (end_p)
		{
			l = end_p - start_p;
		}
	else
		{
			l = strlen (start_p);
		}

	while ((start_p != end_p) && (isspace (*start_p)))
		{
			++ start_p;
			-- l;
		}

	if (end_p)
		{
			if (start_p != end_p)
				{
					while ((start_p != end_p) && (isspace (*end_p)))
						{
							-- end_p;
							-- l;
						}
				}
		}
		
	if (start_p != end_p)
		{
			copy_s = (char *) AllocMemory (l + 1);

			if (copy_s)
				{
					if (*param_value_ss)
						{
							FreeMemory (*param_value_ss);
						}

					strncpy (copy_s, start_p, l);
					* (copy_s + l) = '\0';

					DB (KPRINTF ("%s %ld -  setting param value to: \"%s\"\n", __FILE__, __LINE__, copy_s));

					*param_value_ss = copy_s;

					success_flag = TRUE;
				}

		}		/* f (start_p != end_p) */

	return success_flag;
}


BOOL PrintParameter (FILE *out_f, const struct Parameter * const param_p)
{
	BOOL success_flag;

	if (param_p -> pa_type_s)
		{
			success_flag = (fprintf (out_f, "%s -", param_p -> pa_type_s) >= 0);
		}
	else
		{
			success_flag = (fprintf (out_f, "NULL -") >= 0);
		}

	if (success_flag)
		{
			if (param_p -> pa_name_s)
				{
					success_flag = (fprintf (out_f, " %s", param_p -> pa_name_s) >= 0);
				}
			else
				{
					success_flag = (fprintf (out_f, " NULL") >= 0);
				}
		}

	return success_flag;
}

BOOL PrintParameterList (FILE *out_f, struct List * const params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) GET_HEAD (params_p);
	struct ParameterNode *next_node_p = NULL;
	uint32 i = 0;

	while (curr_node_p)
		{
			next_node_p = (struct ParameterNode *) GET_SUCC (& (curr_node_p -> pn_node));

			fprintf (out_f, " %lu: ", i);
			if (PrintParameter (out_f, curr_node_p -> pn_param_p))
				{
					fprintf (out_f, "\n");
					++ i;
				}
			else
				{
					return FALSE;
				}

			curr_node_p = next_node_p;
		}



	return TRUE;
}

BOOL PrintFunctionDefinition (FILE *out_f, const struct FunctionDefinition * const fn_p)
{
	BOOL success_flag = FALSE;

	if (fprintf (out_f, "FUNCTION:\n") >= 0)
		{
			if (PrintParameter (out_f, fn_p -> fd_definition_p))
				{
					if (fprintf (out_f, "\nPARAMS:\n") >= 0)
						{
							success_flag = PrintParameterList (out_f, fn_p -> fd_args_p);
						}
				}
		}

	return success_flag;
}
