#ifndef C_WRITER_H
#define C_WRITER_H

#include "writer.h"

typedef struct CWriter
{
	Writer cw_base_writer;
} CWriter;


Writer *AllocateCWriter ();

void FreeCWriter (Writer *writer_p);



#endif
 
