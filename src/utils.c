#include <ctype.h>
#include <string.h>

#include "debugging_utils.h"
#include "memory.h"
#include "string_list.h"
#include "utils.h"
#include "parameter.h"





static struct Parameter *GetNextParameter (const char *start_p, const char **end_pp);

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

			if (end_p)
				{
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
		}

	DB (KPRINTF ("%s %ld - Copying \"%s\" - \"%s\" to a new string\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));

	if (start_p < end_p)
		{
			size_t len = end_p - start_p + 1;
						
			char *dest_p = (char *) AllocMemory (len + 1);

			DB (KPRINTF ("%s %ld - len %ld\n", __FILE__, __LINE__, len));

			if (dest_p)
				{
					memcpy (dest_p, start_p, len);
					* (dest_p + len) = '\0';

					return dest_p;
				}
		}
	else
		{
			DB (KPRINTF ("%s %ld - ERROR: \"%s\" is after \"%s\"\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));
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

	DB (KPRINTF ("%s %ld - Tokenizing \"%s\"\n", __FILE__, __LINE__, prototype_s));

	if (fd_p)
		{
			const char *opening_bracket_p = strchr (prototype_s, '(');
			DB (KPRINTF ("%s %ld - opening_bracket \"%s\"\n", __FILE__, __LINE__, opening_bracket_p ? opening_bracket_p : "NULL"));

			if (opening_bracket_p)
				{
					/* scroll to the end of the function name */
					const char *name_end_p = ScrollPastWhitespace (opening_bracket_p - 1, prototype_s, NULL, SB_WHITESPACE, FALSE);
					DB (KPRINTF ("%s %ld - name_end \"%s\"\n", __FILE__, __LINE__, name_end_p ? name_end_p : "NULL"));

					if (name_end_p)
						{
							/* scroll to the start of the function name */
							const char * const delimiters_s = "*[]";
							const char *name_start_p = ScrollPastWhitespace (name_end_p, prototype_s, delimiters_s, SB_NON_WHITESPACE, FALSE);
							DB (KPRINTF ("%s %ld - name_start \"%s\"\n", __FILE__, __LINE__, name_start_p ? name_start_p : "NULL"));
							
							
							if (name_start_p)
								{
									char *function_name_s = CopyToNewString (name_start_p, name_end_p, TRUE);
									DB (KPRINTF ("%s %ld - >>>> function name \"%s\"\n", __FILE__, __LINE__, function_name_s ? function_name_s : "NULL"));
									
									if (function_name_s)
										{
											char *function_type_s = CopyToNewString (prototype_s, name_start_p - 1, TRUE);
											DB (KPRINTF ("%s %ld - >>>> function_type_s \"%s\"\n", __FILE__, __LINE__, function_type_s ? function_type_s : "NULL"));											
											
											if (function_type_s)
												{
													struct Parameter *param_p = AllocateParameter (function_type_s, function_name_s);													

													if (param_p)
														{
															const char *closing_bracket_p = strrchr (prototype_s, ')');

															fd_p -> fd_definition_p = param_p;

															if (closing_bracket_p)
																{
																	BOOL loop_flag = TRUE;

																	// Move to the characters directly within the brackets 
																	++ opening_bracket_p;
																	-- closing_bracket_p;

																	DB (KPRINTF ("%s %ld - opening_bracket_p \"%s\" end_p \"%s\"\n", __FILE__, __LINE__, opening_bracket_p, closing_bracket_p));

																	// now get each of the parameters 
																	while (loop_flag)
																		{
																			/* Are there any intermediate brackets? */
																			const char *c_p = strchr (opening_bracket_p, '(');

																			if (c_p) 
																				{

																				}
																			else
																				{
																					/* 
																						No brackets so we can split the string on commas to
																						get each parameter
																					*/

																					const char *start_p = opening_bracket_p;										
																					const char *comma_p = strchr (start_p, ',');
																					BOOL param_loop_flag = (start_p < closing_bracket_p) && (comma_p != NULL);

																					while (param_loop_flag)
																						{
																							/* work back from the comma past any whitespace */
																							const char *param_name_end_p = ScrollPastWhitespace (comma_p, start_p, NULL, SB_NON_WHITESPACE, FALSE);
																							if (param_name_end_p)
																								{
																									const char *param_name_start_p = ScrollPastWhitespace (param_name_end_p - 1, start_p, "[]*", SB_WHITESPACE, TRUE);

																									if (param_name_start_p)
																										{
																											const char *param_type_end_p = ScrollPastWhitespace (param_name_start_p - 1, start_p, NULL, SB_WHITESPACE, TRUE);
																										
																											if (param_type_end_p)
																												{
																													
																												}
																										}

																								}

																							start_p = comma_p + 1;										
																							comma_p = strchr (start_p, ',');

																							param_loop_flag = (start_p < closing_bracket_p) && (comma_p != NULL);
																						}

																				}

																			param_p = GetNextParameter (opening_bracket_p, &end_p);

																			DB (KPRINTF ("%s %ld - end_p \"%s\"\n", __FILE__, __LINE__, end_p ? end_p : "NULL"));

																			if (param_p)
																				{
																					DB (KPRINTF ("%s %ld - >>>> param type \"%s\" name \"%s\"\n", __FILE__, __LINE__, param_p -> pa_type_s, param_p -> pa_name_s));
																				
																					if (AddParameterAtFront (fd_p, param_p))
																						{
																							loop_flag = end_p > opening_bracket_p;
																						}
																					else
																						{
																							loop_flag = FALSE;
																						}
																				}
																			else
																				{
																					loop_flag = FALSE;
																				}

																		}		/* while (loop_flag) */
																		
																	return fd_p;
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
static struct Parameter *GetNextParameter (const char *start_p, const char **end_pp)
{
	char *name_s = NULL;
	char *type_s = NULL;

	/* scroll to the end of the function name */
	const char *name_end_p = ScrollPastWhitespace (*end_pp, start_p, NULL, SB_WHITESPACE, FALSE);

	if (name_end_p)
		{
			/* scroll to the start of the function name */
			const char *name_start_p = ScrollPastWhitespace (name_end_p, start_p, "*[]", SB_NON_WHITESPACE, FALSE);

			/* TODO - Check for function pointer */
			DB (KPRINTF ("%s %ld - name_start_p \"%s\"\n", __FILE__, __LINE__, name_start_p ? name_start_p : "NULL"));

			if (name_start_p)
				{
					name_s = CopyToNewString (name_start_p, name_end_p, FALSE);
					DB (KPRINTF ("%s %ld - name_s \"%s\"\n", __FILE__, __LINE__, name_s ? name_s : "NULL"));
					
					if (name_s)
						{
							const char * const delimiters_s = "*[]";						
							const char *type_end_p = ScrollPastWhitespace (name_start_p - 1, start_p, delimiters_s, SB_NON_WHITESPACE, FALSE);
							DB (KPRINTF ("%s %ld - type_end_p \"%s\"\n", __FILE__, __LINE__, type_end_p ? type_end_p : "NULL"));
							
							if (type_end_p)
								{
									const char *type_start_p = ScrollPastWhitespace (type_end_p, start_p, ",", SB_IGNORE, FALSE);
									DB (KPRINTF ("%s %ld - type_start_p \"%s\"\n", __FILE__, __LINE__, type_start_p ? type_start_p : "NULL"));
														
									if (type_start_p)
										{
											type_s = CopyToNewString (type_start_p, type_end_p, TRUE);
				
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
				
												}		/* if (type_s) */
										
										}		/* if (type_start_p) */
								
								}		/* if (type_end_p) */
							
						}		/* if (function_name_s) */

				}		/* if (name_start_p) */

		}		/* if (name_end_p) */

	return NULL;
}



//BOOL GetFunctionParameter


const char *ScrollPastWhitespace (const char *text_p, const char * const bounds_p, const char * const delimiters_s, const enum SpaceBehaviour sb, const BOOL stop_on_delimiters_flag)
{
	BOOL loop_flag = TRUE;
	const int inc = (text_p < bounds_p) ? 1 : -1;
	const char *res_p = NULL;

	DB (KPRINTF ("%s %ld - text \"%s\" bounds \"%s\" delimiters \"%s\" inc %ld space_flag %ld\n", 
		__FILE__, __LINE__, text_p ? text_p : "NULL", bounds_p ? bounds_p : "NULL", delimiters_s ? delimiters_s : "NULL", inc, sb));

	while (loop_flag)
		{
			DB (KPRINTF ("%s %ld - text_p '%c' \"%s\"\n", __FILE__, __LINE__, *text_p, text_p ? text_p : "NULL"));		
			
			if (text_p < bounds_p)
				{
					DB (KPRINTF ("%s %ld - Hit bounds\n", __FILE__, __LINE__));		
					loop_flag = FALSE;
				}
			else 
				{
					switch (sb)
						{
							case SB_WHITESPACE:
							case SB_NON_WHITESPACE:
								{
									enum SpaceBehaviour c = (isspace (*text_p) != 0) ? SB_WHITESPACE : SB_NON_WHITESPACE;
									loop_flag = (c == sb);
									
									DB (KPRINTF ("%s %ld - *text_p='%c' c=%ld sb=%ld loop_flag=%ld\n", __FILE__, __LINE__, *text_p, c, sb, loop_flag));
								}
								break;
						
							case SB_IGNORE:
							default:
								break;
						}
				}
				
			if ((loop_flag == TRUE) && (delimiters_s != NULL))
				{
					const char *c_p = strchr (delimiters_s, (int) *text_p);
					
					DB (KPRINTF ("%s %ld - c_p \"%s\"\n", __FILE__, __LINE__, c_p ? c_p : "NULL"));
					if (c_p && stop_on_delimiters_flag)
						{	
							res_p = text_p;
							loop_flag = FALSE;
						}
				}
				
			if (loop_flag)
				{
					text_p += inc;
				}
			else
				{
					res_p = text_p;
					loop_flag = FALSE;
				}
		}

	if (sb != SB_WHITESPACE)
		{
			DB (KPRINTF ("%s %ld - retracing res_p\n", __FILE__, __LINE__));
			res_p -= inc;
		}

	DB (KPRINTF ("%s %ld - res_p \"%s\"\n", __FILE__, __LINE__, res_p ? res_p : "NULL"));
	return res_p;
}

