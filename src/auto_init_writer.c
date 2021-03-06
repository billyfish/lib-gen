#include <proto/dos.h>
#include <proto/exec.h>

#include "auto_init_writer.h"
#include "utils.h"
#include "debugging_utils.h"


/*******/

STATIC BOOL WriteAutoInitFile (CONST_STRPTR library_s, CONST_STRPTR capitalized_library_s, CONST_STRPTR out_dir_s, CONST_STRPTR suffix_s, BOOL (*callback_fn) (BPTR out_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR capitalized_library_s));


STATIC BOOL WriteAutoInitLibraryCode (BPTR out_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR capitalized_library_s);


STATIC BOOL WriteAutoInitInterfaceCode (BPTR out_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR capitalized_library_s);


/*******/


BOOL WriteAutoInitFiles (CONST_STRPTR library_s, CONST_STRPTR out_dir_s)
{
	BOOL success_flag = FALSE;
	STRPTR capitalized_library_s = NULL;	

	ENTER ();

	capitalized_library_s = GetCapitalizedString (library_s);

	if (capitalized_library_s)
		{
			if (WriteAutoInitFile (library_s, capitalized_library_s, out_dir_s, "_library_auto_init.c", WriteAutoInitLibraryCode))
				{
					if (WriteAutoInitFile (library_s, capitalized_library_s, out_dir_s, "_interface_auto_init.c", WriteAutoInitInterfaceCode))
						{
							success_flag = TRUE;	
						}				
				}
				
			IExec->FreeVec (capitalized_library_s);
		}
		
	LEAVE ();
	return success_flag;
}


/*******/


