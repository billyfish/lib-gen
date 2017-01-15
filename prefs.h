/*

************************************************************
**
** Created by: CodeBench 0.42 (14.10.2013)
**
** Project: libgen
**
** File: prefs.h
**
** Date: 15-01-2017 13:46:12
**
************************************************************

*/

#ifndef LIBGEN_PREFS_H
#define LIBGEN_PREFS_H

#include <exec/types.h>

struct LibGenPrefs
{
	CONST_STRPTR lgp_header_input_dir_s;
	CONST_STRPTR lgp_source_input_dir_s;	
	CONST_STRPTR lgp_library_s;
	CONST_STRPTR lgp_paths_to_ignore_s;
	STRPTR lgp_prototype_prefix_s;
	CONST_STRPTR lgp_header_filename_pattern_s;
	CONST_STRPTR lgp_source_filename_pattern_s;
	CONST_STRPTR lgp_defs_filename_s;
	CONST_STRPTR lgp_functions_to_ignore_filename_s;
	STRPTR lgp_prototype_pattern_s;
	int32 lgp_version;
	enum InterfaceFlag lgp_flag;
	BOOL lgp_recurse_flag;
	BOOL lgp_generate_code_flag;	
	BOOL lgp_newlib_flag;
};




BOOL InitLibGenPrefs (struct LibGenPrefs *prefs_p);

void ClearLibGenPrefs (struct LibGenPrefs *prefs_p);



#endif

