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
#include <ctype.h>

#include <dos/dos.h>

#include <exec/types.h>
#include <exec/exectags.h>
#include <exec/lists.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>

#define ALLOCATE_GLOBALS (1)

#include "utils.h"
#include "function_definition.h"
#include "document_parser.h"
#include "debugging_utils.h"
#include "idl_writer.h"
#include "inline_header_writer.h"
#include "init_writer.h"
#include "proto_header_writer.h"
#include "interface_h_writer.h"
#include "library_utils.h"
#include "list_utils.h"
#include "makefile_writer.h"
#include "vectors.h"


static CONST CONST_STRPTR S_DEFAULT_FILENAME_PATTERN_S = "#?.h";
static CONST CONST_STRPTR S_DEFAULT_PROTOTYPE_PATTERN_S = "LIB_API{#?}";


static BOOL OpenLibs (void);
static void CloseLibs (void);


BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, struct DocumentParser *document_parser_p, struct List *function_defs_p);
BOOL ParseFile (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct List *function_defs_p, struct DocumentParser *document_parser_p);
BOOL GeneratePrototypesList (CONST CONST_STRPTR root_path_s, CONST CONST_STRPTR filename_regexp_s, CONST CONST_STRPTR prototype_regexp_s, CONST BOOL recurse_flag, struct List *function_definitions_p);

STRPTR CreateRegEx (CONST_STRPTR pattern_s, BOOL capture_flag);
void ClearCapturedExpression (struct CapturedExpression *capture_p);

int Run (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR prototype_pattern_s, CONST_STRPTR library_s, CONST_STRPTR prefix_s, const BOOL recurse_flag, const int32 version, const enum InterfaceFlag flag, const BOOL gen_source_flag);

STRPTR MakePrototypePattern (CONST_STRPTR pattern_s);

struct List *GetFilesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_regexp_s, const BOOL recurse_flag);

static void ClearList (struct List *list_p);


BOOL WriteHeaderFiles (struct List *function_defs_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s);



static STRPTR GetPrefixString (CONST_STRPTR value_s);



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
	ENTER ();

	int result = 0;

	if (OpenLibs ())
		{
			CONST_STRPTR input_dir_s = NULL;
			CONST_STRPTR library_s = NULL;
			STRPTR prefix_s = NULL;
			CONST_STRPTR filename_pattern_s = S_DEFAULT_FILENAME_PATTERN_S;
			STRPTR prototype_pattern_s = S_DEFAULT_PROTOTYPE_PATTERN_S;
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
							prototype_pattern_s = MakePrototypePattern ((CONST_STRPTR) args [AR_PROTOTYPE_PATTERN]);
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


					if (input_dir_s && filename_pattern_s && library_s && prototype_pattern_s)
						{
							prefix_s = GetPrefixString (library_s);
							
							if (prefix_s)
								{
									result = Run (input_dir_s, filename_pattern_s, prototype_pattern_s, library_s, prefix_s, recurse_flag, version, flag, generate_code_flag);
								}
								
						}		/* if (input_dir_s && filename_pattern_s) */

					if ((prototype_pattern_s != NULL) && (prototype_pattern_s != S_DEFAULT_PROTOTYPE_PATTERN_S))
						{
							DB (KPRINTF ("%s %ld - freeing prototype_pattern_s:= \"%s\"\n", __FILE__, __LINE__, prototype_pattern_s));
							IExec->FreeVec (prototype_pattern_s);
						}

					DB (KPRINTF ("%s %ld - freeing args\n", __FILE__, __LINE__));
					IDOS->FreeArgs (args_p);
				}
			else
				{
					IDOS->PrintFault (IDOS->IoErr (), "Unable to parse command-line args");
				}

			DB (KPRINTF ("%s %ld - closing lins\n", __FILE__, __LINE__));
			CloseLibs ();
		}
	else
		{
			printf ("no libs\n");
		}

	DB (KPRINTF ("%s %ld - exiting\n", __FILE__, __LINE__));

	LEAVE ();
	return 0;
}


