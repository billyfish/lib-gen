/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include "memory_allocations.h"
#include "string_utils.h"
#include "typedefs.h"
#include "byte_buffer.h"


static const char *ScrollPastSpecificCharacters (const char **value_pp, const char * const delimiters_s, const bool ignore_whitespace_flag, const bool state_flag);


void FreeCopiedString (char *str_p)
{
	if (str_p)
		{
			FreeMemory (str_p);
		}
}

/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error.
 */
char *CopyToNewString (const char * const src_p, const size_t l, bool trim)
{
	char *dest_p = NULL;

	if (src_p)
		{
			size_t len = strlen (src_p);

			if (l > 0)
				{
					if (len > l)
						{
							len = l;
						}
				}

			if (trim)
				{
					const char *start_p = src_p;
					const char *end_p = src_p + len - 1;

					while (isspace (*start_p))
						{
						 ++ start_p;
						}

					if (*start_p == '\0')
						{
							/* all white space */
							return NULL;
						}

					while ((start_p < end_p) && (isspace (*end_p)))
						{
						 -- end_p;
						}

					if (start_p <= end_p)
						{
							ptrdiff_t d = end_p - start_p + 1;
							dest_p = (char *) AllocMemory (d + 1);

							if (dest_p)
								{
									memcpy (dest_p, start_p, d);
									* (dest_p + d) = '\0';

									return dest_p;
								}

						}
				}
			else
				{
					dest_p = (char *) AllocMemory (len + 1);

					if (dest_p)
						{
							strncpy (dest_p, src_p, len);
							* (dest_p + len) = '\0';

							return dest_p;
						}
				}
		}

	return dest_p;
}


/**
 * Read a line from a file with no buffer overflow issues.
 *
 * @param in_f The FILE to read from.
 * @param buffer_pp Pointer to a buffer string where the line will get written to.
 * After being used, *buffer_pp muet be freed.
 * @return true if the line was read successfully, false if there was an error.
 */
bool GetLineFromFile (FILE *in_f, char **buffer_pp)
{
	const size_t buffer_size = 1024;
	size_t last_index = 0;
	size_t old_size = 0;
	size_t buffer_length = 0;
	char *old_buffer_p = NULL;
	char *new_buffer_p = NULL;
	size_t latest_block_length = 0;
	size_t new_size = 0;
	size_t actual_length = 0;

	if (!feof (in_f))
		{
			bool looping = true;

			do
				{
					new_size = old_size + buffer_size;

					new_buffer_p = (char *) ReallocMemory (old_buffer_p, new_size, old_size);

					if (new_buffer_p)
						{
							char * const latest_block_p = new_buffer_p + buffer_length;

							* (new_buffer_p + new_size - 1) = '\0';

							if (new_buffer_p != old_buffer_p)
								{
									old_buffer_p = new_buffer_p;
								}

							if (fgets (latest_block_p, buffer_size, in_f))
								{
									char c;

									latest_block_length = strlen (latest_block_p);

									buffer_length += latest_block_length;
									last_index = buffer_length - 1;
									old_size = new_size;

									c = * (new_buffer_p + last_index);
									looping = (c != '\r') && (c != '\n') && (!feof (in_f));
								}
							else
								{
									*latest_block_p = '\0';
									looping = false;
								}
						}
					else
						{
							return false;
						}
				}
			while (looping);

			/* Remove the carriage return and/or line feed */
			actual_length = (new_buffer_p != NULL) ? strlen (new_buffer_p) : 0;
			if (actual_length > 0)
				{
					char *c_p = new_buffer_p + actual_length - 1;

					looping = true;
					while (looping)
						{
							if ((*c_p == '\r') || (*c_p == '\n'))
								{
									*c_p = '\0';

									if (c_p > new_buffer_p)
										{
											-- c_p;
										}
									else
										{
											looping = false;
										}
								}
							else
								{
									looping = false;
								}

						}		/* while (looping) */

				}		/* if (actual_length > 0) */

			*buffer_pp = new_buffer_p;
			return true;
		}

	*buffer_pp = NULL;
	return false;
}


void FreeLineBuffer (char *buffer_p)
{
	FreeMemory (buffer_p);
}


char *ConcatenateStrings (const char * const first_s, const char * const second_s)
{
	const size_t len1 = (first_s != NULL) ? strlen (first_s) : 0;
	const size_t len2 = (second_s != NULL) ? strlen (second_s) : 0;

	char *result_s = (char *) AllocMemory (sizeof (char) * (len1 + len2 + 1));

	if (result_s)
		{
			if (len1 > 0)
				{
					strncpy (result_s, first_s, len1);
				}

			if (len2 > 0)
				{
					strcpy (result_s + len1, second_s);
				}

			* (result_s + len1 + len2) = '\0';
		}

	return result_s;
}





/**
 * Scroll through a string until we reach a charcter that satisfies some input constraints.
 *
 * @param state_flag If this is true then keep scrolling until we find some non-whitespace/delimiters, if false keep
 * scrolling until we find some whitespace/delimiters.
 */
