#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "parameter.h"

/**************************************/
/************ PROTOTYPES **************/
/**************************************/

char *GetFunctionArguments (const char *function_s);

/************************************************/


/*

1. Mark the first "(" and the ");" wrap the parameters which are delimited by commas
2. Extract this substring and split into a list by splitting at the commas.
3. For each string on this list
	a. Trim at both ends to remove trailing whitespace
	b. Find the final space which should delimit the parameter name
	c. Check for any dereferencing characters at the start of the param name
	d. Does the name have an array ([]) specification
		i. TO DO
	e. The text prior to this space denotes the parameter type.


*/

int main (int argc, char *argv [])
{

	return 0;
}


char *GetFunctionArguments (const char *function_s)
{
	/* find the end of method */
	const char *closing_bracket_p = strstr (function_s, ");");

	/*
		Now count backwards through the string until we
		find the matching opening bracket
	*/

	/* We start at 1 since we have the closing bracket */
	int num_open_backets = 1;
	int num_params = 1;

	const char *opening_bracket_p = closing_bracket_p - 1;

	BOOL loop_flag = opening_bracket_p > function_s;

	while (loop_flag)
		{
			switch (*opening_bracket_p)
				{
					case '(':
						-- num_open_backets;
						break;

					case ')':
						++ num_open_backets;
						break;

					case ',':
						++ num_params;
						break;

					default:
						break;
				}

			if (num_open_backets == 0)
				{
					loop_flag = FALSE;
				}
			else
				{
					-- opening_bracket_p;
					loop_flag = opening_bracket_p > function_s;
				}

		}		/* while (loop_flag) */

	/* Have we matched the final closing bracket */
	if (num_open_backets == 0)
		{
			struct ParameterArray *params_p = AllocateParameterArray (num_params);

			if (params_p)
				{
					struct Parameter *param_p = params_p -> pa_params_p;
					const char *start_p = opening_bracket_p;
					const char *end_p = strchr (start_p, ',');
					BOOL success_flag = TRUE;

					while ((end_p != NULL) && (success_flag == TRUE))
						{
							if (FillInParameter (param_p, start_p, end_p))
								{
									++ param_p;
									start_p = end_p + 1;
									end_p = strchr (start_p, ',');
								}
							else
								{
									success_flag = FALSE;
								}
						}

					if (success_flag)
						{
							/* now we have the final param left to do */
							end_p = closing_bracket_p - 1;
							success_flag = FillInParameter (param_p, start_p, end_p);
						}

				}		/* if (params_p) */

		}		/* if (num_open_backets == 0) */

	return NULL;
}



