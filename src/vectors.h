/*

************************************************************
**
** Created by: CodeBench 0.42 (14.10.2013)
**
** Project: libgen
**
** File: vectors.h
**
** Date: 11-06-2016 14:48:04
**
************************************************************

*/

#ifndef VECTORS_H
#define VECTORS_H

#include <dos/dos.h>

#include <exec/lists.h>


BOOL WriteVectorsFile (CONST CONST_STRPTR source_directory_s, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, struct List *function_defs_p);

#endif

