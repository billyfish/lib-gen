#include <proto/dos.h>
#include <proto/exec.h>

#include "debugging_utils.h"
#include "utils.h"
#include "init_writer.h"

static BOOL WriteDefintionsTop (BPTR out_p, CONST CONST_STRPTR library_s);

static BOOL WriteDeclarationsTop (BPTR out_p, CONST CONST_STRPTR library_s);

STATIC BOOL WriteLibManagerFunctionDefinitions (BPTR output_f, CONST CONST_STRPTR library_s);

STATIC BOOL WriteLibOpen (BPTR out_f, CONST CONST_STRPTR library_s);

STATIC BOOL WriteLibClose (BPTR out_f, CONST CONST_STRPTR library_s);

STATIC BOOL WriteLibInit (BPTR out_f, CONST CONST_STRPTR library_s);

STATIC BOOL WriteLibExpunge (BPTR out_f, CONST CONST_STRPTR library_s);

STATIC BOOL WriteInitFile (CONST_STRPTR library_s, CONST_STRPTR out_dir_s, CONST_STRPTR file_name_s, BOOL (*callback_fn) (BPTR out_f, CONST CONST_STRPTR library_s));


/***********************/


BOOL WriteInitFiles (CONST_STRPTR library_s, CONST_STRPTR out_dir_s)
{
	BOOL success_flag = FALSE;
	
	ENTER ();

	if (WriteInitFile (library_s, out_dir_s, "init.c", WriteDefintionsTop))
		{
			if (WriteInitFile (library_s, out_dir_s, "init.h", WriteDeclarationsTop))
				{
					success_flag = TRUE;	
				}				
		}
		
	LEAVE ();
	return success_flag;
}

/*************************/



STATIC BOOL WriteInitFile (CONST_STRPTR library_s, CONST_STRPTR out_dir_s, CONST_STRPTR filename_s, BOOL (*callback_fn) (BPTR out_f, CONST CONST_STRPTR library_s))
{
	BOOL success_flag = FALSE;
	STRPTR file_s = NULL;
	
	ENTER ();
	
	file_s = MakeFilename (out_dir_s, filename_s);

	if (file_s)
		{
			BPTR out_p = IDOS->FOpen (file_s, MODE_NEWFILE, 0);

			if (out_p)
				{
					if (callback_fn (out_p, library_s))
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
	
	if (success_flag)
		{
			
		}
	
	
	LEAVE ();
	
	return success_flag;
}


/* ------------------- Library Interface(s) ------------------------ */



static BOOL WriteDefintionsTop (BPTR out_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "/*\n** This file was generated by LibGen and is based upon the\n** output from idltool.\n*/\n\n") >= 0)
		{			
			if (IDOS->FPrintf (out_p, 
				"#include <proto/exec.h>\n\n"
				"#include \"init.h\"\n\n"   
				"#include \"vectors.h\"\n\n"
				"#include \"lib_init.h\"\n\n"
				"/* Uncomment this line (and see below) if your library has a 68k jump table */\n"
				"/* extern APTR VecTable68K []; */\n\n"
				"STATIC CONST struct TagItem %sTags [] =\n"
				"{\n"
				"\t{ MIT_Name, (Tag) \"%s\" },\n"
				"\t{ MIT_VectorTable, (Tag) %s_vectors },\n"
				"\t{ MIT_Version,	1 },\n"
				"\t{ TAG_DONE, 0 }\n"
				"};\n\n", library_s, library_s, library_s) >= 0)
				{
					if (IDOS->FPrintf (out_p, 
						"STATIC CONST CONST_APTR libInterfaces [] =\n"
						"{\n"
						"\tlib_managerTags,\n"
						"\t%sTags,\n"
						"\tNULL\n"
						"};\n\n", library_s) >= 0)
						{
							if (IDOS->FPrintf (out_p,
								"STATIC CONST struct TagItem libCreateTags [] =\n"
								"{\n"
								"\t{ CLT_DataSize, sizeof (struct Library)	},\n"
								"\t{ CLT_InitFunc, (Tag) LibInit },\n"
								"\t{ CLT_Interfaces, (Tag) libInterfaces },\n\n"
								"\t/* Uncomment the following line if you have a 68k jump table */\n"
								"\t/* { CLT_Vector68K, (Tag) VecTable68K }, */\n\n"
								"\t{TAG_DONE, 0 }\n"
								"};\n\n") >= 0)       
								{
									if (IDOS->FPrintf (out_p, 
										"\n/* ------------------- ROM Tag ------------------------ */\n"
										"STATIC CONST struct Resident lib_res USED =\n"
										"{\n"
										"\tRTC_MATCHWORD,\n"
										"\t(struct Resident *) &lib_res,\n"
										"\t(APTR) (&lib_res + 1),\n"
										"\tRTF_NATIVE | RTF_AUTOINIT, /* Add RTF_COLDSTART if you want to be resident */\n"
										"\tVERSION,\n"
										"\tNT_LIBRARY, /* Make this NT_DEVICE if needed */\n"
										"\t0, /* PRI, usually not needed unless you're resident */\n"
										"\t\"\",\n"
										"\tVSTRING,\n"
										"\t(APTR) libCreateTags\n"
										"};\n\n") >= 0)
										{
											STRPTR interface_s = GetInterfaceName (library_s);
											
											if (interface_s)
												{
													
													if (IDOS->FPrintf (out_p,
														"uint32 _%s_Obtain (struct %s *Self)\n"
														"{\n"
														"\t/* Write me. Really, I dare you! */\n"
														"\t((struct ExecIFace *) ((*(struct ExecBase **) 4)->MainInterface))->DebugPrintF (\"Function test::Obtain not implemented\");\n"
														"\n\treturn (uint32) 0;\n"
														"}\n\n"
														"uint32 _%s_Release (struct %s *Self)\n"
														"{\n"
														"\t/* Write me. Really, I dare you! */\n"
														"\t((struct ExecIFace *) ((*(struct ExecBase **) 4)->MainInterface))->DebugPrintF (\"Function test::Release not implemented\");\n"
														"\treturn (uint32) 0;\n"
														"}\n\n", library_s, interface_s, library_s, interface_s) >= 0)            
														{	
															if (WriteLibManagerFunctionDefinitions (out_p, library_s))
																{
																	success_flag = TRUE;
																}
														}
													
													IExec->FreeVec (interface_s);		
												}
										}
                }     
						}
				}
		}

	LEAVE ();
	
	return success_flag;
}
        