STATIC BOOL WriteAutoInitFile (CONST_STRPTR library_s, CONST_STRPTR capitalized_library_s, CONST_STRPTR out_dir_s, CONST_STRPTR suffix_s, BOOL (*callback_fn) (BPTR out_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR capitalized_library_s))
{
	BOOL success_flag = FALSE;
	STRPTR filename_s = NULL;
	
	ENTER ();
	
	filename_s = ConcatenateStrings (library_s, suffix_s);
	
	if (filename_s)
		{
			STRPTR file_s = MakeFilename (out_dir_s, filename_s);
			
			if (file_s)
				{
					BPTR out_p = IDOS->FOpen (file_s, MODE_NEWFILE, 0);
		
					if (out_p)
						{
							if (callback_fn (out_p, library_s, capitalized_library_s))
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
					IDOS->Printf ("Failed to create auto init file name for \"%s\" and \"%s\"\n", out_dir_s, filename_s);
				}
			IExec->FreeVec (filename_s);	
		}
	
	LEAVE ();
	
	return success_flag;	
}



STATIC BOOL WriteAutoInitLibraryCode (BPTR out_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR capitalized_library_s)
{
	BOOL success_flag = FALSE;
	
	ENTER ();
	
	
	if (IDOS->FPrintf (out_f, 
		"/*\n"
		"**	$Id$\n"
		"**	Generated by IDLTool 53.6\n"
		"**	Do not edit\n"
		"**\n"
		"**	AutoInit stub for %s\n"
		"**\n"
		"**	Copyright (c) 2010 Hyperion Entertainment CVBA.\n"
		"**	All Rights Reserved.\n"
		"*/\n\n"
		"#include <interfaces/%s.h>\n"
		"#include <proto/exec.h>\n"
		"#include <assert.h>\n\n"
		"/****************************************************************************/\n\n"
		"struct Library * %sBase = NULL;\n"
		"static struct Library * __%sBase;\n\n"
		"/****************************************************************************/\n\n"
		"void %s_base_constructor(void)\n"
		"{\n"
		"\tif (%sBase != NULL)\n"
		"\t\t{\n"
		"\t\t\treturn; /* Someone was quicker, e.g. an interface constructor */\n"
		"\t\t}\n"
		"\t__%sBase = %sBase = (struct Library *)IExec->OpenLibrary(\"%s.library\", 0L);\n"
		"\tassert (%sBase != NULL);\n"
		"}\n"
		"__attribute__((section(\".ctors.zzzz\"))) static void\n"
		"(*%s_base_constructor_ptr)(void) USED = %s_base_constructor;\n\n"		
		"/****************************************************************************/\n\n"
		"void %s_base_destructor (void)\n"
		"{\n"
		"\tif (__%sBase)\n"
		"\t\t{\n"
		"\t\t\tIExec->CloseLibrary((struct Library *) __%sBase);\n"
		"\t\t}\n"
		"}\n"
		"__attribute__((section(\".dtors.zzzz\"))) static void\n"
		"(*%s_base_destructor_ptr)(void) USED = %s_base_destructor;\n\n"	
		"/****************************************************************************/\n\n",
		
		/* header strings */
		library_s, library_s, capitalized_library_s, capitalized_library_s,
		
		/* constructor strings */
		library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, library_s, capitalized_library_s, library_s, library_s,

		/* destructor strings */
		library_s, capitalized_library_s, capitalized_library_s, library_s, library_s) >= 0)
		{
			success_flag = TRUE;	
		}
		
	
	LEAVE ();
	
	return success_flag;
}



STATIC BOOL WriteAutoInitInterfaceCode (BPTR out_f, CONST CONST_STRPTR library_s, CONST CONST_STRPTR capitalized_library_s)
{
	BOOL success_flag = FALSE;
	
	ENTER ();
	
	if (IDOS->FPrintf (out_f, 
		"/*\n"
		"**	$Id$\n"
		"**	Generated by IDLTool 53.6\n"
		"**	Do not edit\n"
		"**\n"
		"**	AutoInit stub for %s\n"
		"**\n"
		"**	Copyright (c) 2010 Hyperion Entertainment CVBA.\n"
		"**	All Rights Reserved.\n"
		"*/\n\n"
		"#include <interfaces/%s.h>\n"
		"#include <proto/exec.h>\n"
		"#include <assert.h>\n\n"
		"/****************************************************************************/\n\n"
		"struct %sIFace * I%s = NULL;\n"
		"static struct Library * __%sBase;\n"
		"static struct %sIFace * __I%s;\n\n"
		"/****************************************************************************/\n\n"
		"extern struct Library * %sBase;\n\n"
		"/****************************************************************************/\n\n"
		"void %s_main_constructor(void)\n"
		"{\n"
		"\tif (%sBase == NULL) /*  Library base is NULL, we need to open it */ \n"
		"\t\t{\n"
		"\t\t\t/* We were called before the base constructor.\n"
    "\t\t\t * This means we will be called _after_ the base destructor.\n"
    "\t\t\t * So we cant drop the interface _after_ closing the last library base,\n"
    "\t\t\t * we just open the library here which ensures that.\n"
    "\t\t\t */\n"
    "\t\t\t__%sBase = %sBase = (struct Library *)IExec->OpenLibrary(\"%s.library\", 0L);\n"
    "\t\t\tassert(%sBase != NULL);\n"
		"\t\t}\n\n"
		"\t__I%s = I%s = (struct %sIFace *)IExec->GetInterface((struct Library *)%sBase, \"main\", 1, NULL);\n"
		"\tassert(I%s != NULL);\n"
		"}\n"
		"__attribute__((section(\".ctors.zzzy\"))) static void\n"
		"(*%s_main_constructor_ptr)(void) USED = %s_main_constructor;\n\n"		
		"/****************************************************************************/\n\n"
		"void %s_main_destructor(void)\n"
		"{\n"
		"\tif (__I%s)\n"
		"\t\t{\n"
		"\t\t\tIExec->DropInterface ((struct Interface *)__I%s);\n"
		"\t\t}\n\n"		
		"\tif (__%sBase)\n"
		"\t\t{\n"
		"\t\t\tIExec->CloseLibrary((struct Library *) __%sBase);\n"
		"\t\t}\n"
		"}\n"
		"__attribute__((section(\".dtors.zzzy\"))) static void\n"
		"(*%s_main_destructor_ptr)(void) USED = %s_main_destructor;\n\n"	
		"/****************************************************************************/\n\n",
		
		/* header strings */
		library_s, library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s,
		
		/* constructor strings */
		library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, library_s, capitalized_library_s, 
		capitalized_library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, library_s, library_s,

		/* destructor strings */
		library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, capitalized_library_s, library_s, library_s) >= 0)
		{
			success_flag = TRUE;	
		}	
	
	LEAVE ();
	
	return success_flag;
}

