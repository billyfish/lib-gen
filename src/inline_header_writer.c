#include <ctype.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "inline_header_writer.h"
#include "debugging_utils.h"
#include "function_definition.h"
#include "utils.h"



static BOOL WriteInlineIncludes (BPTR out_p, struct List *function_definitions_list_p);

static BOOL WriteHeader (BPTR out_p, CONST CONST_STRPTR library_s);

static BOOL WriteFooter (BPTR out_p, CONST CONST_STRPTR library_s);



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
											if (WriteHeader (out_p, library_s))
												{
													if (WriteInlineIncludes (out_p, function_definitions_list_p))
														{
															if (WriteFooter (out_p, library_s))
																{
																	success_flag = TRUE;
																}
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


static BOOL WriteHeader (BPTR out_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "/*\n** This file was generated by LibGen and is based upon the\n** output from idltool.\n*/\n\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "#ifndef INLINE4_") >= 0)
				{
					if (PrintUpperCase (out_p, library_s))
						{
							if (IDOS->FPrintf (out_p, "_H\n#define INLINE4_") >= 0)
								{
									if (PrintUpperCase (out_p, library_s))
										{
											if (IDOS->FPrintf (out_p, "_H\n\n"
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
											
										}
								}
								
						}		/* if (PrintUpperCase (out_p, library_s)) */
						
				}		/* .f (IDOS->FPrintf (out_p, "#ifndef INLINE4_") >= 0) */
		}	
	
	LEAVE ();

	return success_flag;
}


static BOOL WriteFooter (BPTR out_p, CONST CONST_STRPTR library_s)
{
	BOOL success_flag = FALSE;

	ENTER ();
	
	if (IDOS->FPrintf (out_p, "\n#include \"interfaces/%s.h\"\n\n", library_s) >= 0)
		{
			if (IDOS->FPrintf (out_p, "/* No inline4 macros are defined here. */\n\n"
					"#endif /* INLINE4_") >= 0)
				{
					if (PrintUpperCase (out_p, library_s))
						{
							if (IDOS->FPrintf (out_p, "_H */\n") >= 0)
								{
									success_flag = TRUE;
								}
						}
						
				}		/* if (PrintUpperCase (out_p, library_s)) */
				
		}		/* .f (IDOS->FPrintf (out_p, "#ifndef INLINE4_") >= 0) */
	
	
	LEAVE ();

	return success_flag;
}


static BOOL WriteInlineIncludes (BPTR out_p, struct List *function_definitions_list_p)
{
	ENTER ();

	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_definitions_list_p);

	while ((node_p != NULL) && success_flag)
		{
			if (IDOS->FPrintf (out_p, "#include \"%s\"\n", node_p -> fdn_function_def_p -> fd_filename_s) >= 0)
				{
					node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
				}
			else
				{
					success_flag = FALSE;
				}
		}

	LEAVE ();
	return success_flag;
}