static BOOL WriteDeclarationsTop (BPTR out_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "/*\n** This file was generated by LibGen \n*/\n\n") >= 0)
		{			
			STRPTR interface_s = GetInterfaceName (library_s);
			
			if (interface_s)
				{
					
					if (IDOS->FPrintf (out_p,
						"#ifndef INIT_H\n"
						"#define INIT_H\n\n"
						"#include <dos/dos.h>\n\n"
						"#include \"proto/%s.h\"\n\n"
						"/* The Library that we are generating */\n"
						"struct %sBase\n"
						"{\n"
    				"\tstruct Library libNode;\n"
    				"\tBPTR segList;\n\n"
    				"\t/* If you need more data fields, add them here */\n"
						"};\n\n"
						"struct Library *LibOpen (struct LibraryManagerInterface *Self, ULONG version);\n\n"
						"struct Library *LibClose (struct LibraryManagerInterface *Self);\n\n"
						"struct Library *LibInit (struct Library *LibraryBase, APTR seglist, struct Interface *exec);\n\n"
						"struct Library *LibExpunge (struct LibraryManagerInterface *Self);\n\n"
						"uint32 _%s_Obtain (struct %s *Self);\n\n"
						"uint32 _%s_Release (struct %s *Self);\n\n"
						"#endif\n",
						library_s, library_s, library_s, interface_s, library_s, interface_s) >= 0)            
						{	
							success_flag = TRUE;
						}
					
					IExec->FreeVec (interface_s);		
				}
		}


	LEAVE ();
	
	return success_flag;
}


STATIC BOOL WriteLibManagerFunctionDefinitions (BPTR output_f, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (WriteLibOpen (output_f, library_s))
		{
			if (WriteLibClose (output_f, library_s))
				{
					if (WriteLibInit (output_f, library_s))
						{
							if (WriteLibExpunge (output_f, library_s))
								{
									success_flag = TRUE;
								}			
						}			
				}			
		}

	
	LEAVE ();
	
	return success_flag;
}


