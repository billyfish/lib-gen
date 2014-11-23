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

#include "utils.h"
#include "header_definitions.h"
#include "debugging_utils.h"
#include "idl_writer.h"
#include "library_utils.h"

static BOOL OpenLibs (void);
static void CloseLibs (void);


BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, struct FReadLineData *line_p, struct HeaderDefinitions *hdr_defs_p);
BOOL ParseFile (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct HeaderDefinitions *header_defs_p);
BOOL GeneratePrototypesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR function_pattern_s, const BOOL recurse_flag, struct List *header_definitions_p);

STRPTR CreateRegEx (CONST_STRPTR pattern_s, BOOL capture_flag);
void ClearCapturedExpression (struct CapturedExpression *capture_p);

int Run (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR prototype_pattern_s, CONST_STRPTR library_s, const BOOL recurse_flag, const int32 version, const enum InterfaceFlag flag, const BOOL gen_source_flag);




enum Args
{
	AR_INPUT_DIR,
	AR_RECURSE,
	AR_LIBRARY_NAME,
	AR_INPUT_FILE_PATTERN,
	AR_PROTOTYPE_PATTERN,
	AR_VERSION,
	AR_FLAGS,
	AR_GENERATE_CODE,

	/** The output format */
	AR_OUTPUT_FORMAT,
	AR_VERBOSE,
	AR_NUM_ARGS
};


int main (int argc, char *argv [])
{
	int result = 0;

	if (OpenLibs ())
		{
			CONST_STRPTR input_dir_s = NULL;
			CONST_STRPTR library_s = NULL;
			CONST_STRPTR filename_pattern_s = "#?.h";
			CONST_STRPTR prototype_pattern_s = NULL;
			CONST_STRPTR format_s = "idl";
			int32 version = 1;
			enum InterfaceFlag flag = IF_PUBLIC;
			BOOL recurse_flag = FALSE;
			BOOL generate_code_flag = FALSE;
			int32 args [AR_NUM_ARGS];
			struct RDArgs *args_p = NULL;

			memset (args, 0, AR_NUM_ARGS * sizeof (int32));

			args_p = IDOS->ReadArgs ("I=Input/A,R=Recurse/S,L=LibraryName/A,IP=InputPattern/K,PP=PrototypePattern/K,VER=Version/N,FL=Flags/K,GC=GenerateCode/S,FMT=Format/K,V=Verbose/N", args, NULL);

			if (args_p != NULL)
				{
					input_dir_s = (CONST_STRPTR) args [AR_INPUT_DIR];
					library_s = (CONST_STRPTR) args [AR_LIBRARY_NAME];

					if (args [AR_RECURSE])
						{
							recurse_flag = TRUE;
						}

					if (args [AR_INPUT_FILE_PATTERN])
						{
							filename_pattern_s = (CONST_STRPTR) args [AR_INPUT_FILE_PATTERN];
						}


					if ((args [AR_PROTOTYPE_PATTERN]) && (strlen ((CONST_STRPTR) args [AR_PROTOTYPE_PATTERN]) > 0))
						{
							prototype_pattern_s = (CONST_STRPTR) args [AR_PROTOTYPE_PATTERN];
						}

					if (args [AR_OUTPUT_FORMAT])
						{
							format_s = (CONST_STRPTR) args [AR_OUTPUT_FORMAT];
						}

					if (args [AR_FLAGS])
						{
							CONST_STRPTR value_s = (CONST_STRPTR) args [AR_FLAGS];

							if (IUtility->Stricmp (value_s, "private") == 0)
								{
									flag = IF_PRIVATE;
								}
							else if (IUtility->Stricmp (value_s, "protected") == 0)
								{
									flag = IF_PROTECTED;
								}
							else if (IUtility->Stricmp (value_s, "none") == 0)
								{
									flag = IF_PUBLIC;
								}
							else
								{
									IDOS->Printf ("Ignoring invalid flag \"%s\", must be either private, protected or none");
								}
						}

					if (args [AR_VERSION])
						{
							version = * ((int32 *) args [AR_VERSION]);
						}

					if (args [AR_VERBOSE])
						{
							SetVerbosity (* ((int32 *) args [AR_VERBOSE]));
						}
					else
						{
							SetVerbosity (VB_NORMAL);
						}


					if (args [AR_GENERATE_CODE])
						{
							generate_code_flag = TRUE;
						}


					if (GetVerbosity () >= VB_LOUD)
						{
							IDOS->Printf ("Input Dir = \"%s\"\n", input_dir_s);
							IDOS->Printf ("Library Name  = \"%s\"\n", library_s);
							IDOS->Printf ("Filename Pattern = \"%s\"\n", filename_pattern_s);
							IDOS->Printf ("Recurse = \"%s\"\n", recurse_flag ? "TRUE" : "FALSE");
							IDOS->Printf ("Generate Code = \"%s\"\n", generate_code_flag ? "TRUE" : "FALSE");
							IDOS->Printf ("Prototype Pattern = \"%s\"\n", prototype_pattern_s);
							IDOS->Printf ("Output Format = \"%s\"\n", format_s);
							IDOS->Printf ("Version = \"%ld\"\n", version);

							switch (flag)
								{
									case IF_PUBLIC:
										IDOS->Printf ("Flags = \"none\"\n");
										break;

									case IF_PROTECTED:
										IDOS->Printf ("Flags = \"protected\"\n");
										break;

									case IF_PRIVATE:
										IDOS->Printf ("Flags = \"private\"\n");
										break;
								}

						}		/* if (verbose_flag) */


					if (input_dir_s && filename_pattern_s && library_s)
						{
							result = Run (input_dir_s, filename_pattern_s, prototype_pattern_s, library_s, recurse_flag, version, flag, generate_code_flag);
						}		/* if (input_dir_s && filename_pattern_s) */


					IDOS->FreeArgs (args_p);
				}
			else
				{
					IDOS->PrintFault (IDOS->IoErr (), "Unable to parse command-line args");
				}
			CloseLibs ();
		}
	else
		{
			printf ("no libs\n");
		}

	return 0;
}


