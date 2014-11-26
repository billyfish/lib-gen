#ifndef UTILS_H
#define UTILS_H

#include <dos/dos.h>
#include <exec/types.h>
#include <exec/lists.h>

enum Verbosity 
{
	VB_QUIET,
	VB_NORMAL,
	VB_LOUD
};


#ifdef __cplusplus
extern "C" {
#endif

char *CopyToNewString (const char *start_p, const char *end_p, const BOOL trim_flag);

STRPTR ConcatenateStrings (CONST_STRPTR first_s, CONST_STRPTR second_s);

STRPTR MakeFilename (CONST_STRPTR first_s, CONST_STRPTR second_s);

int32 ScanDirectories (CONST_STRPTR name_s, struct List *matching_files_list_p, CONST_STRPTR filename_pattern_s, const BOOL recurse_flag);

BOOL AddFullHeaderPathToList (struct List *header_definitions_p, CONST_STRPTR dir_s, CONST_STRPTR name_s);

void SetVerbosity (enum Verbosity v);

enum Verbosity GetVerbosity (void);

BPTR GetMakefileHandle (CONST_STRPTR library_s);

BOOL EnsureDirectoryExists (CONST_STRPTR dir_s);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef UTILS_H */
