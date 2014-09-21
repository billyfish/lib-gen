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

#include "writer.h"


BOOL WriteFunctionDefinitions (struct Writer *writer_p, const struct List *function_definitions_p, BPTR out_p)
{
	return (writer_p -> wr_write_function_definitions_fn (writer_p, function_definitions_p, out_p);
}
