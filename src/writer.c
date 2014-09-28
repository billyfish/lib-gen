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


BOOL WriteHeaderDefinitionsList (struct Writer *writer_p, struct List *header_definitions_p, CONST_STRPTR library_s, const int32 version, const InterfaceFlag flag, BPTR out_p)
{
	BOOL success_flag = writer_p -> wr_write_header_definitions_list_fn (writer_p, header_definitions_p, library_s, version, flag, out_p);

	IDOS->FFlush (out_p);

	return success_flag;
}



CONST_STRPTR GetWriterFileSuffix (struct Writer *writer_p)
{
	return (writer_p -> wr_get_file_suffix_fn (writer_p));
}

