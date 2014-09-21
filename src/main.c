/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: LibraryGenerator
**
** File:
**
** Date: 16-06-2013 14:13:11
**
************************************************************

*/

#include <stdio.h>
#include <string.h>

#include <dos/dos.h>

#include <exec/types.h>
#include <exec/exectags.h>
#include <exec/lists.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include "unit_test.h"
#include "string_list.h"
#include "utils.h"


static BOOL OpenLibs (void);
static void CloseLibs (void);

static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version);

static void CloseLib (struct Library *library_p, struct Interface *interface_p);

BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p, struct List *prototypes_list_p);
BOOL ParseFiles (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct List *prototypes_p);
BOOL GeneratePrototypesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR function_pattern_s);

enum Args
{
	AR_INPUT_DIR,
	AR_RECURSE,
	AR_OUTPUT_DIR,
	AR_INPUT_FILE_PATTERN,
	AR_PROTOTYPE_PATTERN,
	AR_NUM_ARGS
};


int main (int argc, char *argv [])
{
	
	if (OpenLibs ())
		{
			CONST_STRPTR input_dir_s = NULL;
			CONST_STRPTR output_dir_s = NULL;
			CONST_STRPTR filename_pattern_s = "#?.h";
			CONST_STRPTR prototype_pattern_s = NULL;
			
			BOOL recurse_flag = FALSE;
			int32 args [AR_NUM_ARGS];
			struct RDArgs *args_p = IDOS->ReadArgs ("I=Input/A,R=Recurse/S,O=Output/A,IP=InputPattern/K,PP=PrototypePattern/K", args, NULL);
			
			if (args_p != NULL)
				{
					input_dir_s = (CONST_STRPTR) args [AR_INPUT_DIR];
					output_dir_s = (CONST_STRPTR) args [AR_OUTPUT_DIR];
					
					if (args [AR_RECURSE])
						{
							recurse_flag = TRUE;
						}
					
					if (args [AR_INPUT_FILE_PATTERN])
						{
							filename_pattern_s = (CONST_STRPTR) args [AR_INPUT_FILE_PATTERN];
						}					

					
					if (args [AR_PROTOTYPE_PATTERN])
						{
							prototype_pattern_s = (CONST_STRPTR) args [AR_PROTOTYPE_PATTERN];
						}					

					IDOS->Printf ("Input Dir = \"%s\"\n", input_dir_s);
					IDOS->Printf ("Output Dir = \"%s\"\n", output_dir_s);
					IDOS->Printf ("Filename Pattern = \"%s\"\n", filename_pattern_s);
					IDOS->Printf ("Recurse = \"%s\"\n", recurse_flag ? "TRUE" : "FALSE");
					IDOS->Printf ("PrototypePattern = \"%s\"\n", prototype_pattern_s);					
					
					IDOS->FreeArgs (args_p);
				} 
			else
				{
					IDOS->PrintFault (IDOS->IoErr (), "Unable to parse command-line args");
				}
			
			
			/*
			if (argc > 2)
				{
					ParseFiles (argv [1], argv [2]);
				}
			else if ((argc == 2) && (strcmp ("test", argv [1]) == 0))
				{
					UnitTest ("unit_test_out.xml");
				}
			else
				{
					IDOS->Printf ("LibraryGenerator <pattern> <filename>\n");
				}
			*/
			
			CloseLibs ();
		}
	else
		{
			printf ("no libs\n");
		}

	return 0;
}


BOOL GeneratePrototypesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR function_pattern_s)
{
	BOOL success_flag = FALSE;
	struct List headers_list;

	IExec->NewList (&headers_list);

	if (RecursiveScan (root_path_s, &headers_list))
		{
			struct List function_definitions_list;
			struct StringNode *curr_filename_node_p = (struct StringNode *) IExec->GetHead (&headers_list);
			struct StringNode *next_filename_node_p = NULL;

			IExec->NewList (&function_definitions_list);

			/* Open each of the matched filenames in turn */
			while ((next_filename_node_p = (struct StringNode *) (curr_filename_node_p -> sn_node.ln_Succ)) != NULL)
				{
					CONST STRPTR filename_s = curr_filename_node_p -> sn_value_s;

					if (ParseFiles (function_pattern_s, filename_s, &function_definitions_list))
						{

						}

					curr_filename_node_p = next_filename_node_p;
				}

		}

	return success_flag;
}




BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p, struct List *prototypes_list_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			int32 count;

			while ((count = IDOS->FReadLine (handle_p, line_p)) > 0)
				{
					if (IUtility->Strnicmp (pattern_s, line_p -> frld_Line, pattern_length) == 0)
						{
							IDOS->Printf (">>> matched line:= %s", line_p -> frld_Line);

						}
					else
						{
							IDOS->Printf ("line:= %s", line_p -> frld_Line);
						}
				}

			success_flag = (count == 0);

			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
		}

	return success_flag;
}




BOOL ParseFiles (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct List *prototypes_p)
{
	struct FReadLineData *line_data_p = IDOS->AllocDosObject (DOS_FREADLINEDATA, 0);
	BOOL success_flag = FALSE;

	if (line_data_p)
		{
			BOOL read_file_flag;
			const size_t pattern_length = strlen (pattern_s);

			read_file_flag = GetMatchingPrototypes (filename_s, pattern_s, pattern_length, line_data_p, prototypes_p);

			if (read_file_flag)
				{
					IDOS->Printf ("read \"%s\" successfully\n", filename_s);
				}
			else
				{
					IDOS->Printf ("failed to read \"%s\"\n", filename_s);
				}


			IDOS->FreeDosObject (DOS_FREADLINEDATA, line_data_p);
		}		/* if (line_data_p) */

	return success_flag;
}




static BOOL OpenLibs (void)
{
	if (OpenLib (&DOSBase, "dos.library", 52L, (struct Interface **) &IDOS, "main", 1))
		{
			if (OpenLib (&UtilityBase, "utility.library", 52L, (struct Interface **) &IUtility, "main", 1))
				{
					return TRUE;
				}

			CloseLib (DOSBase, (struct Interface *) IDOS);
		}

	return FALSE;
}

static void CloseLibs (void)
{
	CloseLib (UtilityBase, (struct Interface *) IUtility);
	CloseLib (DOSBase, (struct Interface *) IDOS);
}


static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version)
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


static void CloseLib (struct Library *library_p, struct Interface *interface_p)
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


