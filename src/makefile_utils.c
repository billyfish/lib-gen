/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File:
**
** Date: 02-10-2014 22:43:42
**
************************************************************

*/

#include <proto/dos.h>
#include <proto/exec.h>


#include "makefile_utils.h"
#include "utils.h"



#ifdef _DEBUG
#define MAKEFILE_UTILS_DEBUG (1)
#endif


BOOL WriteMakefileHeader (BPTR makefile_p, CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (makefile_p, "# Use our generated source rather than the stub files created by idltool\n\n%s_SRCS = \\\n", library_s) >= 0)
		{
			success_flag = TRUE;
		}

	return success_flag;
}


BOOL WriteMakefileFooter (BPTR makefile_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (makefile_p, "\n\ninclude Makefile\n") >= 0)
		{
			success_flag = TRUE;
		}

	return success_flag;
}

BOOL AddFileToMakefileSources (BPTR makefile_p, CONST_STRPTR filename_s)
{
	BOOL success_flag = TRUE;

	if (makefile_p)
		{
			if (IDOS->FPrintf (makefile_p, "\t%s \\\n", filename_s) < 0)
				{
					IDOS->Printf ("Error writing %s to list of sources in makefile\n", filename_s);
					success_flag = FALSE;
				}
		}
	else
		{
			IDOS->Printf ("makefile handle is null");
		}

	return success_flag;
}


BOOL CloseMakefile (BPTR makefile_p)
{
	return (IDOS->FClose (makefile_p) != 0);
}


BPTR GetMakefileHandle (CONST_STRPTR library_s)
{
	BPTR makefile_p = ZERO;
	STRPTR makefile_s = ConcatenateStrings (library_s, ".mk");

	if (makefile_s)
		{
			makefile_p = IDOS->FOpen (makefile_s, MODE_NEWFILE, 0);

			if (makefile_p)
				{
					if (!WriteMakefileHeader (makefile_p, library_s))
						{
							IDOS->Printf ("Failed to write header block to makefile \"%s\"\n", makefile_s);
						}
				}
			else
				{
					IDOS->Printf ("Failed to open makefile \"%s\"\n", makefile_s);
				}

			IExec->FreeVec (makefile_s);
		}
	else
		{
			IDOS->Printf ("Failed to create makefile name for \"%s\"\n", library_s);
		}

	return makefile_p;
}

