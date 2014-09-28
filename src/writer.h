#ifndef WRITER_H
#define WRITER_H

#include <dos/dos.h>
#include <exec/lists.h>
#include <exec/types.h>

#include "header_definition.h"

struct Writer;

typedef struct Writer
{
	BOOL (*wr_write_header_definitions_list_fn) (struct Writer *writer_p, struct List *header_definitions_p, CONST_STRPTR library_s, const int32 version, const InterfaceFlag flag, BPTR out_p);
	CONST_STRPTR (*wr_get_file_suffix_fn) (struct Writer *writer_p);
} Writer;


BOOL WriteHeaderDefinitionsList (struct Writer *writer_p, struct List *header_definitions_p, CONST_STRPTR library_s, const int32 version, const InterfaceFlag flag, BPTR out_p);

CONST_STRPTR GetWriterFileSuffix (struct Writer *writer_p);

#endif

