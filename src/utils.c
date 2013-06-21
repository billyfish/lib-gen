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



struct List *TokenizeFunctionPrototype (const char *prototype_s)
{
	struct List *tokens_p = AllocateStringList ();

	if (tokens_p)
		{
			//BOOL error_flag = FALSE;
			BOOL space_flag = TRUE;
			const char *token_start_p = NULL;
			const char *token_end_p = NULL;
			const char *index_p = prototype_s;
			int32 num_open_brackets = 0;
			
			while (*index_p != '\0')
				{		
					if (isspace (*index_p))
						{
							if (!space_flag)
								{
									if (token_start_p)
										{
											token_end_p = index_p - 1;
											
											AddStringNode (tokens_p, token_start_p, token_end_p);
											
										}
										
										
									space_flag = TRUE;
								}
						}			
					else
						{
							switch (*index_p)
								{
									case '*':
									
										break;
										
									case '(':
										/* either the start of a function pointer or the start of the function parameters */
										++ num_open_brackets;
										break;
										
										
									case ')':
									
										-- num_open_brackets;
										break;

									default:
										break;								
								}
													
						
							if (space_flag)
								{
									token_start_p = index_p;
									token_end_p = NULL;
									space_flag = FALSE;
								}
						}		
					
					++ index_p;
				}

			return tokens_p;
		}
	
	return NULL;
}


const char *ScrollPastWhiteSpace (const char *text_p, const char * const bounds_p, const BOOL space_flag)
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
			else
				{
					res_p = text_p;
					loop_flag = FALSE;
				}
		}

	return res_p;
}

