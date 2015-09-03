#include <proto/dos.h>
#include <proto/exec.h>

#include "auto_init_writer.h"
#include "utils.h"


#define AUTO_INIT_CODE(library_s,header_s) ( \
	"/*\n" \
	"**	$Id$\n" \
	"**	Generated by IDLTool 53.1\n" \
	"**	Do not edit\n" \
	"**\n" \
	"**	AutoInit stub for " #library_s "\n" \
	"**\n" \
	"**	Copyright (c) 2010 Hyperion Entertainment CVBA.\n" \
	"**	All Rights Reserved.\n" \
	"*/\n" \
	"\n" \
	"#include <interfaces/" #header_s ".h>\n" \
	"#include <proto/exec.h>\n" \
	"#include <assert.h>\n" \
	"\n" \
	"/****************************************************************************/\n" \
	"\n" \
	"struct Library *" #library_s "Base = NULL;\n" \
	"static struct Library * __" #library_s "Base;\n" \
	"\n" \
	"/****************************************************************************/\n" \
	"\n" \
	"void " #library_s "BaseConstructor (void)\n" \
	"{\n" \
	"  if (" #library_s "Base != NULL)\n" \
	"    {\n" \
	"        return; /* Someone was quicker, e.g. an interface constructor */\n" \
	"    }\n" \
	"    __" #library_s "Base = " #library_s "Base = (struct Library *)IExec->OpenLibrary("", 0L);\n" \
	"    assert(" #library_s "Base != NULL);\n" \
	"}\n" \
	"__attribute__((section(\".ctors.zzzz\"))) static void\n" \
	"(*" #header_s "_base_constructor_ptr)(void) USED = " #library_s "BaseConstructor;\n" \
	"\n" \
	"/****************************************************************************/\n" \
	"\n" \
	"void " #library_s "BaseDestructor (void)\n" \
	"{\n" \
	"    if (__" #library_s "Base)\n" \
	"    {\n" \
	"        IExec->CloseLibrary((struct Library *)__" #library_s "Base);\n" \
	"    }\n" \
	"}\n" \
	"__attribute__((section(\".dtors.zzzz\"))) static void\n" \
	"(*" #header_s "_base_destructor_ptr)(void) USED = " #library_s "BaseDestructor;\n" \
	"\n" \
	"/****************************************************************************/\n")


BOOL WriteAutoInitSource (CONST CONST_STRPTR library_s, CONST CONST_STRPTR header_s)
{
	BOOL success_flag = FALSE;
	STRPTR file_s = ConcatenateStrings (header_s, "_auto_init.c");

	if (file_s)
		{
			BPTR out_p = IDOS->FOpen (file_s, MODE_NEWFILE, 0);

			if (out_p)
				{
					CONST_STRPTR code_s = AUTO_INIT_CODE (library_s, header_s);

					if (IDOS->FPrintf (out_p, "%s", code_s) >= 0)
						{
							success_flag = TRUE;
						}		/* if (IDOS->FPrintf (out_p, code_s) >= 0) */


					IDOS->FClose (out_p);
				}
			else
				{
					IDOS->Printf ("Failed to open \"%s\"\n", file_s);
				}

			IExec->FreeVec (file_s);
		}
	else
		{
			IDOS->Printf ("Failed to create vectors file name for \"%s\"\n", library_s);
		}

	return success_flag;


}