static const char *ScrollPastSpecificCharacters (const char **value_pp, const char * const delimiters_s, const bool check_for_whitespace_flag, const bool state_flag)
{
	const char *value_s = *value_pp;
	const size_t delimiters_length = (delimiters_s != NULL) ? strlen (delimiters_s) : 0;
	bool is_delimiter_flag;
	bool loop_flag = (*value_s != '\0');

	while (loop_flag)
		{
			const char c = *value_s;

			is_delimiter_flag = false;

			/* Are we're checking for whitespace? */
			if (check_for_whitespace_flag)
				{
					is_delimiter_flag = (isspace (c) != 0);
				}		/* if (check_for_whitespace_flag) */

			/* if the whitespace check failed, test against our delimiters */
			if (!is_delimiter_flag)
				{
					/* If necessary, check for any delimiters */
					if (delimiters_length > 0)
						{
							size_t i = delimiters_length;
							const char *delim_p = delimiters_s;

							while (i > 0)
								{
									is_delimiter_flag = (*delim_p == c);

									if (is_delimiter_flag)
										{
											i = 0;
										}
									else
										{
											-- i;
											++ delim_p;
										}

								}		/* while (i > 0) */

						}		/* if (delimiters_length > 0) */

				}		/* if (!is_delimter_flag) */


			/* Are we continuing? */
			if (is_delimiter_flag == state_flag)
				{
					++ value_s;

					loop_flag = (*value_s != '\0');
				}
			else
				{
					loop_flag = false;
				}

		}		/* while (loop_flag) */

	return value_s;
}


/**
 * Replace all instances of a charcter by another within a string
 *
 * @param value_s The string to be altered.
 * @param char_to_replace  The character to replace.
 * @param replacement_char The replacement character.
 */
void ReplaceExpression (char *value_s, const char char_to_replace, const char replacement_char)
{
	char *ptr = strchr (value_s, char_to_replace);

	while (ptr)
		{
			*ptr = replacement_char;
			ptr = strchr (ptr + 1, char_to_replace);
		}
}




/**
 * Get a copy of the next word from a buffer.
 *
 * @param buffer_p The buffer to read from.
 * @param end_pp If the word is successfully read, then this will
 * be where the orginal word  in the buffer ends.
 * @return A copied version of the string.
 */
char *GetNextToken (const char **start_pp, const char *delimiters_s, const bool check_for_whitespace_flag, const bool update_position_flag)
{
	char *word_s = NULL;
	const char *value_p = *start_pp;

	const char *start_p = ScrollPastSpecificCharacters (&value_p, delimiters_s, check_for_whitespace_flag, true);

	if (start_p && *start_p)
		{
			const char *end_p = NULL;
			bool quoted_token_flag = false;

			value_p = start_p;

			/* does it start with a " in which case we have a quoted string? */
			if (*start_p == '\"')
				{
					quoted_token_flag = true;

					++ value_p;

					end_p = ScrollPastSpecificCharacters (&value_p, "\"", false, false);
				}
			else
				{
					end_p = ScrollPastSpecificCharacters (&value_p, delimiters_s, check_for_whitespace_flag, false);
				}

			if (end_p)
				{

					/* scroll past the quotes  */
					if (quoted_token_flag)
						{
							++ start_p;
						}

					if (start_p != end_p)
						{
							const ptrdiff_t d = end_p - start_p;

							word_s = CopyToNewString (start_p, d, false);

							if (*end_p)
								{
									if (update_position_flag)
										{
											*start_pp = end_p + 1;
										}
								}
							else
								{
									*start_pp = end_p;
								}

						}		/* if (start_p != end_p) */

				}		/* if (end_p) */

		}		/* if (start_p) */

	return word_s;
}


void FreeToken (char *token_s)
{
	FreeMemory (token_s);
}




bool IsStringEmpty (const char *value_s)
{
	if (value_s)
		{
			while (*value_s)
				{
					if (isspace (*value_s))
						{
							++ value_s;
						}
					else
						{
							return false;
						}
				}
		}

	return true;
}





int Stricmp (const char *c0_p, const char *c1_p)
{
	int res = 0;

	if (c0_p != c1_p)
		{
			bool loop_flag = true;
			int i0;
			int i1;
			char c0;
			char c1;

			while (loop_flag)
				{
					c0 = tolower (*c0_p);
					c1 = tolower (*c1_p);

					if ((c0 == c1) && (c0 != '\0'))
						{
							++ c0_p;
							++ c1_p;
						}
					else
						{
							loop_flag = false;
						}
				}

			/* The comparison must be performed as if the characters were unsigned characters. */
			i0 = (unsigned char) c0;
			i1 = (unsigned char) c1;

			res = i0 - i1;
		}

	return res;
}


