/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File:
**
** Date: 21-09-2014 00:08:56
**
************************************************************

*/

#include <proto/dos.h>

#include "writer.h"
#include "debugging_utils.h"


#ifdef _DEBUG
#define WRITER_DEBUG (1)
#endif



BOOL WriteFunctionDefinitionsList (struct Writer *writer_p, struct List *function_defs_p, CONST_STRPTR library_s, const int32 version, const enum InterfaceFlag flag, BPTR out_p)
{
	ENTER ();

	BOOL success_flag = writer_p -> wr_write_function_definitions_list_fn (writer_p, function_defs_p, library_s, version, flag, out_p);

	IDOS->FFlush (out_p);

	LEAVE ();
	return success_flag;
}



CONST_STRPTR GetWriterFileSuffix (struct Writer *writer_p)
{
	ENTER ();

	LEAVE ();
	return (writer_p -> wr_get_file_suffix_fn (writer_p));
}

