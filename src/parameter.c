#include <ctype.h>
#include <string.h>

#include "debugging_utils.h"

#include "memory.h"
#include "parameter.h"


/**************************/

static BOOL SetParameterValue (char **param_value_ss, const char *start_p, const char *end_p);

/**************************/


static BOOL IsParameterFunctionPointer (const char *start_p, const char *end_p);

static const char *ScrollPastWhiteSpace (const char *text_p, const char * const bounds_p);

static const char *ScrollPastWhiteSpace (const char *text_p, const char * const bounds_p)
{
	BOOL loop_flag = TRUE;
	const int inc = (text_p < bounds_p) ? 1 : -1;
	const char *res_p = NULL;

	while (loop_flag)
		{
			if (text_p == bounds_p)
				{
					loop_flag = FALSE;
				}
			else if (isspace (*text_p))
				{
					text_p += inc; 
				}
			else
				{
					res_p = text_p;
					loop_flag = FALSE;
				}
		}

	return res_p;
}


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
			BOOL loop_flag = TRUE;
			BOOL found_name_flag = FALSE;
			const char *data_p = end_p;

			/* scroll to the start of the param name */
			while (loop_flag && !found_name_flag)
				{
					if (isspace (*data_p))
						{
							/* scroll through any whitespace */
							data_p = ScrollPastWhiteSpace (data_p, start_p);
							
							if (data_p)
								{
									found_name_flag = TRUE;
								}
						}
					else if (*data_p == '*')
						{
							found_name_flag = TRUE;

						}
					else
						{
							-- data_p;
							loop_flag = (data_p != start_p);
						}

				}		/* while (loop_flag) */

					
			DB (KPRINTF ("%s %ld -  data_p: \"%s\"\n", __FILE__, __LINE__, data_p));		
			DB (KPRINTF ("%s %ld -  start: \"%s\"\n", __FILE__, __LINE__, start_p));
			DB (KPRINTF ("%s %ld -  end: \"%s\"\n", __FILE__, __LINE__, end_p));		
			
			if (found_name_flag)
				{
					if (SetParameterType (param_p, start_p, data_p))
						{
							/* scroll to the first char of the name */
							++ data_p;

							if (data_p)
								{
									if (SetParameterName (param_p, data_p, end_p))
										{
											DB (KPRINTF ("%s %ld - param type: \"%s\" name: \"%s\" \n", __FILE__, __LINE__, param_p -> pa_type_s, param_p -> pa_name_s));		
								
											success_flag = TRUE;
										}

								}

						}

				}

		}

	return success_flag;
}


struct ParameterArray *AllocateParameterArray (int num_params)
{
	struct Parameter *params_p = (struct Parameter *) AllocMemory (num_params * sizeof (struct Parameter));

	if (params_p)
		{
			struct ParameterArray *pa_p = (struct ParameterArray *) AllocMemory (sizeof (struct ParameterArray));

			if (pa_p)
				{
					int i;

					pa_p -> pa_params_p = params_p;
					pa_p -> pa_num_params = num_params;

					for (i = num_params; i > 0; -- i, ++ params_p)
						{
							params_p -> pa_name_s = NULL;
							params_p -> pa_type_s = NULL;							 
						}

					return pa_p;
				}		/* if (pa_p) */
			
			FreeMemory (params_p);
		}		/* if (params_p) */

	return NULL;
}


void FreeParameterArray (struct ParameterArray *params_p)
{
	int i = params_p -> pa_num_params;
	struct Parameter *param_p = params_p -> pa_params_p;

	for ( ; i > 0; -- i, ++ param_p)
		{
			ClearParameter (param_p);
		}
}


struct Parameter *AllocateParameter (const char *name_s, const char *type_s)
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

BOOL PrintParameterArray (FILE *out_f, const struct ParameterArray * const params_p)
{
	int i = params_p -> pa_num_params;
	const struct Parameter *param_p = params_p -> pa_params_p;
	
	fprintf (out_f, "%d: ", i);

	for ( ; i > 0; -- i, ++ param_p)
		{
			if (PrintParameter (out_f, param_p))
				{
					fprintf (out_f, " ");
				}
			else
				{
					return FALSE;
				}
		} 

	return TRUE;
}

BOOL PrintFunctionDefinition (FILE *out_f, const struct FunctionDefinition * const fn_p)
{
	BOOL success_flag = PrintParameter (out_f, fn_p -> fd_definition_p);

	if (success_flag)
		{
			success_flag = PrintParameterArray (out_f, fn_p -> fs_args_p);
		}

	return success_flag;
}