static STRPTR GetPrefixString (CONST_STRPTR value_s)
{
	size_t l = strlen (value_s);
	
	STRPTR prefix_s = (STRPTR) IExec->AllocVecTags (l + 3, TAG_END);
	
	
	if (prefix_s)
		{
			*prefix_s = '_';
			strncpy (prefix_s + 1, value_s, l);
			* (prefix_s + l + 1) = '_';
			* (prefix_s + l + 2) = '\0';  
		}
		
	return prefix_s;
}


STRPTR CreateRegEx (CONST_STRPTR pattern_s, BOOL capture_flag)
{
	STRPTR reg_ex_s = NULL;

	ENTER ();

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

	LEAVE ();
	return reg_ex_s;
}


int Run (CONST_STRPTR root_path_s, CONST_STRPTR filename_pattern_s, CONST_STRPTR prototype_pattern_s, CONST_STRPTR library_s, CONST_STRPTR prefix_s, const BOOL recurse_flag, const int32 version, const enum InterfaceFlag flag, const BOOL gen_source_flag)
{
	int res = 0;
	STRPTR prototype_regexp_s = NULL;
	STRPTR filename_regexp_s = NULL;

	/* List of FunctionDefinitionsNodes */
	struct List function_defs;

	/*
		List of the ordering of a previous version
		of the library so that we remain compatible
		e.g. the LVOs remain the same with any new
		functions added at the end
	*/
	struct List previous_fns_ordering_list;

	
	/*
	 List of all the source files 
	*/
	struct List source_files_list;

	ENTER ();


	IExec->NewList (&function_defs);
	IExec->NewList (&previous_fns_ordering_list);
	IExec->NewList (&source_files_list);
	
	if (prototype_pattern_s)
		{
			prototype_regexp_s = CreateRegEx (prototype_pattern_s, TRUE);

			if (!prototype_regexp_s)
				{
					LEAVE ();
					return -1;
				}
		}

	if (filename_pattern_s)
		{
			filename_regexp_s = CreateRegEx (filename_pattern_s, TRUE);

			if (!filename_regexp_s)
				{
					LEAVE ();
					return -1;
				}
		}

	if (GeneratePrototypesList (root_path_s, filename_regexp_s, prototype_regexp_s, recurse_flag, &function_defs))
		{
			Writer *writer_p = AllocateIDLWriter ();

			DB (KPRINTF ("%s %ld - writer %ld", __FILE__, __LINE__, writer_p));

			if (writer_p)
				{
					STRPTR output_s = ConcatenateStrings (library_s, GetWriterFileSuffix (writer_p));

					DB (KPRINTF ("%s %ld - output_s %s", __FILE__, __LINE__, output_s));

					if (output_s)
						{
							BPTR out_p = IDOS->FOpen (output_s, MODE_NEWFILE, 0);

							if (out_p)
								{
									DB (KPRINTF ("%s %ld - opened output_s %s", __FILE__, __LINE__, output_s));

									IDOS->Printf ("%lu headers\n", GetListSize (&function_defs));

									if (WriteFunctionDefinitionsList (writer_p, &function_defs, library_s, prefix_s, version, flag, out_p))
										{
											STRPTR makefile_s = ConcatenateStrings (library_s, ".makefile");

											IDOS->Printf ("Successfully wrote header definitions to %s\n", output_s);

											if (makefile_s)
												{
													if (WriteMakefile (makefile_s, root_path_s, library_s, &function_defs, &source_files_list))
														{
															DB (KPRINTF ("%s %ld - wrote makefile to %s", __FILE__, __LINE__, makefile_s));
														}
													else
														{

														}

													IExec->FreeVec (makefile_s);
												}
											else
												{
													IDOS->Printf ("Failed to create makefile at %s", makefile_s);
												}

											/*
												Write the makefile, vectors, init, autoinit_base, obtain and release files
											*/

										}
									else
										{
											DB (KPRINTF ("%s %ld - failed to open output_s %s", __FILE__, __LINE__, output_s));
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
				}		/* if (writer_p) */
			else
				{
					IDOS->Printf ("Failed to get Writer\n");
				}
		}

	DB (KPRINTF ("%s %ld - prototype_regexp_s:= \"%s\"\n", __FILE__, __LINE__, prototype_regexp_s));

	if (prototype_regexp_s)
		{
			IExec->FreeVec (prototype_regexp_s);
		}


	DB (KPRINTF ("%s %ld - filename_regexp_s:= \"%s\"\n", __FILE__, __LINE__, filename_regexp_s));
	if (filename_regexp_s)
		{
			IExec->FreeVec (filename_regexp_s);
		}


	if (gen_source_flag)
		{
			STRPTR output_dir_s = ConcatenateStrings (library_s, SOURCE_DIR_SUFFIX_S);

			if (output_dir_s)
				{
					if (EnsureDirectoryExists (output_dir_s))
						{
							if (WriteSourceForAllFunctionDefinitions (&function_defs, output_dir_s, library_s, prefix_s))
								{
									IDOS->Printf ("Generating source succeeded\n");
						
									if (WriteSourceForAllFunctionDeclarations (&function_defs, output_dir_s, library_s, prefix_s))
										{
											IDOS->Printf ("Generating headers succeeded\n");
								
											if (WriteInitFiles (library_s, output_dir_s))
												{
													if (WriteVectorsFile (output_dir_s, library_s, prefix_s, &function_defs))
														{
															if (WriteHeaderFiles (&function_defs, library_s, output_dir_s))
																{						
																	STRPTR init_s = NULL;
															
																	IDOS->Printf ("Generating vectors succeeded\n");
																	
																	init_s = MakeFilename (output_dir_s, "lib_init.c");
																	
																	
																	if (init_s)
																		{
																			if (CopyFile ("data/lib_manager.c.template", init_s))
																				{
																					
																				}
																			else
																				{
																					IDOS->Printf ("Failed to generate init.c");
																				}
																			
																			IExec->FreeVec (init_s);	
																		}		/* if (init_s) */
																	else
																		{
																			IDOS->Printf ("Failed to get init.c full filename");
																		}		
																		
																}		/* if (WriteHeaderFiles (&function_defs, library_s, output_dir_s)) */
															
														}
													else
														{
															IDOS->Printf ("Generating vectors failed\n");
														}
																								
												}		/* if (WriteInitFile (library_s, output_dir_s)) */
									
										}
									else
										{
											IDOS->Printf ("Generating headers failed\n");	
										}
								}
							else
								{
									IDOS->Printf ("Generating source failed\n");
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

	DB (KPRINTF ("%s %ld - pre ClearFunctionDefinitionsList\n", __FILE__, __LINE__));
	ClearFunctionDefinitionsList (&function_defs);
	DB (KPRINTF ("%s %ld - post ClearFunctionDefinitionsList\n", __FILE__, __LINE__));


	ClearList (&source_files_list);

	LEAVE ();
	return res;
}


static void ClearList (struct List *list_p)
{
	ENTER ();

	struct Node *current_node_p = IExec->GetHead (list_p);
	
	while (current_node_p)
		{
			struct Node *next_node_p = IExec->GetSucc (current_node_p);
			
			IExec->FreeVec (current_node_p -> ln_Name);
			IExec->FreeVec (current_node_p);
			
			current_node_p = next_node_p;
		}

	LEAVE ();	
}


BOOL WriteHeaderFiles (struct List *function_defs_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s)
{
	BOOL success_flag = FALSE;
	STRPTR include_dir_s = NULL;
		
	ENTER ();
	
	include_dir_s = MakeFilename (output_dir_s, "include");
	
	if (include_dir_s)
		{
			if (WriteInlineHeader (function_defs_p, library_s, include_dir_s))
				{
					if (WriteProtoHeader (library_s, include_dir_s))
						{
							if (WriteInterfaceHeader (function_defs_p, library_s, include_dir_s))
								{
									success_flag = TRUE;
								}
						}
				}
				
			IExec->FreeVec (include_dir_s);
		}		
		
	LEAVE ();
	return success_flag;
}


BOOL GetPreviousLibraryOrder (CONST_STRPTR filename_s, CONST_STRPTR struct_name_s, struct List *ordering_p, struct DocumentParser *parser_p)
{
	ENTER ();

	BOOL success_flag = FALSE;
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			int32 count;
			struct CapturedExpression capture;
			struct CapturedExpression *capture_p = &capture;
			success_flag = TRUE;
			memset (capture_p, 0, sizeof (struct CapturedExpression));

			STRPTR reg_ex_s = CreateRegEx ("#?APICALL#?(*{#?})", TRUE);

			if (reg_ex_s)
				{
					SetDocumentToParse (parser_p, handle_p);

					while ((count = IDOS->FReadLine (parser_p -> dp_file_handle_p, parser_p -> dp_line_p)) > 0)
						{


						}		/* while ((count = GetNextPrototype (parser_p, &full_prototype_s)) > 0) */

					IExec->FreeVec (reg_ex_s);
				}		/* if (reg_ex_s) */



			DB (KPRINTF ("%s %ld - GetPreviousLibraryOrder: pattern \"%s\"\n", __FILE__, __LINE__, reg_ex_s));

			/*
			 TODO: Need to be able to read in multi-line function definitions e.g.

				 int foo (int bar,
					 int boo);

				or

					int
					foo (int bar, int boo);

				etc.
		  */


			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
		}


	DB (KPRINTF ("%s %ld - GetMatchingPrototypes %ld\n", __FILE__, __LINE__, success_flag));

	LEAVE ();
	return success_flag;
}



BOOL GeneratePrototypesList (CONST CONST_STRPTR root_path_s, CONST CONST_STRPTR filename_regexp_s, CONST CONST_STRPTR prototype_regexp_s, CONST BOOL recurse_flag, struct List *function_definitions_p)
{
	ENTER ();

	BOOL success_flag = FALSE;
	struct List *headers_p = GetFilesList (root_path_s, filename_regexp_s, recurse_flag);

	if (headers_p)
		{
			uint32 num_header_files = GetListSize (headers_p);

			if (num_header_files > 0)
				{
					struct DocumentParser *document_parser_p = AllocateDocumentParser ();

					if (document_parser_p)
						{
							struct Node *node_p;

							success_flag = TRUE;
							IDOS->Printf ("Found %lu header files\n", num_header_files);

							for (node_p = IExec->GetHead (headers_p); node_p != NULL; node_p = IExec->GetSucc (node_p))
								{
									CONST_STRPTR filename_s = node_p -> ln_Name;

									IDOS->Printf ("Parsing \"%s\"\n", filename_s);

									/* Get the list of matching prototypes in each file */
									if (!ParseFile (prototype_regexp_s, filename_s, function_definitions_p, document_parser_p))
										{
											success_flag = FALSE;
										}
								}

							FreeDocumentParser (document_parser_p);
						}
					else
						{

						}

				}		/* if (num_header_files > 0) */

			FreeList (headers_p);
		}		/* if (headers_p) */

	LEAVE ();
	return success_flag;
}



struct List *GetFilesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_regexp_s, const BOOL recurse_flag)
{
	ENTER ();

	struct List *filenames_p = IExec->AllocSysObjectTags (ASOT_LIST, TAG_DONE);

	if (filenames_p)
		{
			if (!ScanDirectories (root_path_s, filenames_p, filename_regexp_s, recurse_flag))
				{
					FreeList (filenames_p);
					filenames_p = NULL;
				}

		}		/* if (filenames_p) */

	LEAVE ();
	return filenames_p;
}


BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, struct DocumentParser *parser_p, struct List *function_defs_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = ZERO;

	ENTER ();


	DB (KPRINTF ("%s %ld - GetMatchingPrototypes: About to open \"%s\"\n", __FILE__, __LINE__, filename_s));

	handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			int32 count;
			struct CapturedExpression capture;
			struct CapturedExpression *capture_p = &capture;
			STRPTR full_prototype_s = NULL;
			success_flag = TRUE;
			memset (capture_p, 0, sizeof (struct CapturedExpression));

			DB (KPRINTF ("%s %ld - GetMatchingPrototypes: file \"%s\" pattern \"%s\"\n", __FILE__, __LINE__, filename_s, pattern_s));

			/*
			 TODO: Need to be able to read in multi-line function definitions e.g.

				 int foo (int bar,
					 int boo);

				or

					int
					foo (int bar, int boo);

				etc.
		  */

			SetDocumentToParse (parser_p, handle_p);

			while ((count = GetNextPrototype (parser_p, &full_prototype_s)) > 0)
				{
					STRPTR temp_s = full_prototype_s;

					DB (KPRINTF ("%s %ld - GetMatchingPrototypes: line \"%s\"\n", __FILE__, __LINE__, full_prototype_s));

					while (*temp_s && (isspace (*temp_s) != 0))
						{
							++temp_s;
						}

					/*
					if (*temp_s)
						{
							-- temp_s;
						}
					*/

					if (IDOS->CapturePattern (pattern_s, temp_s, TRUE, &capture_p) != 0)
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

									fn_def_p = TokenizeFunctionPrototype (prototype_s, filename_s, parser_p -> dp_line_number);

									if (fn_def_p)
										{
											/* Add the prototype */

											if (AddFunctionDefinitionToList (fn_def_p, function_defs_p))
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
							DB (KPRINTF ("%s %ld - no match line:= \"%s\"\n", __FILE__, __LINE__, temp_s));
						}

					IExec->FreeVec (full_prototype_s);
					full_prototype_s = NULL;
				}

			success_flag = (count == 0);

			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
		}


	DB (KPRINTF ("%s %ld - GetMatchingPrototypes %ld\n", __FILE__, __LINE__, success_flag));

	LEAVE ();
	return success_flag;
}


void ClearCapturedExpression (struct CapturedExpression *capture_p)
{
	struct CapturedExpression *next_p;

	ENTER ();

	while (capture_p)
		{
			next_p = capture_p -> cape_Next;
			IExec->FreeVec (capture_p);

			capture_p = next_p;
		}

	memset (capture_p, 0, sizeof (struct CapturedExpression));

	LEAVE ();
}


BOOL ParseFile (CONST_STRPTR function_regexp_s, CONST_STRPTR filename_s, struct List *function_defs_p, struct DocumentParser *document_parser_p)
{
	ENTER ();
	BOOL success_flag = GetMatchingPrototypes (filename_s, function_regexp_s, document_parser_p, function_defs_p);

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

	DB (KPRINTF ("%s %ld - ParseFile %ld\n", __FILE__, __LINE__, success_flag));

	LEAVE ();
	return success_flag;
}


STRPTR MakePrototypePattern (CONST_STRPTR pattern_s)
{
	STRPTR prototype_pattern_s = NULL;

	ENTER ();

	if (pattern_s)
		{
			size_t l = strlen (pattern_s);
			CONST_STRPTR prefix_s = "#?";
			CONST_STRPTR suffix_s = "{#?}";
			const size_t prefix_length = strlen (prefix_s);
			const size_t suffix_length = strlen (suffix_s);

			prototype_pattern_s = (STRPTR) IExec->AllocVecTags (l + prefix_length + suffix_length + 1);

			if (prototype_pattern_s)
				{
					STRPTR temp_p = prototype_pattern_s;

					IExec->CopyMem (prefix_s, temp_p, prefix_length);
					temp_p += prefix_length;
					IExec->CopyMem (pattern_s, temp_p, l);
					temp_p += l;
					IExec->CopyMem (suffix_s, temp_p, suffix_length);
					temp_p += suffix_length;
					*temp_p = '\0';
				}
		}

	LEAVE ();
	return prototype_pattern_s;
}


static BOOL OpenLibs (void)
{
	ENTER ();

	if (OpenLib (&DOSBase, "dos.library", 52L, (struct Interface **) &IDOS, "main", 1))
		{
			if (OpenLib (&UtilityBase, "utility.library", 52L, (struct Interface **) &IUtility, "main", 1))
				{
					return TRUE;
				}

			CloseLib (DOSBase, (struct Interface *) IDOS);
		}

	LEAVE ();
	return FALSE;
}


static void CloseLibs (void)
{
	ENTER ();

	CloseLib (UtilityBase, (struct Interface *) IUtility);
	CloseLib (DOSBase, (struct Interface *) IDOS);

	LEAVE ();
}
