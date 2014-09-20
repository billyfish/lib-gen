/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File: 
**
** Date: 20-09-2014 20:01:12
**
************************************************************

*/


#ifndef IDL_WRITER_H
#define IDLWRITER_H

#include "writer.h"


typedef struct IDLWriter
{
	Writer iw_base_writer;
} IDLWriter;


Writer *AllocateIDLWriter (void);

void FreeIDLWriter (Writer *writer_p);


#endif		/* #ifndef IDL_WRITER_H */
 
