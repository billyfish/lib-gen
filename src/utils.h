#ifndef UTILS_H
#define UTILS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

char *CopyToNewString (const char *start_p, const char *end_p, const BOOL trim_flag);

const char *ScrollPastWhiteSpace (const char *text_p, const char * const bounds_p, const BOOL space_flag);

char *TokenizeFunctionPrototype (const char *prototype_s);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef UTILS_H */
