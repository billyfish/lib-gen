/*

************************************************************
**
** Created by: CodeBench 0.42 (14.10.2013)
**
** Project: libgen
**
** File: 
**
** Date: 07-06-2016 23:26:57
**
************************************************************

*/

#include <proto/dos.h>



BOOL WriteVectorsFile (BPTR vector_file_p, CONST CONST_STRPTR library_s)
{
	/* Add the system includes */
	if (IDOS->FPrintf (vector_file_p, "#include <exec/exec.h>\n#include <exec/interfaces.h>\n#include <exec/types.h>\n\n") >= 0)
		{
			/* Add our library includes */
			
			/* Declare our library functions */
			
			
			/* Declare the library vectors */
			
		}		
	
	return FALSE;
}


BOOL WriteLibraryIncludes (BPTR vector_file_p, 
