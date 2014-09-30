#include "c_writer.h"

Writer *AllocateCWriter (void)
{
	CWriter *cw_p = (CWriter *) AllocMemory (sizeof (CWriter));

	if (idl_p)
		{
			idl_p -> iw_base_writer.wr_write_header_definitions_list_fn = WriteCSource;
			idl_p -> iw_base_writer.wr_get_file_suffix_fn = GetCWriterFileSuffix;
		}

	return ((Writer *) idl_p);
}


void FreeIDLWriter (Writer *writer_p)
{
	CWriter *cw_p = (CWriter *) writer_p;

	FreeMemory (cw_p);
} 
