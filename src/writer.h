#ifndef WRITER_H
#define WRITER_H

#include <dos/dos.h>
#include <exec/lists.h>
#include <exec/types.h>

#include "function_definition.h"

struct Writer;

typedef struct Writer
{
	BOOL (*wr_write_header_definitions_list_fn) (struct Writer *writer_p, struct List *header_definitions_p, BPTR out_p);
} Writer;


BOOL WriteHeaderDefinitionsList (struct Writer *writer_p, struct List *header_definitions_p, BPTR out_p);

#endif

