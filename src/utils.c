#include <ctype.h>
#include <string.h>

#include "debugging_utils.h"
#include "memory.h"
#include "string_list.h"
#include "utils.h"
#include "parameter.h"


/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error.
 */
char *CopyToNewString (const char *start_p, const char *end_p, const BOOL trim_flag)
{
	if (trim_flag)
		{
			BOOL loop_flag = TRUE;

			while (loop_flag)
				{
					if (isspace (*start_p))
						{
							++ start_p;
							loop_flag = (start_p < end_p);
						}
					else
						{
							loop_flag = FALSE;
						}
				}

			loop_flag = start_p < end_p;
			while (loop_flag)
				{
					if (isspace (*end_p))
						{
							-- end_p;
							loop_flag = (start_p < end_p);
						}
					else
						{
							loop_flag = FALSE;
						}
				}
		}

	if (start_p != end_p)
		{
			size_t len = end_p - start_p;
			char *dest_p = (char *) AllocMemory (len + 1);

			if (dest_p)
				{
					memcpy (dest_p, start_p, len);
					* (dest_p + len) = '\0';

					return dest_p;
				}
		}

	return NULL;
}

static char PeekNextChar (const char *text_s, BOOL ignore_whitespace_flag);

static char PeekNextChar (const char *text_s, BOOL ignore_whitespace_flag)
{
	if (*text_s != '\0')
		{
			if (ignore_whitespace_flag)
				{
					while (isspace (*text_s))
						{
							++ text_s;
						}
				}
			else
				{
					++ text_s;
				}
		}

	return *text_s;
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
	struct FunctionDefinition *fd_p = AllocateFunctionDefinition ();

	if (fd_p)
		{
			const char *opening_bracket_p = strchr (prototype_s, '(');

			if (opening_bracket_p)
				{
					/* scroll to the end of the function name */
					const char *name_end_p = ScrollPastWhitespace (opening_bracket_p, prototype_s, NULL, TRUE);
					
					if (name_end_p)
						{
							/* scroll to the start of the function name */
							const char * const delimiters_s = "*[]";
							const char *name_start_p = ScrollPastWhitespace (opening_bracket_p, prototype_s, delimiters_s, FALSE);

							if (name_start_p)
								{
									char *function_name_s = CopyToNewString (name_start_p, name_end_p, FALSE);

									if (function_name_s)
										{
											char *function_type_s = CopyToNewString (prototype_s, name_start_p - 1, TRUE);

											if (function_type_s)
												{
													struct Parameter *param_p = AllocateParameter (function_type_s, function_name_s);
													
													if (param_p)
														{
															const char *end_p = strrchr (prototype_s, ')');
															
															fd_p -> fd_definition_p = param_p;
															
															if (end_p)
																{
																	
																	/* now get each of the parameters */
																	while ((param_p = GetNextParameter (opening_bracket_p + 1, &end_p)) != NULL)
																		{
																			AddParameterAtFront (fd_p, param_p);
																		}
																} 
														}			
												}
										}

								}		/* if (name_end_p) */

						}		/* if (name_end_p) */

				}		/* if (opening_bracket_p) */

		}

	return NULL;
}

/**
 * Scroll backwards from the given end point and return a parameter if one
 * can be created before we hit the given start point.
 *
 * @param start_p The start point which is the definite stopping point.
 * @param end_pp Where we still start building the parameter from. If we
 * successfully create the parameter, this will be updated to point to where
 * to start building the next parameter from.
 * @return A new parameter or NULL if one could not be created.
 */ 
struct Parameter *GetNextParameter (const char *start_p, const char **end_pp)
{
	char *name_s = NULL;
	char *type_s = NULL;

	/* scroll to the end of the function name */
	const char *name_end_p = ScrollPastWhitespace (*end_pp, start_p, NULL, TRUE);

	if (name_end_p)
		{
			/* scroll to the start of the function name */
			const char *name_start_p = ScrollPastWhitespace (name_end_p, start_p, "*[]", FALSE);

			/* TODO - Check for function pointer */
			

			if (name_start_p)
				{
					name_s = CopyToNewString (name_start_p, name_end_p, FALSE);

					if (name_s)
						{
							const char *type_start_p = ScrollPastWhitespace (name_end_p, start_p, ",", FALSE);
							
							type_s = CopyToNewString (type_start_p, name_start_p - 1, TRUE);

							if (type_s)
								{
									struct Parameter *param_p = AllocateParameter (name_s, type_s);
									
									if (param_p)
										{
											/* 
												update current position and set to NULL if we have reached
												the start position to signal that we have completed without
												error. 
										  */
											if (type_start_p > start_p)
												{
													*end_pp = type_start_p - 1;
												}
											else
												{
													*end_pp = NULL;
												}											
											
											return param_p;
										}		/* if (param_p) */
									
								}		/* if (function_type_s) */
						
						}		/* if (function_name_s) */

				}		/* if (name_start_p) */

		}		/* if (name_end_p) */	
		
	return NULL;
}



//BOOL GetFunctionParameter


const char *ScrollPastWhitespace (const char *text_p, const char * const bounds_p, const char * const delimiters_s, const BOOL space_flag)
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
			else if (space_flag == (isspace (*text_p) != 0))
				{
					text_p += inc;
				}
			else if ((delimiters_s != NULL) && (strchr (delimiters_s, *text_p) != NULL))
				{
					res_p = text_p - inc;
					loop_flag = FALSE;
				}
			else
				{
					res_p = text_p - inc;
					loop_flag = FALSE;
				}
		}

	return res_p;
}