int Strnicmp (const char *c0_p, const char *c1_p, size_t length)
{
	int res = 0;

	if (c0_p != c1_p)
		{
			bool loop_flag = (length > 0);
			int i0;
			int i1;
			char c0;
			char c1;

			while (loop_flag)
				{
					c0 = tolower (*c0_p);
					c1 = tolower (*c1_p);

					if ((c0 == c1) && (c0 != '\0'))
						{
							++ c0_p;
							++ c1_p;

							-- length;
							if (length == 0)
								{
									loop_flag = false;
								}
						}
					else
						{
							loop_flag = false;
						}
				}

			/* The comparison must be performed as if the characters were unsigned characters. */
			i0 = (unsigned char) c0;
			i1 = (unsigned char) c1;

			res = i0 - i1;
		}

	return res;
}


const char *Stristr (const char *value_s, const char *substring_s)
{
	const char *res_p = NULL;

	if (value_s != substring_s)
		{
			const char *value_p = value_s;
			const char *substring_p = substring_s;

			while (*value_p && *substring_p)
		    {
					/* Do they match? */
					if (tolower (*value_p) == tolower (*substring_p))
		        {
							if (!res_p)
		            {
		              res_p = value_p;
		            }

							++ substring_p;
		        }
					else
		        {
							substring_p = substring_s;

							if (tolower (*value_p) == tolower (*substring_p))
				        {
									res_p = value_p;
									++ substring_p;
				        }
							else
								{
									res_p = NULL;
								}
		        }

					++ value_p;
		    }		/* while (*value_p && *substring_p) */
		}
	else
		{
			res_p = value_s;
		}

	return res_p;
}



bool ReplaceStringValue (char **dest_ss, const char *value_s)
{
	bool success_flag = false;
	char *new_value_s = CopyToNewString (value_s, 0, false);

	if (new_value_s)
		{
			if (*dest_ss)
				{
					FreeCopiedString (*dest_ss);
				}

			*dest_ss = new_value_s;
			success_flag = true;
		}

	return success_flag;
}



void ReplaceChars (char *value_s, char old_data, char new_data)
{
	char *value_p = value_s;

	while (*value_p != '\0')
		{
			if (*value_p == old_data)
				{
					*value_p = new_data;
				}

			++ value_p;
		}
}




char *ConcatenateVarargsStrings (const char *value_s, ...)
{
	char *result_s = NULL;
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			va_list args;

			va_start (args, value_s);

			if (AppendVarArgsToByteBuffer (buffer_p, value_s, args))
				{
					result_s = DetachByteBufferData (buffer_p);
				}		/* if (AppendVarArgsToByteBuffer (buffer_p, value_s, args)) */

			va_end (args);
		}		/* if (buffer_p) */

	return result_s;
}


bool DoesStringContainWhitespace (const char *value_s)
{
  while (*value_s != '\0')
  	{
  		if (isspace (*value_s))
  			{
  				return true;
  			}
  		else
  			{
  				++ value_s;
  			}
  	}

  return false;
}



char *ScrollPastWhitespace (char *data_s)
{
	char *start_s = NULL;


	start_s = data_s;

	if (start_s)
		{
			/*
				Check to see whether possible function prototype
			*/
			bool loop_flag = true;

			/* scroll to the start of the next token */
			while (loop_flag)
				{
					if (*start_s == '\0')
						{
							start_s = NULL;
							loop_flag = false;
						}
					else if (isspace (*start_s))
						{
							++ start_s;
						}
					else
						{
							loop_flag = false;
						}
				}

		}		/* if (start_s) */


	return start_s;
}



char *GetFileContentsAsStringByFilename (const char *filename_s)
{
	char *data_s = NULL;

	if (filename_s)
		{
			FILE *in_f = fopen (filename_s, "r");

			if (in_f)
				{
					int res = 0;

					data_s = GetFileContentsAsString (in_f);

					res = fclose (in_f);

					if (res)
						{
							printf ("Error %d closing %s", res, filename_s);
						}

				}		/* if (in_f) */
			else
				{
					printf ( "Failed to open file %s", filename_s);
				}

		}		/* if (filename_s) */
	else
		{
			printf ( "Filename is NULL");
		}

	return data_s;
}

char *GetFileContentsAsString (FILE *input_f)
{
	char *data_s = NULL;
	long int current_pos = ftell (input_f);

	// Determine file size
	fseek (input_f, 0, SEEK_END);
	long int size = ftell (input_f);

	if (size != -1)
		{
			data_s = (char *) malloc ((size + 1) * sizeof (char));

			if (data_s)
				{
					size_t read_size = 0;

					rewind (input_f);

					read_size = fread (data_s, sizeof (char), size, input_f);

					if (read_size == (size_t) size)
						{
							* (data_s + size) = '\0';

							return data_s;
						}		/* if (read_size == size) */
					else
						{
							printf ("Failed to read all of file %ld / %ld bytes ", read_size, (size_t) size);
						}

					fseek (input_f, current_pos, SEEK_SET);

					free (data_s);
				}
			else
				{
					printf ( "Failed to allocate %ld bytes ", (size_t) size);
				}
		}
	else
		{
			printf( "Failed to determine file size");
		}

	return NULL;
}
