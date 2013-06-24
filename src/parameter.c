#include <ctype.h>
#include <string.h>

#include <proto/exec.h>

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





BOOL FillInParameter (struct Parameter *param_p, const char *start_p, const char *end_p)
{
	BOOL success_flag = FALSE;

	BOOL is_param_function_pointer_flag = IsParameterFunctionPointer (start_p, end_p);


	/*
		After trimming each end, the param name should start after the final space. It may
		be preceeded by a dereferencer such as * or &. Or it could be a function pointer
	*/



	if (is_param_function_pointer_flag)
		{
			DB (KPRINTF ("%s %ld - \"%s\" is a function pointer\n", __FILE__, __LINE__, start_p));

		}
	else
		{
			/* scroll to the end of the name */
			end_p = ScrollPastWhitespace (end_p, start_p, NULL, TRUE);

			if (end_p)
				{
					/* now grab the name */
					const char *name_start_p = ScrollPastWhitespace (end_p, start_p, NULL, FALSE);

					if (name_start_p)
						{
							const char *type_end_p = ScrollPastWhitespace (name_start_p, start_p, NULL, TRUE);

							if (type_end_p)
								{
									if (SetParameterType (param_p, start_p, type_end_p))
										{
											if (SetParameterName (param_p, name_start_p, end_p))
												{
													DB (KPRINTF ("%s %ld - param type: \"%s\" name: \"%s\" \n", __FILE__, __LINE__, param_p -> pa_type_s, param_p -> pa_name_s));

													success_flag = TRUE;
												}		/* if (SetParameterName (param_p, name_start_p, end_p)) */

										}		/* if (SetParameterType (param_p, start_p, type_end_p)) */

								}		/* if (type_end_p) */

						}		/* if (name_start_p) */

				}		/* if (end_p) */

		}		/* if (is_param_function_pointer_flag) else */

	return success_flag;
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


void FreeParameterList (struct List *params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (params_p);
	struct ParameterNode *next_node_p = NULL;

	while (curr_node_p)
		{
			next_node_p = (struct ParameterNode *) IExec -> GetSucc (& (curr_node_p -> pn_node));

			FreeParameterNode (curr_node_p);

			curr_node_p = next_node_p;
		}
		
	IExec->FreeSysObject (ASOT_LIST, params_p);
}


struct List *AllocateParameterList (void)
{
	struct List *params_p = (struct List *) IExec->AllocSysObjectTags (ASOT_LIST, 
		ASOLIST_Type, PT_PARAMETER,
		TAG_DONE);

	return params_p;
}


void FreeParameterNode (struct ParameterNode *node_p)
{
	if (node_p -> pn_param_p)
		{
			FreeParameter (node_p -> pn_param_p);
		}

	IExec->FreeSysObject (ASOT_NODE, node_p);
}


struct ParameterNode *AllocateParameterNode (struct Parameter *param_p)
{
	struct ParameterNode *node_p = IExec->AllocSysObjectTags (ASOT_NODE, 
		ASONODE_Type, PT_PARAMETER,
		TAG_DONE);

	if (node_p)
		{
			node_p -> pn_param_p = param_p;

			return node_p;
		}

	return NULL;
}



struct Parameter *AllocateParameter (const char *type_s, const char *name_s)
{
	struct Parameter *param_p = (struct Parameter *) AllocMemory (sizeof (struct Parameter));

	if (param_p)
		{
			BOOL success_flag = TRUE;

			param_p -> pa_name_s = NULL;
			param_p -> pa_type_s = NULL;

			if (name_s)
				{
					if (!SetParameterName (param_p, name_s, NULL))
						{
							success_flag = FALSE;
						}
				}

			if (success_flag)
				{
					if (type_s)
						{
							if (!SetParameterType (param_p, type_s, NULL))
								{
									success_flag = FALSE;
								}
						}

				}

			if (success_flag)
				{
					return param_p;
				}

			FreeParameter (param_p);
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


static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p)
{
	BOOL success_flag = FALSE;
	size_t l;
	char *copy_s = NULL;

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

	if (start_p != end_p)
		{
			while ((start_p != end_p) && (isspace (*end_p)))
				{
					-- end_p;
					-- l;
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
			success_flag = (fprintf (out_f, "%s - ", param_p -> pa_type_s) == 1);
		}
	else
		{
			success_flag = (fprintf (out_f, "NULL - ") >= 0);
		}

	if (success_flag)
		{
			if (param_p -> pa_name_s)
				{
					success_flag = (fprintf (out_f, "%s - ", param_p -> pa_name_s) == 1);
				}
			else
				{
					success_flag = (fprintf (out_f, "NULL") >= 0);
				}
		}

	return success_flag;
}

BOOL PrintParameterList (FILE *out_f, struct List * const params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (params_p);
	struct ParameterNode *next_node_p = NULL;
	uint32 i = 0;
	
	while (curr_node_p)
		{
			next_node_p = (struct ParameterNode *) IExec -> GetSucc (& (curr_node_p -> pn_node));

			fprintf (out_f, "%lu: ", i);
			if (PrintParameter (out_f, curr_node_p -> pn_param_p))
				{
					fprintf (out_f, " ");
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
	BOOL success_flag = PrintParameter (out_f, fn_p -> fd_definition_p);

	if (success_flag)
		{
			success_flag = PrintParameterList (out_f, fn_p -> fd_args_p);
		}

	return success_flag;
}
