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
#include "auto_init_writer.h"
#include "init_writer.h"
#include "proto_header_writer.h"
#include "interface_h_writer.h"
#include "library_utils.h"
#include "list_utils.h"
#include "makefile_writer.h"
#include "vectors.h"
#include "prefs.h"
#include "gui.h"

/* from ctags */
#include "main.h"


static BOOL OpenLibs (void);
static void CloseLibs (void);


BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, struct DocumentParser *document_parser_p, struct List *function_defs_p);
BOOL ParseFile (CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct List *function_defs_p, struct DocumentParser *document_parser_p, struct List *functions_to_ignore_p);
struct List *GeneratePrototypesList (CONST CONST_STRPTR root_path_s, CONST CONST_STRPTR filename_regexp_s, CONST CONST_STRPTR prototype_regexp_s, CONST BOOL recurse_flag, struct List *function_definitions_p, struct List *paths_to_ignore_p, struct List *functions_to_ignore_p);

void ClearCapturedExpression (struct CapturedExpression *capture_p);

int Run (LibGenPrefs *prefs_p, STRPTR prefix_s);

STRPTR MakePrototypePattern (CONST_STRPTR pattern_s);

struct List *GetFilesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_regexp_s, const BOOL recurse_flag, struct List *paths_to_ignore_p);

static void ClearList (struct List *list_p, BOOL free_list_flag);

static struct List *ParsePaths (CONST_STRPTR root_path_s, CONST_STRPTR paths_s);

BOOL WriteHeaderFiles (struct List *function_defs_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s);

static BOOL ReadCTagsFile (CONST_STRPTR ctags_file_s, CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct List *function_defs_p, struct List *functions_to_ignore_p);

static BOOL ReadWindowsExportsFile (CONST_STRPTR filename_s, struct List *function_defs_p);

static BOOL FillInPrefs (LibGenPrefs *prefs_p, const int32 * const args_p);


static STRPTR GetPrefixString (CONST_STRPTR value_s);

static void DebugArgs (const int32 * const args_p);


enum Args
{
	AR_HEADER_INPUT_DIR,
	AR_SOURCE_INPUT_DIR,
	AR_RECURSE,
	AR_LIBRARY_NAME,
	AR_INPUT_HEADER_FILE_PATTERN,
	AR_INPUT_SOURCE_FILE_PATTERN,
	AR_PROTOTYPE_PATTERN,
	AR_VERSION,
	AR_FLAGS,
	AR_GENERATE_CODE,

	/** The output format */
//	AR_OUTPUT_FORMAT,
	AR_VERBOSE,
	AR_NEWLIB,
	AR_ORDERING_FILENAME,
	AR_EXCLUDED_FUNCTIONS_FILENAME,
	AR_PATHS_TO_IGNORE,
	AR_NUM_ARGS
};





int main (int argc, char *argv [])
{
	ENTER ();

	int result = 0;

	if (OpenLibs ())
		{
			LibGenPrefs prefs;
			CONST_STRPTR paths_to_ignore_s = NULL;
			struct RDArgs *args_p = NULL;
																					
			InitLibGenPrefs (&prefs);
														
			if (argc == 1)
				{
					OpenPrefsGUI (&prefs);
				}		/* if (argc == 1) */
			else
				{
					int32 args [AR_NUM_ARGS];
					struct RDArgs *args_p = NULL;
					CONST_STRPTR args_s = "HI=HeadersInput/A,SI=SourceInput/K,R=Recurse/S,L=LibraryName/A,HP=HeaderFilePattern/K,SP=SourceFilePattern/K,PP=PrototypePattern/K,VER=Version/N,FL=Flags/K,GC=GenerateCode/S,V=Verbose/N,NL=Newlib/S,ORD=OrderingFile/K,EXC=ExcludeFile/K,IGN=Ignore/F";
					
					memset (args, 0, AR_NUM_ARGS * sizeof (int32));
						
					args_p = IDOS->ReadArgs (args_s, args, NULL);
		
					if (args_p != NULL)
						{							
							DebugArgs (args);
						
							if (FillInPrefs (&prefs, args))
								{
									if (GetVerbosity () >= VB_LOUD)
										{
											PrintPrefs (&prefs, IDOS -> Output ());
		
											switch (prefs.lgp_visibility_flag)
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
		
										}		/* if (GetVerbosity () >= VB_LOUD) */
								
								}		/* if (FillInPrefs (&prefs, args)) */
						}		/* if (args_p != NULL) */
					else
						{
							IDOS->Printf ("Reading args failed\n");
						}
				}		/* if (argc == 1) else ... */
		
			if (ArePrefsValid (&prefs))
				{
					STRPTR prefix_s = GetPrefixString (prefs.lgp_library_s);
					
					if (prefix_s)
						{
							struct List *paths_to_ignore_p = NULL;
							struct List *functions_to_ignore_p = NULL;

							if (paths_to_ignore_s)
								{
									paths_to_ignore_p = ParsePaths (prefs.lgp_source_input_dir_s, paths_to_ignore_s);
								}


							result = Run (&prefs, prefix_s);

							if (functions_to_ignore_p)
								{
									ClearList (functions_to_ignore_p, TRUE);
								}

							if (paths_to_ignore_p)
								{
									ClearList (paths_to_ignore_p, TRUE);
								}

							IExec->FreeVec (prefix_s);
						}

				}		/* if (ArePrefsValid (&prefs)) */
			else
				{
					PrintPrefs (&prefs, IDOS->Output ());
					
				}
				
			ClearLibGenPrefs (&prefs);

			DB (KPRINTF ("%s %ld - freeing args\n", __FILE__, __LINE__));
			if (args_p)
				{
					IDOS->FreeArgs (args_p);	
				}

			DB (KPRINTF ("%s %ld - closing libs\n", __FILE__, __LINE__));
			CloseLibs ();
		}
	else
		{
			printf ("no libs\n");
		}

	LEAVE ();
	return 0;
}





