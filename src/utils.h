#ifndef UTILS_H
#define UTILS_H

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/lists.h>

enum Verbosity 
{
	VB_QUIET,
	VB_NORMAL,
	VB_LOUD,
	VB_LOUDER
};

#ifdef ALLOCATE_GLOBALS
	#define PREFIX 
	#define VAL(x)	= x
#else
	#define PREFIX extern
	#define VAL(x)
#endif


PREFIX const char *SOURCE_DIR_SUFFIX_S VAL("_source"); 


#ifdef __cplusplus
extern "C" {
#endif

char *CopyToNewString (const char *start_p, const char *end_p, const BOOL trim_flag);

STRPTR ConcatenateStrings (CONST_STRPTR first_s, CONST_STRPTR second_s);

STRPTR MakeFilename (CONST_STRPTR first_s, CONST_STRPTR second_s);

STRPTR GetParentName (CONST_STRPTR filename_s);

int32 ScanPath (CONST_STRPTR path_s, struct List *matching_files_list_p, CONST_STRPTR filename_pattern_s, const BOOL recurse_flag, struct List *paths_to_ignore_p);

BOOL AddFullFilenameToList (struct List *header_definitions_p, CONST_STRPTR dir_s, CONST_STRPTR name_s, struct List *paths_to_ignore_p);

void SetVerbosity (enum Verbosity v);

enum Verbosity GetVerbosity (void);

void SetNewlibNeeded (const BOOL value);

BOOL IsNewlibNeeded (void);

STRPTR GetInterfaceName (CONST CONST_STRPTR library_s);

STRPTR GetGlobalInterfaceName (CONST CONST_STRPTR library_s);

BOOL EnsureDirectoryExists (CONST_STRPTR dir_s);

STRPTR GetSourceFilename (CONST CONST_STRPTR library_s, CONST CONST_STRPTR header_filename_s, char file_suffix);

BOOL CopyFile (CONST CONST_STRPTR src_s, CONST CONST_STRPTR dest_s);

BOOL PrintUpperCase (BPTR out_p, CONST_STRPTR value_s);

BOOL PrintCaptitalizedString (BPTR out_p, CONST_STRPTR value_s);

STRPTR GetUpperCaseString (CONST_STRPTR src_s);

STRPTR GetCapitalizedString (CONST_STRPTR src_s);


STRPTR ScrollPastWhitespace (STRPTR data_s);		

STRPTR CreateRegEx (CONST_STRPTR pattern_s, BOOL capture_flag);

void ReplaceChars (STRPTR value_s, CONST CONST_STRPTR old_values_s, const char new_value);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef UTILS_H */
