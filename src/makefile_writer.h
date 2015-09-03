#ifndef MAKEFILE_WRITER_H
#define MAKEFILE_WRITER_H

#include <exec/lists.h>


#ifdef __cplusplus
extern "C" {
#endif

BOOL WriteMakefile (CONST CONST_STRPTR  makefile_s, CONST CONST_STRPTR library_s, struct List * const fn_defs_p);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef MAKEFILE_WRITER_HH */

