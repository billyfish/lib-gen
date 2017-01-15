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

typedef struct LibGenPrefs
{
	CONST_STRPTR lgp_header_input_s;
	CONST_STRPTR lgp_source_input_dir_s;	
	CONST_STRPTR lgp_library_s;
	struct List *lgp_paths_to_ignore_p;
	STRPTR lgp_prototype_prefix_s;
	CONST_STRPTR lgp_header_filename_regexp_s;
	CONST_STRPTR lgp_source_filename_regexp_s;
	CONST_STRPTR lgp_defs_filename_s;
	struct List *lgp_functions_to_ignore_p;
	STRPTR lgp_prototype_regexp_s;
	int32 lgp_version;
	enum InterfaceFlag lgp_flag;
	BOOL lgp_recurse_flag;
	BOOL lgp_generate_code_flag;	
	BOOL lgp_newlib_flag;
} LibGenPrefs;




BOOL InitLibGenPrefs (LibGenPrefs *prefs_p, CONST_STRPTR prototype_pattern_s, CONST_STRPTR header_filename_pattern_s, CONST_STRPTR source_filename_pattern_s, CONST_STRPTR functions_to_ignore_filename_s);


void ClearLibGenPrefs (LibGenPrefs *prefs_p);


BOOL ArePrefsValid (const LibGenPrefs *prefs_p);



#endif