static struct List *ParsePaths (CONST_STRPTR root_path_s, CONST_STRPTR paths_s)
{
	struct List *paths_p = NULL;	
	
	ENTER ();

	paths_p = IExec->AllocSysObjectTags (ASOT_LIST, TAG_DONE);
	
	if (paths_p)
		{
			CONST_STRPTR start_p = paths_s;
			CONST_STRPTR token_p = paths_s;
			BOOL in_quotes_flag = FALSE;
			BOOL loop_flag = TRUE;
			BOOL success_flag = TRUE;
			BOOL in_whitespace_flag = FALSE;
			
			/*
				test "foo bar" trump toot
				
				Ignoring path: "test"
				Ignoring path: "foo bar"
				Ignoring path: "foo bar""
				Ignoring path: "foo bar" trump"
			*/
			
			while (loop_flag && success_flag)
				{
					STRPTR path_s = NULL;
					
					if (*token_p == '\"')
						{
							if (in_quotes_flag)
								{
									path_s = CopyToNewString (start_p, token_p - 1, FALSE);
									
									if (path_s)
										{
											start_p = NULL;
										}
									else
										{
											DB (KPRINTF ("%s %ld - Failed to copy string starting from \"%s\" to \"%s\"\n", __FILE__, __LINE__, start_p, token_p));
											success_flag = FALSE;
										}
									
									in_quotes_flag = FALSE;
								}	
							else
								{
									start_p = token_p + 1;
									
									if (*start_p == '\0')
										{
											DB (KPRINTF ("%s %ld - Error with \" after \"%s\"\n", __FILE__, __LINE__, token_p));
											success_flag = FALSE;
										}	
										
									in_quotes_flag = TRUE;
								}
						}		/* f (*token_p == '\"') */
					else
						{
							if (!in_quotes_flag)
								{
									if (isspace (*token_p))
										{
											if (!in_whitespace_flag)
												{
													if (start_p)
														{
															path_s = CopyToNewString (start_p, token_p - 1, FALSE);
															
															if (path_s)
																{
																	start_p = NULL;
																}
															else
																{
																	DB (KPRINTF ("%s %ld - Failed to copy string starting from \"%s\" to \"%s\"\n", __FILE__, __LINE__, start_p, token_p));
																	success_flag = FALSE;
																}													
														}
														
													in_whitespace_flag = TRUE;
												}
										}
									else
										{
											if (in_whitespace_flag)
												{
													start_p = token_p;
													in_whitespace_flag = FALSE;
												}
										}
								}
						}
						

					/* increment the pointer through the paths */
					if (success_flag)
						{
							++ token_p;
							
							if (*token_p == '\0')
								{
									if (start_p)
										{
											path_s = CopyToNewString (start_p, token_p - 1, FALSE);
										}
									
									if (!path_s)
										{
											DB (KPRINTF ("%s %ld - Failed to copy string starting from \"%s\" to \"%s\"\n", __FILE__, __LINE__, start_p, token_p));
											success_flag = FALSE;
										}	
									
									loop_flag = FALSE;	
								}		
						}
				
					if (path_s)
						{
							STRPTR full_filename_s = MakeFilename (root_path_s, path_s);
							
							if (full_filename_s)
								{
									struct Node *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
										ASONODE_Name, full_filename_s,
										ASONODE_Size, sizeof (struct Node),
										TAG_DONE);
					
									if (node_p)
										{
											IExec->AddTail (paths_p, node_p);
											success_flag = TRUE;
										}						
									else
										{
											DB (KPRINTF ("%s %ld - Failed to allocate node for \"%s\"\n", __FILE__, __LINE__, full_filename_s));
											IExec->FreeVec (full_filename_s);
											
											success_flag = FALSE;								
										}						
														
								}		/* if (full_filename_s) */
							else
								{
									IDOS->Printf ("Failed to make ignore path for \"%s\" and \"%s\" \n", __FILE__, __LINE__, root_path_s, path_s);											
									success_flag = FALSE;			
								}							
							
							IExec->FreeVec (path_s);
						}
	
				}		/* hile (loop_flag && success_flag) */
			
