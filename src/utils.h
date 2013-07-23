#ifndef UTILS_H
#define UTILS_H

#include "types.h"


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


const char *ScrollPastWhitespace (const char *text_p, const char * const bounds_p, const char * const delimiters_s, const enum SpaceBehaviour space_flag);


struct FunctionDefinition *TokenizeFunctionPrototype (const char *prototype_s);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef UTILS_H */
