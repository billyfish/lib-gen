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

/**
 * @file
 * @brief
 */
/**@file string_utils.h
 * @brief A set of string functions.
 *
 *  A variety of functions for manipulating strings.
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdio.h>

#include "typedefs.h"

#ifdef __cplusplus
	extern "C" {
#endif


/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param length_to_copy If this is greater than zero, copy this many characters from src_p
 * or its entirety, whichever is smaller. If this is zero or less then all of src_p is copied.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error. If
 * the string was successfully created, its deallocation must be done with FreeCopiedString
 * when you are finished with it.
 */
char *CopyToNewString (const char * const src_p, const size_t length_to_copy, bool trim);


/**
 * Delete a string previously allocated using CopyToNewString.
 *
 * @param str_p The string to deallocate.
 */
void FreeCopiedString (char *str_p);


/**
 * Read a line from a file with no buffer overflow issues.
 *
 * @param in_f The File to read from.
 * @param buffer_pp Pointer to a buffer string where the line will get written to.
 * After being used, *buffer_pp muet be freed.
 * @return TRUE if the line was read successfully, false if there was an error.
 */
bool GetLineFromFile (FILE *in_f, char **buffer_pp);


/**
 * Free a buffer that has been allocated with GetLineFromFile.
 *
 * @param buffer_p Pointer to a buffer to be freed.
 */
void FreeLineBuffer (char *buffer_p);




/**
 * Get a newly created string that is the concatenation of two others.
 *
 * @param first_s  The first string to concatenate.
 * @param second_s  The second string to concatenate.
 * @return The newly created string or NULL if there was an error.
 */
 char *ConcatenateStrings (const char * const first_s, const char * const second_s);


/**
 * Replace all instances of a charcter by another within a string
 *
 * @param value_s The string to be altered.
 * @param char_to_replace  The character to replace.
 * @param replacement_char The replacement character.
 */
 void ReplaceExpression (char *value_s, const char char_to_replace, const char replacement_char);




/**
 * Get a copy of the next word from a buffer.
 *
 * @param start_pp Address of the buffer to read from.
 * @param delimiters_p An array of characters that can be used as delimiters. If this is NULL
 * then the behvaiour is determined by check_for_whitespace_flag.
 * @param check_for_whitespace_flag
 * @param update_position_flag If this is set to <code>true</code>, then the address pointed to by start_pp will be updated to after
 * the token.
 * @return A copied version of the string or <code>NULL</code> if there was an error or no more tokens were available.
 * @see FreeToken
 */
char *GetNextToken (const char **start_pp, const char *delimiters_p, const bool check_for_whitespace_flag, const bool update_position_flag);

/**
 * Free a c-style string that has been returned by GetNextToken.
 *
 * @param token_s The token to free.
 * @see GetNextToken
 */
void FreeToken (char *token_s);



/**
 * Test whether a string is NULL or contains just whitespace.
 *
 * @param value_s The string to test.
 * @return <code>true</code> If the string is <code>NULL</code> or just whitespace, <code>false</code> otherwise.
 */
 bool IsStringEmpty (const char *value_s);


/**
 * Do a case-insensitive comparison between two strings.
 * This is a case-insensitive version of the standard ANSI function strcmp.
 *
 * @param c0_s The first string.
 * @param c1_s The second string.
 * @return Less than zero if the first string is less than the second,
 * greater than zero if the first string is greater than the second and
 * zero if the strings match.
 */
 int Stricmp (const char *c0_s, const char *c1_s);


/**
 * Do a case-insensitive comparison between the initial portions of two strings.
 * This is a case-insensitive version of the standard ANSI function strncmp.
 *
 * @param c0_s The first string.
 * @param c1_s The second string.
 * @param length The maximum number of characters in each string to compare.
 * @return Less than zero if the first string is less than the second,
 * greater than zero if the first string is greater than the second and
 * zero if the strings match.
 */
 int Strnicmp (const char *c0_s, const char *c1_s, size_t length);


/**
 * Find a substring within a string. The search is case-insensitive.
 * This is a case-insensitive version of the standard ANSI function strstr.
 *
 * @param value_s The string to search within.
 * @param substring_s The string to search for
 * @return The substring within the larger string or <code>NULL</code> if it was
 * not found.
 */
 const char *Stristr (const char *value_s, const char *substring_s);



/**
 * Make a copy of a string.
 *
 * @param dest_ss Pointer to the string where the source string
 * will be copied to.  * @param src_s The string to copy.
 * @return <code>true</code> if successful, <code>false</code> otherwise.
 * If successful, any string that dest_ss currently points to will be freed
 * prior to pointing at the copy. If the function is unsuccessful then the value
 * pointed to by dest_ss will remain intact.
 */
 bool ReplaceStringValue (char **dest_ss, const char * const src_s);


/**
 * Replace each instance of a character within a string with another
 *
 * @param value_s The string to replace the character in.
 * @param old_data The character to be replaced.
 * @param new_data The replacement character.
 */
 void ReplaceChars (char *value_s, char old_data, char new_data);




/**
 * Get the contents of FILE as c-style string.
 *
 * @param input_f The FILE to get the contents of
 * @return The newly-allocated c-style string containing the contents
 * of the FILE or <code>NULL</code> upon error. If this value is not
 * <code>NULL</code>, it should be freed by calling FreeCopiedString
 * when it is no longer needed.
 * @see FreeCopiedString
 */
 char *GetFileContentsAsString (FILE *input_f);



/**
 * Concatenate a va_list of strings.
 *
 * @param value_s The varargs-style array of <code>NULL</code> terminated strings to append. The final entry
 * in this varargs-array must be a <code>NULL</code>.
 * @return The newly-allocated string that should be freed with FreeCopiedString when
 * no longer needed or <code>NULL</code> upon failure.
 */
 char *ConcatenateVarargsStrings (const char *value_s, ...);





/**
 * Test whether a given string contains any whitespace characters.
 *
 * @param value_s The string to check.
 * @return <code>true</code> if the string contains any whitespace characters, <code>false</code> otherwise.
 */
 bool DoesStringContainWhitespace (const char *value_s);


 char *ScrollPastWhitespace (char *data_s);


 /**
  * Get the contents of FILE as c-style string.
  *
  * @param filename_s The filename to get the contents of
  * @return The newly-allocated c-style string containing the contents
  * of the FILE or <code>NULL</code> upon error. If this value is not
  * <code>NULL</code>, it should be freed by calling FreeCopiedString
  * when it is no longer needed.
  * @see GetFileContentsAsString
  */
 char *GetFileContentsAsStringByFilename (const char *filename_s);

 char *GetFileContentsAsString (FILE *input_f);

#ifdef __cplusplus
}
#endif





#endif	/* #ifndef STRING_UTILS_H */

