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
#include "header_definitions.h"
#include "debugging_utils.h"
#include "idl_writer.h"


static BOOL OpenLibs (void);
static void CloseLibs (void);

static BOOL OpenLib (struct Library **library_pp, CONST_STRPTR lib_name_s, const uint32 lib_version, struct Interface **interface_pp, CONST_STRPTR interface_name_s, const uint32 interface_version);

static void CloseLib (struct Library *library_p, struct Interface *interface_p);

BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p, struct HeaderDefinitions *hdr_defs_p);
BOOL ParseFile (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct HeaderDefinitions *header_defs_p);
BOOL GeneratePrototypesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR function_pattern_s, const BOOL recurse_flag, struct List *header_definitions_p);

int Run (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR prototype_pattern_s, CONST_STRPTR output_dir_s, const BOOL recurse_flag);


enum Args
{
	AR_INPUT_DIR,
	AR_RECURSE,
	AR_OUTPUT_DIR,
	AR_INPUT_FILE_PATTERN,
	AR_PROTOTYPE_PATTERN,

	/** The output format */
	AR_OUTPUT_FORMAT,
	AR_NUM_ARGS
};


int main (int argc, char *argv [])
{
	int result = 0;

	if (OpenLibs ())
		{
			CONST_STRPTR input_dir_s = NULL;
			CONST_STRPTR output_dir_s = NULL;
			CONST_STRPTR filename_pattern_s = "#?.h";
			CONST_STRPTR prototype_pattern_s = NULL;
			CONST_STRPTR format_s = "idl";

			BOOL recurse_flag = FALSE;
			int32 args [AR_NUM_ARGS];
			struct RDArgs *args_p = NULL;

			memset (args, 0, AR_NUM_ARGS * sizeof (int32));

			args_p = IDOS->ReadArgs ("I=Input/A,R=Recurse/S,O=Output/A,IP=InputPattern/K,PP=PrototypePattern/K,FMT=Format/K", args, NULL);

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

					if (args [AR_OUTPUT_FORMAT])
						{
							format_s = (CONST_STRPTR) args [AR_OUTPUT_FORMAT];
						}

					IDOS->Printf ("Input Dir = \"%s\"\n", input_dir_s);
					IDOS->Printf ("Output Dir = \"%s\"\n", output_dir_s);
					IDOS->Printf ("Filename Pattern = \"%s\"\n", filename_pattern_s);
					IDOS->Printf ("Recurse = \"%s\"\n", recurse_flag ? "TRUE" : "FALSE");
					IDOS->Printf ("PrototypePattern = \"%s\"\n", prototype_pattern_s);
					IDOS->Printf ("OutputFormat = \"%s\"\n", format_s);

					if (input_dir_s && filename_pattern_s)
						{
							result = Run (input_dir_s, filename_pattern_s, prototype_pattern_s, output_dir_s, recurse_flag);
						}		/* if (input_dir_s && filename_pattern_s) */


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


int Run (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR prototype_pattern_s, CONST_STRPTR output_s, const BOOL recurse_flag)
{
	int res = 0;
	STRPTR prototype_regexp_s = NULL;
	/* List of HeaderDefinitionsNodes */
	struct List headers_list;

	IExec->NewList (&headers_list);

	if (prototype_pattern_s)
		{
			size_t l = (2 * strlen (prototype_pattern_s)) + 2;
			
			prototype_regexp_s = (STRPTR) IExec->AllocVecTags (l, TAG_DONE);
			
			if (prototype_regexp_s)
				{
					int32 is_wild = IDOS->ParsePatternNoCase (prototype_pattern_s, prototype_regexp_s, l);
					
					if (is_wild < 0)
						{
							IDOS->Printf ("Error creating pattern from \"%s\"\n", prototype_pattern_s);
						}
				}
			else
				{
					IDOS->Printf ("Not enough memory to create regular expression from \"%s\"\n", prototype_pattern_s);
				}
		}

	if (GeneratePrototypesList (root_path_s, filename_pattern_s, prototype_regexp_s, recurse_flag, &headers_list))
		{
			Writer *writer_p = AllocateIDLWriter ();
			
			if (writer_p)
				{
					BPTR out_p = IDOS->FOpen (output_s, MODE_NEWFILE, 0);
					
					if (out_p)
						{
							IDOS->Printf ("%lu headers\n", GetHeaderDefinitionsListSize (&headers_list));
						
							if (WriteHeaderDefinitionsList (writer_p, &headers_list, out_p))
								{
									IDOS->Printf ("Successfully wrote header definitions to %s\n", output_s);
								}
							else
								{
									IDOS->Printf ("Failed to write header definitions to %s\n", output_s);
								}

							IDOS->FClose (out_p);
						}
					else
						{
							IDOS->Printf ("Failed to open %s for writing\n", output_s);
						}
					
					FreeIDLWriter (writer_p);	
				}
			else
				{
					IDOS->Printf ("Failed to get Writer\n");
				}
		}

	if (prototype_regexp_s)
		{
			IExec->FreeVec (prototype_regexp_s);
		}

	return res;
}


BOOL GeneratePrototypesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR function_pattern_s, const BOOL recurse_flag, struct List *header_definitions_p)
{
	BOOL success_flag = FALSE;

	/* Get a list of the header filenames */
	if (ScanDirectories (root_path_s, header_definitions_p, filename_pattern_s, recurse_flag))
		{
			struct HeaderDefinitionsNode *curr_hdr_def_node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (header_definitions_p);
			struct HeaderDefinitionsNode *next_hdr_def_node_p = NULL;

			success_flag = TRUE;
			
			/* Open each of the matched filenames in turn */
			while ((next_hdr_def_node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc ((struct Node *) curr_hdr_def_node_p)) != NULL)
				{
					struct HeaderDefinitions *current_header_defs_p = curr_hdr_def_node_p -> hdn_defs_p;
					CONST_STRPTR filename_s = current_header_defs_p -> hd_filename_s;

					IDOS->Printf ("Parsing \"%s\"\n", filename_s);

					/* Get the list of matching prototypes in each file */
					if (!ParseFile (function_pattern_s, filename_s, current_header_defs_p))
						{
							success_flag = FALSE;
						}

					curr_hdr_def_node_p = next_hdr_def_node_p;
				}

		}

	DB (KPRINTF ("%s %ld - GeneratePrototypesList %ld\n", __FILE__, __LINE__, success_flag));

	return success_flag;
}



BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p, struct HeaderDefinitions *header_defs_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{			
			int32 count;
			
			success_flag = TRUE;
			
			while ((count = IDOS->FReadLine (handle_p, line_p)) > 0)
				{
					if (IUtility->Strnicmp (pattern_s, line_p -> frld_Line, pattern_length) == 0)
						{
							char *prototype_s = (line_p -> frld_Line) + pattern_length;
							struct FunctionDefinition *fn_def_p = NULL;

							//IDOS->Printf (">>> matched line:= %s", line_p -> frld_Line);

							fn_def_p = TokenizeFunctionPrototype (prototype_s);

							if (fn_def_p)
								{
									/* Add the prototype */
									if (AddFunctionDefinitionToHeaderDefinitions (header_defs_p, fn_def_p))
										{
											DB (KPRINTF ("%s %ld - GetMatchingPrototypes: Added function definition for \"%s\"\n", __FILE__, __LINE__, prototype_s));
										}
									else
										{
											IDOS->Printf ("Failed to add function definition for \"%s\"\n", prototype_s);
											success_flag = FALSE;
										}
								}
							else
								{
									IDOS->Printf ("Failed to tokenize \"%s\"\n", prototype_s);
									success_flag = FALSE;
								}

						}
					else
						{
							//IDOS->Printf ("line:= %s", line_p -> frld_Line);
						}
				}

			success_flag = (count == 0);

			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
		}


	DB (KPRINTF ("%s %ld - GetMatchingPrototypes %ld\n", __FILE__, __LINE__, success_flag));
	
	return success_flag;
}




BOOL ParseFile (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct HeaderDefinitions *header_defs_p)
{
	struct FReadLineData *line_data_p = IDOS->AllocDosObject (DOS_FREADLINEDATA, 0);
	BOOL success_flag = FALSE;

	if (line_data_p)
		{
			const size_t pattern_length = strlen (pattern_s);

			success_flag = GetMatchingPrototypes (filename_s, pattern_s, pattern_length, line_data_p, header_defs_p);

			if (success_flag)
				{
					IDOS->Printf ("read \"%s\" successfully\n", filename_s);
				}
			else
				{
					IDOS->Printf ("failed to read \"%s\"\n", filename_s);
				}


			IDOS->FreeDosObject (DOS_FREADLINEDATA, line_data_p);
		}		/* if (line_data_p) */


	DB (KPRINTF ("%s %ld - ParseFile %ld\n", __FILE__, __LINE__, success_flag));

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


