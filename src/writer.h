#ifndef WRITER_H
#define WRITER_H

typedef struct Writer
{
	BOOL (*wr_write_function_defintion_fn)(FILE *out_f);
} Writer;


#endif