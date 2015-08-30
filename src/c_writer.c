#include <proto/exec.h>
#include "c_writer.h"

Writer *AllocateCWriter (void)
{
	CWriter *cw_p = (CWriter *) IExec->AllocVecTags (sizeof (CWriter), TAG_DONE);

	if (cw_p)
		{
			cw_p -> cw_base_writer.wr_write_function_definitions_list_fn = NULL;
			cw_p -> cw_base_writer.wr_get_file_suffix_fn = NULL;
		}

	return ((Writer *) cw_p);
}


void FreeCWriter (Writer *writer_p)
{
	CWriter *cw_p = (CWriter *) writer_p;

	IExec->FreeVec (cw_p);
} 

