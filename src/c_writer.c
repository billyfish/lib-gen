#include "c_writer.h"

Writer *AllocateCWriter (void)
{
	CWriter *cw_p = (CWriter *) AllocMemory (sizeof (CWriter));

	if (cw_p)
		{
			cw_p -> cw_base_writer.wr_write_header_definitions_list_fn = NULL;
			cw_p -> cw_base_writer.wr_get_file_suffix_fn = NULL;
		}

	return ((Writer *) cw_p);
}


void FreeCWriter (Writer *writer_p)
{
	CWriter *cw_p = (CWriter *) writer_p;

	FreeMemory (cw_p);
} 
