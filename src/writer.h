#ifndef WRITER_H
#define WRITER_H

#include <dos/dos.h>
#include <exec/lists.h>
#include <exec/types.h>

#include "function_definition.h"

struct Writer;

typedef struct Writer
{
	BOOL (*wr_write_function_definitions_fn) (struct Writer *writer_p, const struct List *function_definitions_p, BPTR out_p);
} Writer;


BOOL WriteFunctionDefinitions (struct Writer *writer_p, const struct List *function_definitions_p, BPTR out_p);

#endif

