/*

************************************************************
**
** Created by: CodeBench 0.42 (14.10.2013)
**
** Project: libgen
**
** File: interface_h_writer.c
**
** Date: 09-11-2016 11:08:13
**
************************************************************

*/

#include <ctype.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "interface_h_writer.h"
#include "utils.h"
#include "debugging_utils.h"
#include "function_definition.h"

/*
 * STATIC DECLARATIONS
 */

static BOOL WriteHeaderTop (BPTR out_p, CONST CONST_STRPTR upper_case_library_s);

static BOOL WritePreInterface (BPTR out_p);

static BOOL WriteHeaderBottom (BPTR out_p, CONST CONST_STRPTR upper_case_library_s);

static BOOL WriteInterface (BPTR out_p, struct List *function_defs_p, CONST CONST_STRPTR library_s);

/*
 * API DEFINITIONS
 */

BOOL WriteInterfaceHeader (struct List *function_defs_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s)
{
	BOOL success_flag = FALSE;
	STRPTR dir_s = NULL;
	
	ENTER ();
	
	dir_s = MakeFilename (output_dir_s, "interfaces");

	if (dir_s)
		{
			if (EnsureDirectoryExists (dir_s))
				{
					STRPTR header_s = ConcatenateStrings (library_s, ".h");
					
					if (header_s)
						{
							STRPTR full_path_to_header_s = MakeFilename (dir_s, header_s);
							
							if (full_path_to_header_s)
								{
									BPTR out_p = IDOS->FOpen (full_path_to_header_s, MODE_NEWFILE, 0);
									
									if (out_p)
										{
											STRPTR upper_case_library_s = GetUpperCaseString (library_s);
			
											if (upper_case_library_s)
												{
													if (WriteHeaderTop (out_p, upper_case_library_s))
														{
															/* Write library includes */
															
															if (WritePreInterface (out_p))
																{
																	if (WriteInterface (out_p, function_defs_p, library_s))
																		{
																			
																	
																			if (WriteHeaderBottom (out_p, upper_case_library_s))
																				{
																					success_flag = TRUE;
																				}
																		}					
																}
														}
														
													IExec->FreeVec (upper_case_library_s);
												}		/* if (upper_case_library_s) */		
											
											if (IDOS->FClose (out_p) == 0)
												{
													
												}
										}
									
									IExec->FreeVec (full_path_to_header_s);
								}
							
							IExec->FreeVec (header_s);
						}					 
					
				}		/* if (EnsureDirectoryExists (dir_s)) */
			
			IExec->FreeVec (dir_s);
		}		/* if (dir_s) */
		
	LEAVE ();
	
	return success_flag;	
}


/* 
 * STATIC DEFINITIONS
 */

