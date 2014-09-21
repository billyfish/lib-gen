/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: LibraryGenerator
**
** File:
**
** Date: 16-06-2013 14:13:11
**
************************************************************

*/

#include <stdio.h>
#include <string.h>

#include <dos/dos.h>

#include <exec/types.h>
#include <exec/exectags.h>
#include <exec/lists.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include "unit_test.h"

static BOOL OpenLibs (void);
static void CloseLibs (void);

BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p);
BOOL ParseFiles (CONST_STRPTR pattern_s, CONST_STRPTR filename_s);


int main (int argc, char *argv [])
{
	if (OpenLibs ())
		{
			if (argc > 2)
				{
					ParseFiles (argv [1], argv [2]);
				}
			else if ((argc == 2) && (strcmp ("test", argv [1]) == 0))
				{
					UnitTest ();
				}
			else
				{
					IDOS->Printf ("LibraryGenerator <pattern> <filename>\n");
				}

			CloseLibs ();
		}
	else
		{
			printf ("no libs\n");
		}

	return 0;
}



BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			int32 count;

			while ((count = IDOS->FReadLine (handle_p, line_p)) > 0)
				{
					if (IUtility->Strnicmp (pattern_s, line_p -> frld_Line, pattern_length) == 0)
						{
							IDOS->Printf (">>> matched line:= %s", line_p -> frld_Line);
						}
					else
						{
							IDOS->Printf ("line:= %s", line_p -> frld_Line);
						}
				}

			success_flag = (count == 0);

			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
		}

	return success_flag;
}




BOOL ParseFiles (CONST_STRPTR pattern_s, CONST_STRPTR filename_s)
{
	struct FReadLineData *line_data_p = IDOS->AllocDosObject (DOS_FREADLINEDATA, 0);
	BOOL success_flag = FALSE;

	if (line_data_p)
		{
			BOOL read_file_flag;
			const size_t pattern_length = strlen (pattern_s);

			read_file_flag = GetMatchingPrototypes (filename_s, pattern_s, pattern_length, line_data_p);

			if (read_file_flag)
				{
					IDOS->Printf ("read \"%s\" successfully\n", filename_s);
				}
			else
				{
					IDOS->Printf ("failed to read \"%s\"\n", filename_s);
				}


			IDOS->FreeDosObject (DOS_FREADLINEDATA, line_data_p);
		}		/* if (line_data_p) */

	return success_flag;
}




static BOOL OpenLibs (void)
{
	if (OpenLib (&DOSBase, "dos.library", 52L, (struct Interface **) &IDOS, "main", 1))
		{
			if (OpenLib (&UtilityBase, "utility.library", 52L, (struct Interface **) &IUtility, "main", 1))
				{
					return TRUE;
				}

			CloseLib (DOSBase, (struct Interface *) IDOS);
		}

	return FALSE;
}

static void CloseLibs (void)
{
	CloseLib (UtilityBase, (struct Interface *) IUtility);
	CloseLib (DOSBase, (struct Interface *) IDOS);
}

