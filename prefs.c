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






BOOL InitLibGenPrefs (struct LibGenPrefs *prefs_p, CONST_STRPTR prototype_pattern_s, CONST_STRPTR header_filename_pattern_s, CONST_STRPTR source_filename_pattern_s)
{
	BOOL success_flag = TRUE;
	
	prefs_p -> lgp_header_input_dir_s = NULL;
	prefs_p -> lgp_source_input_dir_s = NULL;	
	prefs_p -> lgp_library_s = NULL;
	prefs_p -> lgp_paths_to_ignore_s = NULL;
	prefs_p -> lgp_prototype_prefix_s = NULL;
	prefs_p -> lgp_header_filename_regexp_s = NULL;
	prefs_p -> lgp_source_filename_regexp_s = NULL;
	prefs_p -> lgp_defs_filename_s = NULL;
	prefs_p -> lgp_functions_to_ignore_filename_s = NULL;
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

	return success_flag;
}


void ClearLibGenPrefs (struct LibGenPrefs *prefs_p)
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
	
}

