/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: LibraryGenerator
**
** File: 
**
** Date: 19-06-2013 16:07:32
**
************************************************************

*/

#include <stdio.h>

#include "library_utils.h"


BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version)
{
	if ((*library_pp = IExec->OpenLibrary (lib_name_s, lib_version)) != NULL)
		{
			if ((*interface_pp = IExec->GetInterface (*library_pp, interface_name_s, interface_version, NULL)) != NULL)
				{
					return TRUE;
				}
			else
				{
					printf ("failed to open interface \"%s\" version %lu from \"%s\"\n", interface_name_s, interface_version, lib_name_s);
				}
			IExec->CloseLibrary (*library_pp);
		}
	else
		{
			printf ("failed to open library \"%s\" version %lu\n", lib_name_s, lib_version);
		}

	return FALSE;
}


void CloseLib (struct Library *library_p, struct Interface *interface_p)
{
	if (interface_p)
		{
			IExec->DropInterface (interface_p);
		}

	if (library_p)
		{
			IExec->CloseLibrary (library_p);
		}
}
