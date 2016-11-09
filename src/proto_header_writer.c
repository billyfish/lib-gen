#include <proto/dos.h>

#include "proto_header_writer.h"
#include "debugging_utils.h"
#include "utils.h"



static BOOL WriteProtoTop (BPTR out_p, CONST CONST_STRPTR library_s);

static BOOL WriteProtoBottom (BPTR out_p, CONST CONST_STRPTR library_s);



BOOL WriteProtoHeader (CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s)
{
	BOOL success_flag = FALSE;
	STRPTR dir_s;
	
	ENTER ();
	
	dir_s = MakeFilename (output_dir_s, "proto");

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
											if (WriteProtoTop (out_p, library_s))
												{
													if (WriteProtoBottom (out_p, library_s))
														{
															success_flag = TRUE;	
														}
												}
												
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


static BOOL WriteProtoTop (BPTR out_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "/*\n** This file was generated by LibGen and is based upon the\n** output from idltool.\n/*\n\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "#ifndef PROTO_") >= 0)
				{
					if (PrintUpperCase (out_p, library_s))
						{
							if (IDOS->FPrintf (out_p, "_H\n#define PROTO_") >= 0)
								{
									if (PrintUpperCase (out_p, library_s))
										{
											if (IDOS->FPrintf (out_p, "_H\n\n/*\n"
												"**	$Id$\n"
												"**	Includes Release 50.1\n" 
												"**\n" 
												"**	Prototype/inline/pragma header file combo\n" 
												"**\n" 
												"**	Copyright (c) 2010 Hyperion Entertainment CVBA.\n" 
												"**	All Rights Reserved.\n" 
												"*/\n" 
												"\n" 
												"/****************************************************************************/\n" 
												"\n" 
												"#ifndef __NOLIBBASE__\n" 
												" #ifndef __USE_BASETYPE__\n" 
												"  extern struct Library *") >= 0)
												{
													if (PrintCaptitalizedString (out_p, library_s))
														{
															if (IDOS->FPrintf (out_p, "Base;\n #else\n  extern struct Library *") >= 0)
																{
																	if (PrintCaptitalizedString (out_p, library_s))
																		{																			
																			if (IDOS->FPrintf (out_p, "Base;\n #endif /* __USE_BASETYPE__ */\n#endif /* __NOLIBBASE__ */\n\n"
																				"/****************************************************************************/\n\n") >= 0)
																				{
																					success_flag = TRUE;
																				}
																		}
																}
														}
												}
										}		
								}
						}
				}
		}
		
	LEAVE ();
	
	return success_flag;
}


static BOOL WriteProtoBottom (BPTR out_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;
	STRPTR upper_case_library_s = NULL;
	
	ENTER ();
	
	
	upper_case_library_s = GetUpperCaseString (library_s);
	
	if (upper_case_library_s)
		{
			if (IDOS->FPrintf (out_p, "#ifdef __amigaos4__\n" \
				" #include <interfaces/%s.h>\n" \
				" #ifdef __USE_INLINE__\n" \
				"  #include <inline4/%s.h>\n" \
				" #endif /* __USE_INLINE__ */\n" \
				" #ifndef CLIB_%s_PROTOS_H\n" \
				"  #define CLIB_%s_PROTOS_H 1\n" \
				" #endif /* CLIB_%s_PROTOS_H */\n" \
				" #ifndef __NOGLOBALIFACE__\n" \
				" #endif /* __NOGLOBALIFACE__ */\n" \
				"#else /* __amigaos4__ */\n" \
				" #ifndef CLIB_%s_PROTOS_H\n" \
				"  #include <clib/%s_protos.h>\n" \
				" #endif /* CLIB_%s_PROTOS_H */\n" \
				" #if defined(__GNUC__)\n" \
				"  #ifndef __PPC__\n" \
				"   #include <inline/%s.h>\n" \
				"  #else\n" \
				"   #include <ppcinline/%s.h>\n" \
				"  #endif /* __PPC__ */\n" \
				" #elif defined(__VBCC__)\n" \
				"  #ifndef __PPC__\n" \
				"   #include <inline/%s_protos.h>\n" \
				"  #endif /* __PPC__ */\n" \
				" #else\n" \
				"  #include <pragmas/%s_pragmas.h>\n" \
				" #endif /* __GNUC__ */\n" \
				"#endif /* __amigaos4__ */\n" \
				"\n" \
				"/****************************************************************************/\n" \
				"\n" \
				"#endif /* PROTO_%s_H */\n", library_s, library_s, upper_case_library_s, upper_case_library_s, upper_case_library_s,
				upper_case_library_s, library_s, upper_case_library_s, library_s, library_s, library_s, library_s, upper_case_library_s) >= 0)
				{
					success_flag = TRUE;
				}
			
			IExec->FreeVec (upper_case_library_s);	
		}
		
	LEAVE ();
	
	return success_flag;
}