STATIC BOOL WriteLibOpen (BPTR out_f, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_f,
		"/* Open the library */\n"
		"struct Library *LibOpen (struct LibraryManagerInterface *Self, ULONG version)\n"
		"{\n"
		"\tstruct %sBase *lib_base_p = (struct %sBase *) Self->Data.LibBase;\n\n"
		"\tif (version > VERSION)\n"
		"\t\t{\n"
		"\t\t\treturn NULL;\n"
		"\t\t}\n"
		"\t/*\n" 
		"\t * Add any specific open code here\n"
		"\t * Return 0 before incrementing OpenCnt to fail opening\n"
		"\t	*/\n\n"
		"\t/* Add up the open count */\n"
		"\tlib_base_p -> libNode.lib_OpenCnt ++;\n"
		"\treturn ((struct Library *) lib_base_p);\n"
		"}\n\n",
		library_s, library_s) >= 0)            
		{	
			success_flag = TRUE;
		}

	LEAVE ();
	
	return success_flag;
}


STATIC BOOL WriteLibClose (BPTR out_f, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_f,
		"/* Close the library */\n"
		"struct Library *LibClose (struct LibraryManagerInterface *Self)\n"
		"{\n"
		"\tstruct %sBase *lib_base_p = (struct %sBase *) Self->Data.LibBase;\n\n"
		"\t/*\n" 
		"\t * Add any specific close code here\n"
		"\t */\n\n"
		"\t/* decrement up the open count */\n"
		"\tlib_base_p -> libNode.lib_OpenCnt --;\n"
		"\treturn 0;\n"
		"}\n\n",
		library_s, library_s) >= 0)            
		{	
			success_flag = TRUE;
		}

	LEAVE ();
	
	return success_flag;
}


STATIC BOOL WriteLibInit (BPTR out_f, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_f,
		"/* The ROMTAG Init Function */\n"
		"struct Library *LibInit(struct Library *LibraryBase, APTR seglist, struct Interface *exec)\n"
		"{\n"
    "\tstruct ExecIFace *IExec UNUSED = (struct ExecIFace *) exec;\n"
		"\tstruct %sBase *lib_base_p = (struct %sBase *) LibraryBase;\n\n"
		"\tlib_base_p -> libNode.lib_Node.ln_Type = NT_LIBRARY;\n"
    "\tlib_base_p -> libNode.lib_Node.ln_Pri  = 0;\n"
    "\tlib_base_p -> libNode.lib_Node.ln_Name = \"%s.library\";\n"
    "\tlib_base_p -> libNode.lib_Flags        = LIBF_SUMUSED | LIBF_CHANGED;\n"
    "\tlib_base_p -> libNode.lib_Version      = VERSION;\n"
    "\tlib_base_p -> libNode.lib_Revision     = REVISION;\n"
    "\tlib_base_p -> libNode.lib_IdString     = VSTRING;\n\n"
    "\tlib_base_p -> segList = (BPTR) seglist;\n\n"
    "\t/* Add any additional code here */\n"
		"\treturn (struct Library *) lib_base_p;\n"
		"}\n\n",
		library_s, library_s, library_s) >= 0)            
		{	
			success_flag = TRUE;
		}

	LEAVE ();
	
	return success_flag;	
}


STATIC BOOL WriteLibExpunge (BPTR out_f, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_f,
		"/* Expunge the library */\n"
		"struct Library *LibExpunge (struct LibraryManagerInterface *Self)\n"
		"{\n"
		"\t/* If your library cannot be expunged, return 0 */\n"
    "\tstruct ExecIFace *IExec = (struct ExecIFace *)(* (struct ExecBase **)4) -> MainInterface;\n"
    "\tAPTR result_p = (APTR) 0;\n"
		"\tstruct %sBase *lib_base_p = (struct %sBase *) Self->Data.LibBase;\n\n"
		"\tif (lib_base_p -> libNode.lib_OpenCnt != 0)\n"
		"\t\t{\n"
		"\t\t\tlib_base_p -> libNode.lib_Flags |= LIBF_DELEXP;\n"
		"\t\t}\n"
		"\telse\n"
		"\t\t{\n"
		"\t\t\tresult_p = (APTR) lib_base_p -> segList;\n"
		"\t\t\t/* Undo the code in LibInit */\n\n"
		"\t\t\tIExec->Remove ((struct Node *) lib_base_p);\n"
    "\t\t\tIExec->DeleteLibrary ((struct Library *) lib_base_p);\n"
		"\t\t}\n"
		"\treturn result_p;\n"
		"}\n\n",
		library_s, library_s) >= 0)            
		{	
			success_flag = TRUE;
		}

	LEAVE ();
	
	return success_flag;	
}

