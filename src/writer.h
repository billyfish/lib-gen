/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File: 
**
** Date: 20-09-2014 19:53:18
**
************************************************************

*/

#ifndef WRITER_H
#define WRITER_H

#include <stdio.h>
#include <exec/lists.h>

struct Writer;

typedef struct Writer
{
	BOOL (*wr_write_function_definitions_fn) (struct Writer *writer_p, const struct List *function_definitions_p, FILE *out_f);
} Writer;


BOOL WriteFunctionDefinitions (struct Writer *writer_p, const struct List *function_definitions_p, FILE *out_f);


#endif		/* #ifndef WRITER_H */
 