STRPTR CreateRegEx (CONST_STRPTR pattern_s, BOOL capture_flag)
{
	STRPTR reg_ex_s = NULL;

	if (pattern_s)
		{
			size_t l = (2 * strlen (pattern_s)) + 2;

			reg_ex_s = (STRPTR) IExec->AllocVecTags (l, TAG_DONE);

			if (reg_ex_s)
				{
					int32 is_wild;

					if (capture_flag)
						{
							is_wild = IDOS->ParseCapturePattern (pattern_s, reg_ex_s, l, TRUE);
						}
					else
						{
							is_wild = IDOS->ParsePatternNoCase (pattern_s, reg_ex_s, l);
						}

					if (is_wild < 0)
						{
							IDOS->Printf ("Error creating pattern from \"%s\"\n", pattern_s);
						}
				}
			else
				{
					IDOS->Printf ("Not enough memory to create regular expression from \"%s\"\n", pattern_s);
				}
		}

	return reg_ex_s;
}


int Run (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR prototype_pattern_s, CONST_STRPTR library_s, const BOOL recurse_flag, const int32 version, const enum InterfaceFlag flag, const BOOL gen_source_flag)
{
	int res = 0;
	STRPTR prototype_regexp_s = NULL;
	STRPTR filename_regexp_s = NULL;
	/* List of HeaderDefinitionsNodes */
	struct List headers_list;

	IExec->NewList (&headers_list);

	if (prototype_pattern_s)
		{
			prototype_regexp_s = CreateRegEx (prototype_pattern_s, TRUE);

			if (!prototype_regexp_s)
				{
					return -1;
				}
		}

	if (filename_pattern_s)
		{
			filename_regexp_s = CreateRegEx (filename_pattern_s, TRUE);

			if (!filename_regexp_s)
				{
					return -1;
				}
		}


	if (GeneratePrototypesList (root_path_s, filename_regexp_s, prototype_regexp_s, recurse_flag, &headers_list))
		{
			Writer *writer_p = AllocateIDLWriter ();

			if (writer_p)
				{
					STRPTR output_s = ConcatenateStrings (library_s, GetWriterFileSuffix (writer_p));

					if (output_s)
						{
							BPTR out_p = IDOS->FOpen (output_s, MODE_NEWFILE, 0);

							if (out_p)
								{
									IDOS->Printf ("%lu headers\n", GetHeaderDefinitionsListSize (&headers_list));

									if (WriteHeaderDefinitionsList (writer_p, &headers_list, library_s, version, flag, out_p))
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

							IExec->FreeVec (output_s);
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


	if (filename_regexp_s)
		{
			IExec->FreeVec (filename_regexp_s);
		}


	if (gen_source_flag)
		{
			STRPTR output_dir_s = ConcatenateStrings (library_s, "_source");

			if (output_dir_s)
				{
					if (EnsureDirectoryExists (output_dir_s))
						{
							if (WriteSourceForAllHeaderDefinitions (&headers_list, output_dir_s, library_s))
								{
									IDOS->Printf ("Generating source succeeded");
								}
							else
								{
									IDOS->Printf ("Generating source failed");
								}
						}
					else
						{
							IDOS->Printf ("Could not create source output directory \"%s\"\n", output_dir_s);
						}

					IExec->FreeVec (output_dir_s);
				}
			else
				{
					IDOS->Printf ("Not enough memory for source output directory name\n");
				}


		}


	ClearHeaderDefinitionsList (&headers_list);

	return res;
}





BOOL GeneratePrototypesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_regexp_s, CONST_STRPTR function_regexp_s, const BOOL recurse_flag, struct List *header_definitions_p)
{
	BOOL success_flag = FALSE;

	/* Get a list of the header filenames */
	if (ScanDirectories (root_path_s, header_definitions_p, filename_regexp_s, recurse_flag))
		{
			struct HeaderDefinitionsNode *node_p;

			success_flag = TRUE;


			IDOS->Printf ("Found %lu header files\n", GetHeaderDefinitionsListSize (header_definitions_p));

			for (node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (header_definitions_p); node_p != NULL; node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc ((struct Node *) node_p))
				{
					struct HeaderDefinitions *header_defs_p = node_p -> hdn_defs_p;
					CONST_STRPTR filename_s = header_defs_p -> hd_filename_s;

					IDOS->Printf ("Parsing \"%s\"\n", filename_s);

					/* Get the list of matching prototypes in each file */
					if (!ParseFile (function_regexp_s, filename_s, header_defs_p))
						{
							success_flag = FALSE;
						}
				}

		}

	DB (KPRINTF ("%s %ld - GeneratePrototypesList %ld\n", __FILE__, __LINE__, success_flag));

	return success_flag;
}


BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, struct FReadLineData *line_p, struct HeaderDefinitions *header_defs_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			int32 count;
			struct CapturedExpression capture;
			struct CapturedExpression *capture_p = &capture;

			success_flag = TRUE;
			memset (capture_p, 0, sizeof (struct CapturedExpression));

			DB (KPRINTF ("%s %ld - GetMatchingPrototypes: pattern \"%s\"\n", __FILE__, __LINE__, pattern_s));

			/*
			 TODO: Need to be able to read in multi-line function definitions e.g.

				 int foo (int bar,
					 int boo);

				or

					int
					foo (int bar, int boo);

				etc.
		 */
			while ((count = IDOS->FReadLine (handle_p, line_p)) > 0)
				{
					/* DB (KPRINTF ("%s %ld - GetMatchingPrototypes: line \"%s\"\n", __FILE__, __LINE__, line_p -> frld_Line));	*/

					if (IDOS->CapturePattern (pattern_s, line_p -> frld_Line, TRUE, &capture_p) != 0)
						{
							/* we only want the first match */
							STRPTR prototype_s = capture_p -> cape_Match;

							if (capture_p)
								{
									DB (KPRINTF ("%s %ld - GetMatchingPrototypes: cap match %8X start %8X end %8X next %8X\n", __FILE__, __LINE__, capture_p -> cape_Match,  capture_p -> cape_Start, capture_p -> cape_End, capture_p -> cape_Next));

								if  (capture_p -> cape_Start)
								{
									DB (KPRINTF ("%s %ld - GetMatchingPrototypes: start \"%s\" end \"%s\" match \"%s\"\n", __FILE__, __LINE__, capture_p -> cape_Start, (capture_p -> cape_End) - 1, capture_p -> cape_Match));
								}
							else
								{
									DB (KPRINTF ("%s %ld - GetMatchingPrototypes: start NULL\n", __FILE__, __LINE__));
								}
							}
						else
							{
								DB (KPRINTF ("%s %ld - GetMatchingPrototypes: capture_p NULL\n", __FILE__, __LINE__));
							}


							//prototype_s = CopyToNewString (capture_p -> cape_Start, (capture_p -> cape_End) - 1, FALSE);
							if (prototype_s)
								{
									struct FunctionDefinition *fn_def_p = NULL;

									//IDOS->Printf (">>> matched line:= %s", line_p -> frld_Line);

									/* For easier debugging, overwrite the \n with a \0 */
									size_t l = strlen (prototype_s);
									if (* (prototype_s + l - 1) == '\n')
										{
											* (prototype_s + l - 1) = '\0';
										}

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

									IDOS->ReleaseCapturedExpressions (capture_p);
								}		/* if (protoype_s) */
							else
								{
									IDOS->Printf ("Not enough memory to copy prototype\n");
									success_flag = FALSE;
								}

							/* Clear the capture list */
							//ClearCapturedExpression (capture_p);

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


void ClearCapturedExpression (struct CapturedExpression *capture_p)
{
	struct CapturedExpression *next_p;

	while (capture_p)
		{
			next_p = capture_p -> cape_Next;
			IExec->FreeVec (capture_p);

			capture_p = next_p;
		}

	memset (capture_p, 0, sizeof (struct CapturedExpression));
}



BOOL ParseFile (CONST_STRPTR function_regexp_s, CONST_STRPTR filename_s, struct HeaderDefinitions *header_defs_p)
{
	struct FReadLineData *line_data_p = IDOS->AllocDosObject (DOS_FREADLINEDATA, 0);
	BOOL success_flag = FALSE;

	if (line_data_p)
		{
			success_flag = GetMatchingPrototypes (filename_s, function_regexp_s, line_data_p, header_defs_p);

			if (success_flag)
				{
					if (GetVerbosity () > VB_NORMAL)
						{
							IDOS->Printf ("read \"%s\" successfully\n", filename_s);
						}
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


