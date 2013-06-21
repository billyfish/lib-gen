/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: LibraryGenerator
**
** File: 
**
** Date: 19-06-2013 16:08:52
**
************************************************************

*/

#ifndef LIBRARY_UTILS_H
#define LIBRARY_UTILS_H

#include <exec/types.h>

#include <proto/exec.h>


BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version);

void CloseLib (struct Library *library_p, struct Interface *interface_p);

#endif		/* ifndef LIBRARY_UTILS_H */