			if (GetVerbosity () >= VB_LOUD)
				{
					struct Node *curr_node_p = IExec->GetHead (paths_p);
				
					while (curr_node_p)
						{
							struct Node *next_node_p = IExec->GetSucc (curr_node_p);
							
							IDOS->Printf ("Ignoring path: \"%s\"\n", curr_node_p -> ln_Name);
														
							curr_node_p = next_node_p;
						}		/* while (curr_node_p) */
				}
			
		}		/* if (paths_p) */
	else
		{
			DB (KPRINTF ("%s %ld - Failed to allocate list for ParsePaths\n", __FILE__, __LINE__));
		}
		
	LEAVE ();

	return paths_p;
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





int Run (LibGenPrefs *prefs_p, STRPTR prefix_s)
{
	int res = 0;
	BOOL success_flag = FALSE;
	
	/* List of FunctionDefinitionsNodes */
	struct List function_defs;


	struct List *header_filenames_p = NULL;
	/*
		List of the ordering of a previous version
		of the library so that we remain compatible
		e.g. the LVOs remain the same with any new
		functions added at the end
	*/
	struct List previous_fns_ordering_list;

	const enum Verbosity verbosity = GetVerbosity ();

	ENTER ();


	IExec->NewList (&function_defs);
	IExec->NewList (&previous_fns_ordering_list);
	

	if (verbosity >= VB_LOUDER)
		{			
//			IDOS->Printf ("source pattern \"%s\" regexp \"%s\"\n", prefs_p -> lgp_source_filename_pattern_s, source_filename_regexp_s);
//			IDOS->Printf ("header pattern \"%s\" regexp \"%s\"\n", header_filename_pattern_s, header_filename_regexp_s);
		}
			
	if ((header_filenames_p = GeneratePrototypesList (prefs_p -> lgp_header_input_dir_s, prefs_p -> lgp_header_filename_regexp_s, prefs_p -> lgp_prototype_regexp_s, prefs_p -> lgp_recurse_flag, &function_defs, prefs_p -> lgp_paths_to_ignore_p, prefs_p -> lgp_functions_to_ignore_p)) != NULL)
		{
			struct List *source_files_p = GetFilesList (prefs_p -> lgp_source_input_dir_s, prefs_p -> lgp_source_filename_regexp_s, prefs_p -> lgp_recurse_flag, prefs_p -> lgp_paths_to_ignore_p);
			
			if (source_files_p)
				{
					Writer *writer_p = AllocateIDLWriter ();

					DB (KPRINTF ("%s %ld - writer %ld", __FILE__, __LINE__, writer_p));
		
					if (writer_p)
						{
							STRPTR output_s = ConcatenateStrings (prefs_p -> lgp_library_s, GetWriterFileSuffix (writer_p));
		
							DB (KPRINTF ("%s %ld - output_s %s", __FILE__, __LINE__, output_s));
		
							if (output_s)
								{
									BPTR out_p = IDOS->FOpen (output_s, MODE_NEWFILE, 0);
		
									if (out_p)
										{
											success_flag = TRUE;
											
											DB (KPRINTF ("%s %ld - opened output_s %s", __FILE__, __LINE__, output_s));
		
											if (verbosity >= VB_NORMAL)
												{
													IDOS->Printf ("Found %lu functions\n", GetListSize (&function_defs));
												}
												
											
											/*
											 * Get any previous ordering file 
											 */ 
											if (prefs_p -> lgp_defs_filename_s)
												{	
													ClearAllFunctionDefintionExportFlags (&function_defs);
													
													if (ReadWindowsExportsFile (prefs_p -> lgp_defs_filename_s, &function_defs))
														{
															if (verbosity >= VB_LOUDER)
																{
																	PrintAllFunctionDefinitions (&function_defs, IDOS->Output ());
																}
														}
													else
														{
															IDOS->Printf ("Failed to read exported function names from  %s\n", prefs_p -> lgp_defs_filename_s);
														}					
												}		/* if (defs_filename_s) */	
												
											if (success_flag)
												{
													if (WriteFunctionDefinitionsList (writer_p, &function_defs, prefs_p -> lgp_library_s, prefix_s, prefs_p -> lgp_version, prefs_p -> lgp_visibility_flag, out_p))
														{
															STRPTR makefile_s = ConcatenateStrings (prefs_p -> lgp_library_s, ".makefile");
				
															if (verbosity >= VB_NORMAL)
																{
																	IDOS->Printf ("Successfully wrote header definitions to %s\n", output_s);
																}		
				
															if (makefile_s)
																{
																	if (WriteMakefile (makefile_s, prefs_p -> lgp_source_input_dir_s, prefs_p -> lgp_library_s, &function_defs, source_files_p))
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
						
					ClearList (source_files_p, TRUE);
				}		/* if (source_files_p) */
		
		}		/* if (GeneratePrototypesList (root_path_s, header_filename_regexp_s, prototype_regexp_s, recurse_flag, &function_defs)) */


	if (success_flag && (prefs_p -> lgp_generate_code_flag))
		{
			STRPTR output_dir_s = ConcatenateStrings (prefs_p -> lgp_library_s, SOURCE_DIR_SUFFIX_S);

			if (output_dir_s)
				{
					if (EnsureDirectoryExists (output_dir_s))
						{
							if (WriteSourceForAllFunctionDefinitions (&function_defs, output_dir_s, prefs_p -> lgp_library_s, prefix_s))
								{	
									if (verbosity >= VB_NORMAL)
										{
											IDOS->Printf ("Generating source succeeded\n");
										}	
						
									if (WriteSourceForAllFunctionDeclarations (&function_defs, output_dir_s, prefs_p -> lgp_library_s, prefix_s))
										{
											if (verbosity >= VB_NORMAL)
												{
													IDOS->Printf ("Generating headers succeeded\n");
												}	
								
											if (WriteInitFiles (prefs_p -> lgp_library_s, output_dir_s))
												{													
													if (WriteAutoInitFiles (prefs_p -> lgp_library_s, output_dir_s))
														{
															if (WriteVectorsFile (output_dir_s, prefs_p -> lgp_library_s, prefix_s, &function_defs))
																{
																	if (WriteHeaderFiles (&function_defs, prefs_p -> lgp_library_s, output_dir_s))
																		{						
																			STRPTR init_s = NULL;
								
																			if (verbosity >= VB_NORMAL)
																				{
																					IDOS->Printf ("Generating vectors succeeded\n");
																				}	
																			
																			init_s = MakeFilename (output_dir_s, "lib_init.c");
																			
																			
																			if (init_s)
																				{
																					/* Get the path to the libgen directory */
																					BOOL success_flag = FALSE;
																					BPTR lock_p = IDOS->GetProgramDir ();
																		
																					if (lock_p)
																						{
																							char buffer_s [4096] = "\0";
																							
																							int32 success = IDOS->NameFromLock (lock_p, buffer_s, 4095);
																							
																							if (success)
																								{
																									STRPTR full_filename_s = MakeFilename (buffer_s, "data/lib_manager.c.template");
																									
																									if (full_filename_s)
																										{
																											if (CopyFile (full_filename_s, init_s))
																												{
																													success_flag = TRUE;	
																												}
																										
																											IExec->FreeVec (full_filename_s);
																										}
																								}
																						}
																			
																					if (!success_flag)
																						{
																							IDOS->Printf ("Failed to generate lib_init.c");
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
														
														}
													else
														{
															IDOS->Printf ("Generating auto init files failed\n");
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

	if (header_filenames_p)
		{
			FreeList (header_filenames_p);	
		}

	LEAVE ();
	return res;
}


static void ClearList (struct List *list_p, BOOL free_list_flag)
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

	if (free_list_flag)
		{
			IExec->FreeVec (list_p);
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


static BOOL ReadWindowsExportsFile (CONST_STRPTR filename_s, struct List *function_defs_p)
{
	BOOL success_flag = FALSE;
	
	ENTER ();
	
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			struct FReadLineData *line_data_p = IDOS->AllocDosObjectTags (DOS_FREADLINEDATA, TAG_DONE);
			
			if (line_data_p)
				{
					int32 count = IDOS->FReadLine (handle_p, line_data_p);
					
					if (count > 0)
						{
							if (strstr (line_data_p -> frld_Line, "EXPORTS"))
								{
									int32 index = 0;
									success_flag = TRUE;
									
									while ((count = IDOS->FReadLine (handle_p, line_data_p)) > 0)
										{											
											STRPTR start_s = ScrollPastWhitespace (line_data_p -> frld_Line);
											
											/* 
											 * check line is valid and not a comment line which begins 
											 * with a ;
											 */
											if ((start_s != NULL) && (*start_s != '\0') && (*start_s != ';'))
												{
													BOOL loop_flag = TRUE;
													STRPTR end_s = start_s;
													STRPTR function_s = NULL;
													
													while (loop_flag)
														{
															++ end_s;	
															
															if (*end_s == '\0') 
																{
																	loop_flag = FALSE;	
																	-- end_s;
																}
															else if (isspace (*end_s))
																{
																	loop_flag = FALSE;	
																	-- end_s;
																}
														}
														
													function_s = CopyToNewString (start_s, end_s, FALSE);
													
													if (function_s)
														{
															struct FunctionDefinition *fn_def_p = GetNamedFunctionDefinition (function_defs_p, function_s);
															
															if (fn_def_p)
																{
																	fn_def_p -> fd_export_index = index;
																	DB (KPRINTF ("%s %ld - Setting function \"%s\" to index %ld\n", __FILE__, __LINE__, function_s, index));
																}
															else
																{
																	IDOS->Printf ("Failed to find exported function \"%s\" in parsed declarations\n", function_s);	
																}					
																										
															++ index;
															
															IExec->FreeVec (function_s);	
														}
													else
														{
															IDOS->Printf ("Failed to allocate memory for function name from \"%s\"\n", start_s);	
														}
													
												}
											
										}		/* while ((count = IDOS->FReadLine (handle_p, line_data_p)) > 0) */			
										
									if (index > 0)
										{
											SortFunctionDefinitions (function_defs_p, NULL);
										}
								}
								
						}		/* if (count > 0) */	
					
					IDOS->FreeDosObject (DOS_FREADLINEDATA, line_data_p);
				}		/* if (line_data_p) */

			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
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


						}		/* while ((count = IDOS->FReadLine (parser_p -> dp_file_handle_p, parser_p -> dp_line_p)) > 0) */

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



struct List *GeneratePrototypesList (CONST CONST_STRPTR root_path_s, CONST CONST_STRPTR filename_regexp_s, CONST CONST_STRPTR prototype_regexp_s, CONST BOOL recurse_flag, struct List *function_definitions_p, struct List *paths_to_ignore_p, struct List *functions_to_ignore_p)
{
	ENTER ();

	BOOL success_flag = FALSE;
	struct List *headers_p = GetFilesList (root_path_s, filename_regexp_s, recurse_flag, paths_to_ignore_p);

	if (headers_p)
		{
			uint32 num_header_files = GetListSize (headers_p);

			if (num_header_files > 0)
				{
					struct DocumentParser *document_parser_p = AllocateDocumentParser ();

					if (document_parser_p)
						{
							struct Node *node_p;
							enum Verbosity verbosity = GetVerbosity ();
							 
							success_flag = TRUE;
							
							if (verbosity >= VB_NORMAL)
								{
									IDOS->Printf ("Found %lu header files\n", num_header_files);
								}
								
							for (node_p = IExec->GetHead (headers_p); node_p != NULL; node_p = IExec->GetSucc (node_p))
								{
									CONST_STRPTR filename_s = node_p -> ln_Name;

									if (verbosity >= VB_NORMAL)
										{
											IDOS->Printf ("Parsing \"%s\"\n", filename_s);
										}

									/* Get the list of matching prototypes in each file */
									if (!ParseFile (prototype_regexp_s, filename_s, function_definitions_p, document_parser_p, functions_to_ignore_p))
										{
											IDOS->Printf ("Failed to parse \"%s\"\n", filename_s);
										}
								}

							if (verbosity >= VB_LOUD)
								{
									IDOS->Printf ("Finished parsing files");
								}

							FreeDocumentParser (document_parser_p);
						}
					else
						{

						}

				}		/* if (num_header_files > 0) */

			if (!success_flag)
				{
					FreeList (headers_p);
					headers_p = NULL;	
				}
		}		/* if (headers_p) */

	LEAVE ();
	return headers_p;
}



struct List *GetFilesList (CONST_STRPTR root_path_s, CONST_STRPTR filename_regexp_s, const BOOL recurse_flag, struct List *paths_to_ignore_p)
{
	ENTER ();

	struct List *filenames_p = IExec->AllocSysObjectTags (ASOT_LIST, TAG_DONE);

	if (filenames_p)
		{
			IDOS->Printf ("About to call ScanPath with root \"%s\"\n", root_path_s);
			if (!ScanPath (root_path_s, filenames_p, filename_regexp_s, recurse_flag, paths_to_ignore_p))
				{
					FreeList (filenames_p);
					filenames_p = NULL;
				}

		}		/* if (filenames_p) */

	LEAVE ();
	return filenames_p;
}


static BOOL ReadCTagsFile (CONST_STRPTR ctags_file_s, CONST_STRPTR pattern_s, CONST_STRPTR filename_s, struct List *function_defs_p, struct List *functions_to_ignore_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = ZERO;

	ENTER ();


	DB (KPRINTF ("%s %ld - ReadCTagsFile: About to open \"%s\"\n", __FILE__, __LINE__, ctags_file_s));

	handle_p = IDOS->FOpen (ctags_file_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			struct FReadLineData *line_p = IDOS->AllocDosObjectTags (DOS_FREADLINEDATA, TAG_DONE);
			
			if (line_p)
				{
					while (IDOS->FReadLine (handle_p, line_p) > 0)
						{
							
							DB (KPRINTF ("%s %ld - ReadCTagsFile: got linen \"%s\"\n", __FILE__, __LINE__, line_p -> frld_Line));
							/* ctags comments begin with a ! so make sure we haven't got one of those */
							if (* (line_p -> frld_Line) != '!')
								{
									/*
										json_array	/jansson-2.9/src/jansson.h	/^json_t *json_array(void);$/;"	p	line:85
										json_array_append	/jansson-2.9/src/jansson.h	/^int json_array_append(json_t *array, json_t *value)$/;"	f	line:214
										json_array_append_new	/jansson-2.9/src/jansson.h	/^int json_array_append_new(json_t *array, json_t *value);$/;"	p	line:201
								 	*/
									CONST_STRPTR start_marker_s = "/^";
								 	STRPTR start_s = strstr (line_p -> frld_Line, start_marker_s);
								 
								 	if (start_s)
								 		{
									 		CONST_STRPTR end_marker_s = "$/";								 		
									 		STRPTR end_s = strstr (line_p -> frld_Line, end_marker_s);
									 		
									 		if (end_s)
									 			{
													CONST_STRPTR line_marker_s = "line:";
											 		STRPTR line_s;
											 		
									 				/* 
									 					The protoype tokenizer depends upon a ; at the end of the prototype which 
									 					inline functions don't have so add it if it's needed.
									 				*/
									 				if (* (end_s - 1) == ')')
									 					{
									 						*end_s = ';';
									 					}
											 		
											 		start_s += strlen (start_marker_s);	
			
													line_s = strstr (start_s, line_marker_s);
													
													if (line_s)
														{
															int32 line_number = 0;
															
															line_s += strlen (line_marker_s);
															
															//IDOS->Printf ("start_s %s", start_s);
															//IDOS->Printf ("line_s \"%s\"\n", line_s);
															if (sscanf (line_s, "%ld", &line_number) == 1)
																{
																	int32 res = GetAndAddFunctionDefinitionFromString (start_s, filename_s, line_number, function_defs_p, functions_to_ignore_p);
																																		
																	if (res >= 0)
																		{
																			success_flag = TRUE;
																		}
																	else
																		{
																			IDOS->Printf ("Failed to add definition for \"%s\"\n", start_s);
																		}
																}
															
														}
									 				
									 			}		/* if (end_s) */
								 		
										}
								 
								}
						}
						
					IDOS->FreeDosObject (DOS_FREADLINEDATA, line_p);
				}
											
			IDOS->FClose (handle_p);
		}
		
		
	LEAVE ();
		
	return success_flag;
}


BOOL GetMatchingPrototypesWithCTags (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, struct DocumentParser *parser_p, struct List *function_defs_p, struct List *functions_to_ignore_p)
{
	BOOL success_flag = FALSE;
	struct ByteBuffer *buffer_p = NULL;
	
	ENTER ();	
		
	buffer_p = AllocateByteBuffer (256);
	
	if (buffer_p)
		{
			CONST_STRPTR local_filename_s = IDOS->FilePart (filename_s);			
			STRPTR tags_filename_s = MakeFilename ("T:", local_filename_s);
			
			if (tags_filename_s)
				{
					if (AppendStringsToByteBuffer (buffer_p, "ctags ", "-f ", tags_filename_s, " --fields=+n --c-kinds=+pf-dmstne ", filename_s, NULL))
						{
							int res;
							
							DB (KPRINTF ("%s %ld - GetMatchingPrototypesWithCTags: About to open \"%s\"\n", __FILE__, __LINE__, filename_s));							
							res = IDOS->System (buffer_p -> bb_data_p, NULL); //RunCTagsFromStringArgs (buffer_p -> bb_data_p);
							
							
							
							DB (KPRINTF ("%s %ld - GetMatchingPrototypesWithCTags: RunCTagsFromStringArgs returned \"%d\"\n", __FILE__, __LINE__, res));	
							
							if (res == 0)
								{
									success_flag = ReadCTagsFile (tags_filename_s, pattern_s, filename_s, function_defs_p, functions_to_ignore_p);
								}
							else
								{
									IDOS->Printf ("%s %ld - GetMatchingPrototypesWithCTags: RunCTagsFromStringArgs failed \"%s\"\n", __FILE__, __LINE__, buffer_p -> bb_data_p);	
								}
								
							//IDOS->Delete (tags_filename_s);
						}			
						
					IExec->FreeVec (tags_filename_s);		
				}
			else
				{
					DB (KPRINTF ("%s %ld - GetMatchingPrototypesWithCTags: RunCTagsFromStringArgs failed to create tag filename for \"%s\"\n", __FILE__, __LINE__, local_filename_s));	
				}
		
			FreeByteBuffer (buffer_p);	
		}
	
	
	
	LEAVE ();
	return success_flag;
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
									int8 tokenized_flag = 0;
									enum Verbosity v = GetVerbosity ();									
									//IDOS->Printf (">>> matched line:= %s", line_p -> frld_Line);

									/* For easier debugging, overwrite the \n with a \0 */
									size_t l = strlen (prototype_s);
									if (* (prototype_s + l - 1) == '\n')
										{
											* (prototype_s + l - 1) = '\0';
										}

									tokenized_flag = TokenizeFunctionPrototype (&fn_def_p, prototype_s, filename_s, parser_p -> dp_line_number);

									switch (tokenized_flag)
										{
											case 1:
												{													
													if (v >= VB_LOUDER)
														{
															IDOS->Printf ("Function definition for \"%s\" :: -> \n", prototype_s);
															BPTR out_p = IDOS->Output ();
															PrintFunctionDefinition (out_p, fn_def_p);
														}
													
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
												break;
												
											
											case 0:
												/* function callback ignore */
												break;
										
											case -1:
												if (v >= VB_LOUDER)
													{
														IDOS->Printf ("Could not parse \"%s\" as a function prototype\n", prototype_s);
													}	
												success_flag = FALSE;
												break;
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


BOOL ParseFile (CONST_STRPTR function_regexp_s, CONST_STRPTR filename_s, struct List *function_defs_p, struct DocumentParser *document_parser_p, struct List *functions_to_ignore_p)
{
	ENTER ();
	BOOL success_flag = GetMatchingPrototypesWithCTags (filename_s, function_regexp_s, document_parser_p, function_defs_p, functions_to_ignore_p);

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



static void DebugArgs (const int32 * const args_p)
{
	IDOS->Printf ("AR_HEADER_INPUT_DIR \"%s\"\n", (CONST_STRPTR) args_p [AR_HEADER_INPUT_DIR]);
	IDOS->Printf ("AR_SOURCE_INPUT_DIR \"%s\"\n", (CONST_STRPTR) args_p [AR_SOURCE_INPUT_DIR]);		
	IDOS->Printf ("AR_RECURSE \"%d\"\n",  args_p [AR_RECURSE]);
	IDOS->Printf ("AR_LIBRARY_NAME \"%s\"\n", (CONST_STRPTR) args_p [AR_LIBRARY_NAME]);
	IDOS->Printf ("AR_INPUT_HEADER_FILE_PATTERN \"%s\"\n", (CONST_STRPTR) args_p [AR_INPUT_HEADER_FILE_PATTERN]);
	IDOS->Printf ("AR_INPUT_SOURCE_FILE_PATTERN \"%s\"\n", (CONST_STRPTR) args_p [AR_INPUT_SOURCE_FILE_PATTERN]);		
	IDOS->Printf ("AR_PROTOTYPE_PATTERN \"%s\"\n", (CONST_STRPTR) args_p [AR_PROTOTYPE_PATTERN]);	
	IDOS->Printf ("AR_VERSION \"%d\"\n",  args_p [AR_VERSION]);
	IDOS->Printf ("AR_FLAGS \"%s\"\n", (CONST_STRPTR) args_p [AR_FLAGS]);
	IDOS->Printf ("AR_GENERATE_CODE \"%d\"\n", args_p [AR_GENERATE_CODE]);
	IDOS->Printf ("AR_VERBOSE \"%d\"\n", args_p [AR_VERBOSE]);		
	IDOS->Printf ("AR_NEWLIB \"%d\"\n",  args_p [AR_NEWLIB]);
	IDOS->Printf ("AR_ORDERING_FILENAME \"%s\"\n", (CONST_STRPTR) args_p [AR_ORDERING_FILENAME]);
	IDOS->Printf ("AR_EXCLUDED_FUNCTIONS_FILENAME \"%s\"\n", (CONST_STRPTR) args_p [AR_EXCLUDED_FUNCTIONS_FILENAME]);
	IDOS->Printf ("AR_PATHS_TO_IGNORE \"%s\"\n", (CONST_STRPTR) args_p [AR_PATHS_TO_IGNORE]);		
} 


static BOOL FillInPrefs (LibGenPrefs *prefs_p, const int32 * const args_p)
{
	BOOL success_flag = TRUE;

	InitLibGenPrefs (prefs_p);

	if (args_p [AR_INPUT_HEADER_FILE_PATTERN])
		{
			if (!SetLibGenPrefsHeadersPattern (prefs_p, (CONST_STRPTR) args_p [AR_INPUT_HEADER_FILE_PATTERN]))
				{
					IDOS->Printf ("SetLibGenPrefsHeadersPattern failed for \"%s\"\n", (CONST_STRPTR) args_p [AR_INPUT_HEADER_FILE_PATTERN]);	
				}
		}

	if (success_flag)
		{				
			if (args_p [AR_INPUT_SOURCE_FILE_PATTERN])
				{
					if (!SetLibGenPrefsSourcesPattern (prefs_p, (CONST_STRPTR) args_p [AR_INPUT_SOURCE_FILE_PATTERN]))
						{
							IDOS->Printf ("SetLibGenPrefsSourcesPattern failed for \"%s\"\n", (CONST_STRPTR) args_p [AR_INPUT_SOURCE_FILE_PATTERN]);	
						}
				}

		}		/* if (success_flag) */

	if (success_flag)
		{
			if (args_p [AR_PROTOTYPE_PATTERN])
				{					
					if (!SetLibGenPrefsPrototypePattern (prefs_p, (CONST_STRPTR) args_p [AR_PROTOTYPE_PATTERN]))
						{
							IDOS->Printf ("SetLibGenPrefsPrototypePattern failed for \"%s\"\n", (CONST_STRPTR) args_p [AR_PROTOTYPE_PATTERN]);	
						}
				}

		}		/* if (success_flag) */


	prefs_p -> lgp_library_s = (CONST_STRPTR) args_p [AR_LIBRARY_NAME];
	//paths_to_ignore_s = (CONST_STRPTR) args_p [AR_PATHS_TO_IGNORE];


	prefs_p -> lgp_header_input_dir_s  = (CONST_STRPTR) args_p [AR_HEADER_INPUT_DIR];
	prefs_p -> lgp_source_input_dir_s  = (CONST_STRPTR) args_p [AR_SOURCE_INPUT_DIR];

	if (args_p [AR_RECURSE])
		{
			prefs_p -> lgp_recurse_flag = TRUE;
		}


	if (args_p [AR_ORDERING_FILENAME])
		{
			prefs_p -> lgp_defs_filename_s = (CONST_STRPTR) args_p [AR_ORDERING_FILENAME];
		}


	if (args_p [AR_EXCLUDED_FUNCTIONS_FILENAME])
		{
			//functions_to_ignore_filename_s = (CONST_STRPTR) args [AR_EXCLUDED_FUNCTIONS_FILENAME];
		}



	if (args_p [AR_FLAGS])
		{
			CONST_STRPTR value_s = (CONST_STRPTR) args_p [AR_FLAGS];

			if (IUtility->Stricmp (value_s, "private") == 0)
				{
					prefs_p -> lgp_visibility_flag = IF_PRIVATE;
				}
			else if (IUtility->Stricmp (value_s, "protected") == 0)
				{
					prefs_p -> lgp_visibility_flag = IF_PROTECTED;
				}
			else if (IUtility->Stricmp (value_s, "none") == 0)
				{
					prefs_p -> lgp_visibility_flag = IF_PUBLIC;
				}
			else
				{
					IDOS->Printf ("Ignoring invalid flag \"%s\", must be either private, protected or none", value_s);
				}
		}

	if (args_p [AR_VERSION])
		{
			prefs_p -> lgp_version = * ((int32 *) args_p [AR_VERSION]);
		}

	if (args_p [AR_VERBOSE])
		{
			SetVerbosity (* ((int32 *) args_p [AR_VERBOSE]));
		}
	else
		{
			SetVerbosity (VB_NORMAL);
		}


	if (args_p [AR_GENERATE_CODE])
		{
			prefs_p -> lgp_generate_code_flag = TRUE;
		}

	if (args_p [AR_NEWLIB])
		{
			SetNewlibNeeded (TRUE);
		}


	return success_flag;
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
