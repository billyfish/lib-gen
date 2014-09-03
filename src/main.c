#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "parameter.h"
#include "utils.h"
#include "string_list.h"
#include "debugging_utils.h"

/**************************************/
/************ PROTOTYPES **************/
/**************************************/

struct FunctionDefinition *GetFunctionArguments (const char *function_s);

void UnitTest (const char * const prototype_s, FILE *out_f);

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
//	struct ParameterArray *params_p = NULL;
	FILE *out_f = stdout;

	UnitTest ("int main (int argc, char **argv);", out_f);

	UnitTest ("int				*GetAddress (const int **ptr, const int num);", out_f);
	UnitTest ("struct Test *GetTest (void);", out_f);
	UnitTest ("void SetTest (struct Test *test_p, int num);", out_f);
//	UnitTest ("void SetFunction (int (*test_fn) (int num));", out_f);

	return 0;
}



void UnitTest (const char * const prototype_s, FILE *out_f)
{
	struct FunctionDefinition *fd_p = TokenizeFunctionPrototype (prototype_s);
	
	if (fd_p)
		{
			fprintf (out_f, "********* BEGIN FD *********\n");
			PrintFunctionDefinition (out_f, fd_p);						
			fprintf (out_f, "\n********* END FD *********\n\n");
		//	FreeFunctionDefinition (fd_p);
		}
	else
		{
			fprintf (out_f, "No match for \"%s\"\n", prototype_s);
		}
}





void UnitTest2 (const char * const prototype_s, FILE *out_f)
{
	struct FunctionDefinition *fd_p = GetFunctionArguments (prototype_s);

	if (fd_p)
		{
			fprintf (out_f, "********* BEGIN FD *********\n");
			PrintFunctionDefinition (out_f, fd_p);						
			fprintf (out_f, "\n********* END FD *********\n\n");
			FreeFunctionDefinition (fd_p);
		}
	else
		{
			fprintf (out_f, "No match for \"%s\"\n", prototype_s);
		}
}


struct FunctionDefinition *GetFunctionArguments (const char *function_s)
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
			struct FunctionDefinition *fd_p = AllocateFunctionDefinition ();

			if (fd_p)
				{
					const char *end_p = ScrollPastWhitespace (opening_bracket_p - 1, function_s, NULL, SB_WHITESPACE, FALSE);

					if (end_p)
						{

						}

					FreeFunctionDefinition (fd_p);
				}		/* if (fd_p) */

		}		/* if (num_open_backets == 0) */

	return NULL;
}







