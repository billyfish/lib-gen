#ifndef UTILS_H
#define UTILS_H

#include <exec/types.h>
#include <exec/lists.h>

enum SpaceBehaviour
{
	SB_IGNORE = 0,
	
	SB_WHITESPACE,
	
	SB_NON_WHITESPACE
};

#ifdef __cplusplus
extern "C" {
#endif

char *CopyToNewString (const char *start_p, const char *end_p, const BOOL trim_flag);


const char *ScrollPastWhitespace (const char *text_p, const char * const bounds_p, const char * const delimiters_s, const enum SpaceBehaviour space_flag, const BOOL stop_on_delimiters_flag);


struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s);


int32 ScanDirectories (CONST_STRPTR name_s, struct List *matching_files_list_p, const BOOL recurse_flag);


BOOL AddFullHeaderPathToList (struct List *header_definitions_p, CONST STRPTR dir_s, CONST STRPTR name_s);


#ifdef __cplusplus
}
#endif


#endif		/* ifndef UTILS_H */
