#include <ctype.h>
#include <string.h>

#ifdef AMIGA
#include <proto/dos.h>
#include <proto/exec.h>
#endif

#include "debugging_utils.h"

#include "memory.h"
#include "parameter.h"
#include "utils.h"


/**************************/

static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p);

/**************************/


BOOL IsParameterFunctionPointer (const char *start_p, const char *end_p)
{
	const char *data_p = start_p;
	BOOL is_param_function_pointer_flag = FALSE;

	while (data_p && (data_p < end_p))
		{
			data_p = strchr (data_p, '(');

			while (data_p && (data_p < end_p))
				{				
					++ data_p;
					
					if (*data_p == '*')
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

	return FALSE; //is_param_function_pointer_flag;
}


struct Parameter *ParseFunctionPointerParameter (const char *start_p, const char **end_pp)
{
	struct Parameter *param_p = NULL;
	const char *type_start_p = NULL;
	const char *type_end_p = NULL;
	const char *name_start_p = strchr (start_p, '(');
	const char *name_end_p = *end_pp;
	char *name_s = NULL;
	uint8 array_count = 0;
	
	if (name_start_p)
		{
			const char *data_p = name_start_p;
			BOOL loop_flag = TRUE;
			BOOL matched_flag = FALSE;
			BOOL space_flag = TRUE;
			
			/* find the closing bracket */
			while (loop_flag && !matched_flag)
				{
					const char c = *data_p;
					
					if (c == ')')
						{
							matched_flag = TRUE;
						}		
					else if (space_flag && (!isspace (c)))
						{
							space_flag = FALSE;
							
							if (c == '*')
								{
									
									loop_flag = FALSE;
								}
							
						}
						
					if (loop_flag && !matched_flag)
						{
							++ data_p;
							
							loop_flag = (data_p && (data_p < *end_pp));
						}
				}
				
				
			if (matched_flag)
				{
					loop_flag = TRUE;
					matched_flag = FALSE;
					
					
				}
		}
	

				
				
	return param_p;
}


struct Parameter *ParseParameter (const char *start_p, const char *end_p)
{
	struct Parameter *param_p = NULL;
	const char *type_start_p = NULL;
	const char *type_end_p = NULL;
	const char *name_start_p = NULL;
	const char *name_end_p = end_p;
	char *name_s = NULL;
	char *type_s = NULL;
	uint8 array_count = 0;
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

	DB (KPRINTF ("%s %ld -  name_end_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, name_end_p, start_p, (int) matched_flag));

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

			DB (KPRINTF ("%s %ld -  name_start_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, name_start_p,  start_p, (int) matched_flag));

			if (!matched_flag)
				{
					DB (KPRINTF ("%s %ld -  checking void match name_start_p: \"%s\" name_end_p: \"%s\" diff %ld\n", __FILE__, __LINE__, name_start_p,  name_end_p, (int) (name_end_p - name_start_p)));
					
					if ((name_end_p - name_start_p == 3) && (strncmp ("void", name_start_p, 4) == 0))
						{
							type_s = CopyToNewString (name_start_p, name_end_p, FALSE);
							
							matched_flag = (type_s != NULL);
							name_s = NULL;
						}				
						
					DB (KPRINTF ("%s %ld -  void match %ld\n", __FILE__, __LINE__, (int) matched_flag));
				}
			else
				{
					// success;
					name_s = CopyToNewString (name_start_p, name_end_p, FALSE);
					
					DB (KPRINTF ("%s %ld -  setting param name to: \"%s\" from \"%s\"\n", __FILE__, __LINE__, name_s ? name_s : "NULL", start_p));
					
					if (!name_s)
						{
							IDOS->Printf ("Failed to allocate memory for param name");
							matched_flag = FALSE;
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
		
		
							DB (KPRINTF ("%s %ld -  type_end_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, type_end_p,  start_p, (int) matched_flag));
		
							/* Did we get the end of the type? */
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
		
									DB (KPRINTF ("%s %ld -  type_end_p to: \"%s\" from \"%s\" %ld\n", __FILE__, __LINE__, type_end_p,  start_p, (int) matched_flag));
								
								}
								
							if (matched_flag)
								{
									type_s = CopyToNewString (type_start_p, type_end_p, FALSE);
								}
						}	
				}
		
			
			if (matched_flag)
				{
					param_p = AllocateParameter (type_s, name_s);
				}
		}

	return param_p;

}


BOOL IsVoidParameter (const struct Parameter *param_p)
{
	return ((strcmp (param_p -> pa_type_s, "void") == 0) && (param_p -> pa_type_s == NULL));
}


void FreeParameterList (struct List *params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (params_p);
	struct ParameterNode *next_node_p = NULL;

	while (curr_node_p)
		{
			next_node_p = (struct ParameterNode *) IExec->GetSucc (& (curr_node_p -> pn_node));

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
		ASONODE_Size, sizeof (struct ParameterNode),
		TAG_DONE);


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


BOOL WriteParameterAsSource (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "%s", param_p -> pa_type_s) >= 0)
		{
			if (param_p -> pa_name_s)
				{
					if (IDOS->FPrintf (out_p, " %s", param_p -> pa_name_s) >= 0)
						{
							success_flag = TRUE;
						}
				}
			else
				{
					success_flag = TRUE;
				}
		}
		
	return success_flag;
}


BOOL PrintParameter (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = TRUE;

	if (param_p)
		{
			if (param_p -> pa_type_s)
				{
					success_flag = (IDOS->FPrintf (out_p, "%s -", param_p -> pa_type_s) >= 0);
				}
			else
				{
					success_flag = (IDOS->FPrintf (out_p, "NULL -") >= 0);
				}

			if (success_flag)
				{
					if (param_p -> pa_name_s)
						{
							success_flag = (IDOS->FPrintf (out_p, " %s", param_p -> pa_name_s) >= 0);
						}
					else
						{
							success_flag = (IDOS->FPrintf (out_p, " NULL") >= 0);
						}
				}
		}

	return success_flag;
}

BOOL PrintParameterList (BPTR out_p, struct List * const params_p)
{
	struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (params_p);
	struct ParameterNode *next_node_p = NULL;
	uint32 i = 0;

	while ((next_node_p = (struct ParameterNode *) IExec->GetSucc (& (curr_node_p -> pn_node))) != NULL)
		{
			IDOS->FPrintf (out_p, " %lu: ", i);
			if (PrintParameter (out_p, curr_node_p -> pn_param_p))
				{
					IDOS->FPrintf (out_p, "\n");
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

