#ifndef IDL_WRITER_H
#define IDL_WRITER_H

#include "writer.h"

typedef struct IDLWriter
{
	Writer iw_base_writer;
} IDLWriter;


Writer *AllocateIDLWriter ();

void FreeIDLWriter (Writer *writer_p);



#endif
