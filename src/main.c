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
	UnitTest ("void SetFunction (int (*test_fn) (int num));", out_f);

	return 0;
}



void UnitTest (const char * const prototype_s, FILE *out_f)
{
	struct List *tokens_p = TokenizeFunctionPrototype (prototype_s);

	if (tokens_p)
		{
			FreeStringList (tokens_p);
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
			struct FunctionDefinition *fd_p = AllocateFunctionDefinition (num_params);

			if (fd_p)
				{
					const char *end_p = ScrollPastWhitespace (opening_bracket_p - 1, function_s, NULL, TRUE);

					if (end_p)
						{
							/* Get the function name and return type */
							if (FillInParameter (fd_p -> fd_definition_p, function_s, end_p))
								{
									/* fill in the args */
									struct Parameter *param_p = fd_p -> fd_args_p -> pa_params_p;
									const char *start_p = opening_bracket_p;
									BOOL success_flag = TRUE;

									end_p = strchr (start_p, ',');
							
									DB (KPRINTF ("%s %ld -  start: \"%s\"\n", __FILE__, __LINE__, start_p));
									DB (KPRINTF ("%s %ld -  end: \"%s\"\n", __FILE__, __LINE__, end_p));		
					
									while ((end_p != NULL) && (success_flag == TRUE))
										{
											if (FillInParameter (param_p, start_p + 1, end_p))
												{
													printf (">>> param: ");
													PrintParameter (stdout, param_p);
													printf ("\n");
									
													++ param_p;
													start_p = end_p + 1;

													while (start_p && isspace (*start_p))
														{
															++ start_p;
														}

													if (start_p)
														{
															end_p = strchr (start_p, ',');
														}
													else
														{
															end_p = NULL;
														}
												}
											else
												{
													success_flag = FALSE;
												}
										}

									if (success_flag)
										{
											/* now we have the final param left to do */
											end_p = closing_bracket_p;
											success_flag = FillInParameter (param_p, start_p, end_p);

											return fd_p;
										}
						
								}		/* if (FillInParameter (fd_p -> fd_definition_p, function_s, opening_bracket_p)) */					

						}

					FreeFunctionDefinition (fd_p);
				}		/* if (fd_p) */

		}		/* if (num_open_backets == 0) */

	return NULL;
}







