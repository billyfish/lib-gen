#ifndef WRITER_H
#define WRITER_H

#include <exec/lists.h>
#include <exec/types.h>

struct Writer;

typedef struct Writer
{
	BOOL (*wr_write_function_defintion_fn) (struct Writer *writer_p, const struct FunctionDefinition *fd_p, BPTR out_p);
} Writer;


BOOL WriteFunctionDefinitions (Writer *writer_p, struct List *fds_p, BPTR out_p);


#endif