static BOOL WriteHeaderTop (BPTR out_p, CONST CONST_STRPTR upper_case_library_s)
{
	BOOL success_flag = FALSE;
	
	ENTER ();
	
	if (IDOS->FPrintf (out_p, "/*\n** This file was generated by LibGen and is based upon the\n** output from idltool.\n/*\n\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "#ifndef %s_INTERFACE_DEF_H\n#define %s_INTERFACE_DEF_H\n\n", upper_case_library_s, upper_case_library_s) >= 0)
				{
					if (IDOS->FPrintf (out_p, "/*\n"
						"** This file was machine generated by idltool 53.1.\n"
						"** Do not edit\n"
						"*/\n\n"
						"#include <exec/types.h>\n"
						"#include <exec/exec.h>\n"
						"#include <exec/interfaces.h>\n\n") >= 0)
						{
							success_flag = TRUE;
						}
				}
		}

	LEAVE ();
	
	return success_flag;
}


/*

struct TestIFace
{
	struct InterfaceData Data;

	uint32 APICALL (*Obtain)(struct TestIFace *Self);
	uint32 APICALL (*Release)(struct TestIFace *Self);
	void APICALL (*Expunge)(struct TestIFace *Self);
	struct Interface * APICALL (*Clone)(struct TestIFace *Self);
	BOOLEAN APICALL (*InitialiseLoaderData)(struct TestIFace *Self, LoaderModuleData * data_p, const HashTable * const config_p);
	BOOLEAN APICALL (*InitialiseLoaderData)(struct TestIFace *Self, LoaderModuleData * data_p, const HashTable * const config_p);
	BOOLEAN APICALL (*InitialiseLoaderData)(struct TestIFace *Self, LoaderModuleData * data_p, const HashTable * const config_p);
	void APICALL (*ClearLoaderData)(struct TestIFace *Self, LoaderModuleData * data_p);
	void APICALL (*ClearLoaderData)(struct TestIFace *Self, LoaderModuleData * data_p);
	void APICALL (*ClearLoaderData)(struct TestIFace *Self, LoaderModuleData * data_p);
	Trajectory * APICALL (*LoadTrajectory)(struct TestIFace *Self, LoaderModule * loader_p, const char * const path, const double64 first_frame_time, const double64 chunk_time, const uint32 num_frames_to_skip, EventListener * events_listener_p);
	Trajectory * APICALL (*LoadTrajectory)(struct TestIFace *Self, LoaderModule * loader_p, const char * const path, const double64 first_frame_time, const double64 chunk_time, const uint32 num_frames_to_skip, EventListener * events_listener_p);
	Trajectory * APICALL (*LoadTrajectory)(struct TestIFace *Self, LoaderModule * loader_p, const char * const path, const double64 first_frame_time, const double64 chunk_time, const uint32 num_frames_to_skip, EventListener * events_listener_p);
};

*/
static BOOL WriteInterface (BPTR out_p, struct List *function_defs_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;
	STRPTR interface_s = NULL;
	
	ENTER ();
	
	interface_s = ConcatenateStrings (library_s, "IFace");
	
	if (interface_s)
		{
			/* Captialize the interface */
			*interface_s = toupper (*interface_s);
			
			if (IDOS->FPrintf (out_p, "struct %s\n{\n\tstruct InterfaceData Data;\n\n", interface_s) >= 0)
				{
					if (IDOS->FPrintf (out_p, "\tuint32 APICALL (*Obtain) (struct %s *Self)\n"
						"\tuint32 APICALL (*Release) (struct TestIFace *Self);\n"
						"\tvoid APICALL (*Expunge) (struct TestIFace *Self);\n"
						"\tstruct Interface * APICALL (*Clone) (struct TestIFace *Self);\n\n", 
						interface_s, interface_s, interface_s, interface_s) >= 0)
						{
							if (WriteAllInterfaceFunctionDefinitions (function_defs_p, out_p, interface_s))
								{
									if (IDOS->FPrintf (out_p, "};\n\n") >= 0)
										{
											success_flag = TRUE;
										}										
								}							
						}
				}
			
			IExec->FreeVec (interface_s);	
		}
	
	
	LEAVE ();
	
	return success_flag;
}


static BOOL WritePreInterface (BPTR out_p)
{
	BOOL success_flag = FALSE;
	
	ENTER ();
	
	if (IDOS->FPrintf (out_p, "#ifdef __cplusplus\n"
		"#ifdef __USE_AMIGAOS_NAMESPACE__\n"
		"namespace AmigaOS {\n"
		"#endif\n"
		"extern \"C\" {\n"
		"#endif\n\n\n") >= 0)
		{
			success_flag = TRUE;
		}

	LEAVE ();
	
	return success_flag;
}


static BOOL WriteHeaderBottom (BPTR out_p, CONST CONST_STRPTR upper_case_library_s)
{

	BOOL success_flag = FALSE;
	
	ENTER ();
	
	if (IDOS->FPrintf (out_p, "\n\n#ifdef __cplusplus\n"
		"}\n"
		"#ifdef __USE_AMIGAOS_NAMESPACE__\n"
		"}\n"
		"#endif\n"
		"#endif\n\n\n"
		"#endif /* %s_INTERFACE_DEF_H */\n", upper_case_library_s) >= 0)
		{
			success_flag = TRUE;
		}

	LEAVE ();
	
	return success_flag;
}

