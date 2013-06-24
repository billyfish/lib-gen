#include <ctype.h>
#include <string.h>

#include "debugging_utils.h"
#include "memory.h"
#include "string_list.h"
#include "utils.h"


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

static const char PeekNextChar (const char *text_s, BOOL ignore_whitespace_flag);

static const char PeekNextChar (const char *text_s, BOOL ignore_whitespace_flag)
{
	if (*text_s != '\0)
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
struct List *TokenizeFunctionPrototype (const char *prototype_s)
{
	struct List *tokens_p = AllocateStringList ();

	if (tokens_p)
		{
			const char *opening_bracket_p = strchr (prototype_s, '(');

			if (opening_bracket_p)
				{
					char *function_name_s = NULL;
					char *function_type_s = NULL;

					/* scroll to the end of the function name */
					const char *name_end_p = ScrollPastWhitespace (opening_bracket_p, prototype_s, NULL, TRUE);

					if (name_end_p)
						{
							/* scroll to the start of the function name */
							const char * const delimiters_s = "*[]";
							const char *name_start_p = ScrollPastWhitespace (opening_bracket_p, prototype_s, delimiters_s, FALSE);

							if (name_start_p)
								{
									function_name_s = CopyToNewString (name_start_p, name_end_p, FALSE);

									if (function_name_s)
										{
											function_type_s = CopyToNewString (prototype_s, name_start_p - 1, TRUE);

											if (function_type_s)
												{

												}
										}

								}		/* if (name_end_p) */

						}		/* if (name_end_p) */

				}		/* if (opening_bracket_p) */

		}

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

