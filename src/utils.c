#include <ctype.h>
#include <string.h>

#include "debugging_utils.h"
#include "memory.h"
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
	char *dest_p = NULL;

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



char *TokenizeFunctionPrototype (const char *prototype_s)
{
	char *buffer_s = (char *) AllocMemory (strlen (prototype_s) + 1);

	if (buffer_s)
		{
			const char *src_p = prototype_s;
			char *dest_p = buffer_s;
			BOOL space_flag = FALSE;

			while (*src_p != '\0')
				{					
					if (isspace (*src_p))
						{
							if (!space_flag)
								{
									*dest_p = '-';
									++ dest_p;
									space_flag = TRUE;
								}
						}			
					else
						{
							if (space_flag)
								{
									space_flag = FALSE;
								}

							*dest_p = *src_p;
							++ dest_p;
						}		
					
					++ src_p;
				}

			*dest_p = '\0';

			DB (KPRINTF ("%s %ld - buffer: \"%s\"\n", __FILE__, __LINE__, buffer_s));		
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

