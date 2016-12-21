#include <ctype.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "inline_header_writer.h"
#include "debugging_utils.h"
#include "function_definition.h"
#include "utils.h"


static BOOL WriteHeader (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR upper_case_library_s);

static BOOL WriteFooter (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR upper_case_library_s, struct List *function_definitions_list_p);



BOOL WriteInlineHeader (struct List *function_definitions_list_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s)
{
	BOOL success_flag = FALSE;
	STRPTR dir_s;

	ENTER ();
	
	dir_s = MakeFilename (output_dir_s, "inline4");

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
											
													if (WriteHeader (out_p, library_s, upper_case_library_s))
														{
															if (WriteFunctionDefinitionListIncludes (out_p, function_definitions_list_p, "#include \"", "\"\n"))
																{
																	if (WriteFooter (out_p, library_s, upper_case_library_s, function_definitions_list_p))
																		{
																			success_flag = TRUE;
																		}
																}
														}													
													
													IExec->FreeVec (upper_case_library_s);
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


static BOOL WriteHeader (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR upper_case_library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "/*\n** This file was generated by LibGen and is based upon the\n** output from idltool.\n*/\n\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "#ifndef INLINE4_%s_H\n", upper_case_library_s) >= 0)
				{
					if (IDOS->FPrintf (out_p, "#define INLINE4_%s_H\n", upper_case_library_s) >= 0)
						{
							if (IDOS->FPrintf (out_p, "\n\n/*\n"
									"** This file was auto generated by idltool 53.1.\n"
									"**\n"
									"** It provides compatibility to OS3 style library\n"
									"** calls by substituting functions.\n"
									"**\n"
									"** Do not edit manually.\n"
									"*/\n\n"
									"#include <exec/types.h>\n"
									"#include <exec/exec.h>\n"
									"#include <exec/interfaces.h>\n\n") >= 0)
								{
									success_flag = TRUE;
								}
											
						}		/* if (PrintUpperCase (out_p, library_s)) */
						
				}		/* .f (IDOS->FPrintf (out_p, "#ifndef INLINE4_") >= 0) */
		}	
	
	LEAVE ();

	return success_flag;
}


static BOOL WriteFooter (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR upper_case_library_s, struct List *fn_defs_p)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "\n#include \"interfaces/%s.h\"\n\n", library_s) >= 0)
		{
			if (IDOS->FPrintf (out_p, "\n/* Inline macros for interface \"main\". */\n\n") >= 0)
				{
					STRPTR interface_name_s = GetGlobalInterfaceName (library_s);
					
					if (interface_name_s)
						{
							if (WriteInlineMacrosForAllFunctionDeclarations (fn_defs_p, out_p, interface_name_s))
								{
									if (IDOS->FPrintf (out_p, "\n\n#endif /* INLINE4_%s_H */\n", upper_case_library_s) >= 0)
										{
											success_flag = TRUE;
										}
								}
													
						}
					

				}		/* if (PrintUpperCase (out_p, library_s)) */
				
		}		/* .f (IDOS->FPrintf (out_p, "#ifndef INLINE4_") >= 0) */
	
	
	LEAVE ();

	return success_flag;
}


