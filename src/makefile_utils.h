/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File: 
**
** Date: 02-10-2014 22:44:31
**
************************************************************

*/

#ifndef MAKEFILE_UTILS_H
#define MAKEFILE_UTILS_H

#include <exec/types.h>


BOOL WriteMakefileHeader (BPTR makefile_p, CONST_STRPTR library_s);

BOOL WriteMakefileFooter (BPTR makefile_p);

BOOL AddFileToMakefileSources (BPTR makefile_p, CONST_STRPTR filename_s);

BOOL CloseMakefile (BPTR makefile_p);

BPTR GetMakefileHandle (CONST_STRPTR library_s);


#endif		/* #ifndef MAKEFILE_UTILS_H */

