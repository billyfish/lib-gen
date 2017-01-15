/*

************************************************************
**
** Created by: CodeBench 0.42 (14.10.2013)
**
** Project: libgen
**
** File: prefs.c
**
** Date: 15-01-2017 13:46:03
**
************************************************************

*/

#include <proto/exec.h>

#include "prefs.h"



static struct List *GetFunctionsToIgnore (CONST_STRPTR filename_s);





BOOL InitLibGenPrefs (LibGenPrefs *prefs_p, CONST_STRPTR prototype_pattern_s, CONST_STRPTR header_filename_pattern_s, CONST_STRPTR source_filename_pattern_s, CONST_STRPTR functions_to_ignore_filename_s)
{
	BOOL success_flag = TRUE;
	
	prefs_p -> lgp_header_input_s = NULL;
	prefs_p -> lgp_source_input_dir_s = NULL;	
	prefs_p -> lgp_library_s = NULL;
	prefs_p -> lgp_paths_to_ignore_s = NULL;
	prefs_p -> lgp_prototype_prefix_s = NULL;
	prefs_p -> lgp_header_filename_regexp_s = NULL;
	prefs_p -> lgp_source_filename_regexp_s = NULL;
	prefs_p -> lgp_defs_filename_s = NULL;
	prefs_p -> lgp_functions_to_ignore_p = NULL;
	prefs_p -> lgp_prototype_regexp_s = NULL;
	prefs_p -> lgp_version = 1;
	prefs_p -> lgp_visibility_flag = IF_PUBLIC;
	prefs_p -> lgp_recurse_flag = FALSE;
	prefs_p -> lgp_generate_code_flag = FALSE;	
	prefs_p -> lgp_newlib_flag = FALSE;

	if (prototype_pattern_s)
		{
			prefs_p -> lgp_prototype_regexp_s = CreateRegEx (prototype_pattern_s, TRUE);

			if (! (prefs_p -> lgp_prototype_regexp_s))
				{
					success_flag = FALSE;
				}
		}

	if (success_flag && source_filename_pattern_s)
		{
			prefs_p -> lgp_source_filename_regexp_s = CreateRegEx (source_filename_pattern_s, TRUE);

			if (! (prefs_p -> lgp_source_filename_regexp_s))
				{
					success_flag = FALSE;
				}
		}

	if (success_flag && header_filename_pattern_s)
		{
			prefs_p -> lgp_header_filename_regexp_s = CreateRegEx (header_filename_pattern_s, TRUE);

			if (! (prefs_p -> lgp_header_filename_regexp_s))
				{
					success_flag = FALSE;
				}
		}

	if (success_flag && functions_to_ignore_filename_s)
		{
			prefs_p -> lgp_functions_to_ignore_p = GetFunctionsToIgnore (functions_to_ignore_filename_s);
			
			if (! (prefs_p -> lgp_functions_to_ignore_p))
				{
					
					success_flag = FALSE;
				}
		}


									
	if (success_flag && paths_to_ignore_s)
		{
			prefs_p -> lgp_paths_to_ignore_p = ParsePaths (input_dir_s, paths_to_ignore_s);
			
			if (! (prefs_p -> lgp_paths_to_ignore_p))
				{
					success_flag = FALSE;	
				}
		}


	return success_flag;
}


void ClearLibGenPrefs (LibGenPrefs *prefs_p)
{
	if (prefs_p -> lgp_prototype_regexp_s)
		{
			IExec->FreeVec (prefs_p -> lgp_prototype_regexp_s);
			prefs_p -> lgp_prototype_regexp_s = NULL;
		}

	if (prefs_p -> lgp_source_filename_regexp_s)
		{
			IExec->FreeVec (prefs_p -> lgp_source_filename_regexp_s);
			prefs_p -> lgp_source_filename_regexp_s = NULL;
		}
		
	if (prefs_p -> lgp_header_filename_regexp_s)
		{
			IExec->FreeVec (prefs_p -> lgp_header_filename_regexp_s);
			prefs_p -> lgp_header_filename_regexp_s = NULL;
		}

	if (prefs_p -> lgp_functions_to_ignore_p)
		{
			FreeList (prefs_p -> lgp_functions_to_ignore_p);
			prefs_p -> lgp_functions_to_ignore_p = NULL;
		}
	
}



BOOL ArePrefsValid (const LibGenPrefs *prefs_p)
{
	return ((prefs_p -> lgp_input_s) && 
		(prefs_p -> lgp_header_filename_pattern_s) && 
		(prefs_p -> lgp_library_s) && 
		(prefs_p -> lgp_prototype_pattern_s));
}




/*
 * STATIC DEFINITIONS
 */


static struct List *GetFunctionsToIgnore (CONST_STRPTR filename_s)
{
	struct List *names_p = NULL;	
	BPTR input_f = ZERO;
	
	ENTER ();
	
	if ((input_f = IDOS->FOpen (filename_s, MODE_OLDFILE, 0)) != ZERO)
		{
			names_p = IExec->AllocSysObjectTags (ASOT_LIST, TAG_DONE);
	
			if (names_p)
				{					
					char line_s [1024];
										
					while (IDOS->FGets (input_f, line_s, 1024))
						{
							char buffer_s [1024];
							char *end_p = buffer_s;
							
							while ((*end_p != '\0') && (sscanf (line_s, "%s", end_p) == 1))
								{
									STRPTR name_s = NULL;
																		
									end_p = buffer_s + strlen (buffer_s);
						
									name_s = CopyToNewString (buffer_s, end_p, FALSE);									

									while ((*end_p != '\0') && (isspace (*end_p)))
										{
											++ end_p;	
										}
									
									if (name_s)
										{
											IDOS->Printf ("ignoring \"%s\"\n", name_s);
											
											struct Node *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
												ASONODE_Name, name_s,
												ASONODE_Size, sizeof (struct Node),
												TAG_DONE);
							
											if (node_p)
												{
													IExec->AddTail (names_p, node_p);
												}						
											else
												{
													DB (KPRINTF ("%s %ld - Failed to allocate node for \"%s\"\n", __FILE__, __LINE__, name_s));
													IExec->FreeVec (name_s);							
												}										
											
										}									
									
								}
						}
				}		
				
			IDOS->FClose (input_f);	
		}	
	

	LEAVE ();
	
	return names_p;
}

