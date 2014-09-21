#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dos/dos.h>


#include <proto/dos.h>


#include "memory.h"
#include "parameter.h"
#include "function_definition.h"
#include "utils.h"
#include "string_list.h"
#include "debugging_utils.h"
#include "idl_writer.h"

/**************************************/
/************ PROTOTYPES **************/
/**************************************/

static int32 FunctionDefinitionUnitTest (const char * const prototype_s, struct List *fds_p);

/************************************************/


/*

1. Mark the first "(" and the ");" wrap the parameters which are delimited by commas
2. Extract this substring and split into a list by splitting at the commas.
3. For each string on this list
	a. Trim at both ends to remove trailing whitespace
	b. Find the final space which should delimit the parameter name
	c. Check for any dereferencing characters at the start of the param name
	d. Does the name have an array ([]) specification
		i. TO DO
	e. The text prior to this space denotes the parameter type.


*/

int UnitTest (CONST_STRPTR output_name_s)
{
	BPTR out_p = IDOS->FOpen (output_name_s, MODE_NEWFILE, 1024);

	if (out_p)
		{
			struct List *fds_p = AllocateFunctionDefinitionsList ();

			if (fds_p)
				{
					FunctionDefinitionUnitTest ("int main (int argc, char *argv []);", fds_p);
					FunctionDefinitionUnitTest ("int				*GetAddress (const int **ptr, const int num);", fds_p);
					FunctionDefinitionUnitTest ("struct Test *GetTest (void);", fds_p);
					FunctionDefinitionUnitTest ("void SetTest (struct Test *test_p, int num);", fds_p);

					if (!IsListEmpty (fds_p))
						{
							Writer *writer_p = AllocateIDLWriter ();

							if (writer_p)
								{
									BOOL success_flag = WriteFunctionDefinitions (writer_p, fds_p, out_p);

									FreeIDLWriter (writer_p);
								}
						}

					FreeFunctionDefinitionsList (fds_p);
				}

			IDOS->FClose (out_p);
		}

	return 0;
}



static int32 FunctionDefinitionUnitTest (const char * const prototype_s, struct List *fds_p)
{
	int32 res = 0;
	struct FunctionDefinition *fd_p = TokenizeFunctionPrototype (prototype_s);
	BPTR out_p = IDOS->Output ();
			
	if (fd_p)
		{
			IDOS->FPrintf (out_p, "********* BEGIN FD for \"%s\" *********\n", prototype_s);
			PrintFunctionDefinition (out_p, fd_p);

			if (!AddFunctionDefinitionToList (fds_p, fd_p))
				{
					FreeFunctionDefinition (fd_p);
					res = -2;
				}

			IDOS->FPrintf (out_p, "\n********* END FD *********\n\n");
		}
	else
		{
			IDOS->FPrintf (out_p, "No match for \"%s\"\n", prototype_s);
			res = -1;
		}

	return res;
}
