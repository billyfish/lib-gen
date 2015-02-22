#include <proto/dos.h>
#include <proto/exec.h>

#include <stdlib.h>
#include <string.h>

#include "header_definitions.h"
#include "function_definition.h"
#include "debugging_utils.h"
#include "utils.h"
#include "makefile_utils.h"

struct HeaderDefinitions *AllocateHeaderDefinitions (STRPTR filename_s)
{
	struct HeaderDefinitions *hdr_defs_p = NULL;
	STRPTR name_s = strdup (filename_s);

	if (name_s)
		{
			hdr_defs_p = (struct HeaderDefinitions *) IExec->AllocVecTags (sizeof (struct HeaderDefinitions), TAG_END);

			if (hdr_defs_p)
				{
					hdr_defs_p -> hd_filename_s = name_s;
					IExec->NewList (& (hdr_defs_p -> hd_function_definitions));
				}
			else
				{
					free (name_s);
				}
		}

	return hdr_defs_p;
}


void FreeHeaderDefinitions (struct HeaderDefinitions *header_defs_p)
{
	struct FunctionDefinitionNode *curr_node_p = (struct FunctionDefinitionNode *) IExec->GetHead (& (header_defs_p -> hd_function_definitions));
	struct FunctionDefinitionNode *next_node_p = NULL;

	while (curr_node_p != NULL)
		{
			next_node_p = (struct FunctionDefinitionNode *) IExec->GetSucc (& (curr_node_p -> fdn_node));
			FreeFunctionDefinitionNode (curr_node_p);
			curr_node_p = next_node_p;
		}


	free (header_defs_p -> hd_filename_s);

	IExec->FreeVec (header_defs_p);
}



BOOL AddHeaderDefintionsToList (struct List *hdr_defs_list_p, struct HeaderDefinitions *hdr_defs_p)
{
	BOOL success_flag = FALSE;
	struct HeaderDefinitionsNode *node_p = AllocateHeaderDefinitionsNode (hdr_defs_p);

	if (node_p)
		{
			IExec->AddTail (hdr_defs_list_p, (struct Node *) node_p);
			success_flag = TRUE;
		}

	return success_flag;
}

struct HeaderDefinitionsNode *AllocateHeaderDefinitionsNode (struct HeaderDefinitions *hdr_defs_p)
{
	struct HeaderDefinitionsNode *node_p = (struct HeaderDefinitionsNode *) IExec->AllocVecTags (sizeof (struct HeaderDefinitionsNode), TAG_END\);

	if (node_p)
		{
			node_p -> hdn_defs_p = hdr_defs_p;
			node_p -> hdn_node.ln_Pred = NULL;
			node_p -> hdn_node.ln_Succ = NULL;
		}

	return node_p;
}


void FreeHeaderDefinitionsNode (struct HeaderDefinitionsNode *node_p)
{
	FreeHeaderDefinitions (node_p -> hdn_defs_p);
	IExec->FreeVec (node_p);
}


uint32 GetHeaderDefinitionsListSize (struct List * const list_p)
{
	uint32 i = 0;
	struct HeaderDefinitionsNode *node_p = NULL;

	for (node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (list_p); node_p != NULL; node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc ((struct Node *) node_p))
		{
			++ i;
			DB (KPRINTF ("%s %ld - GetHeaderDefinitionsListSize i: %lu node filename %s\n", __FILE__, __LINE__, i, node_p -> hdn_defs_p -> hd_filename_s));
		}

	return i;
}


BOOL AddFunctionDefinitionToHeaderDefinitions (struct HeaderDefinitions *header_defs_p, struct FunctionDefinition *fd_p)
{
	BOOL success_flag = FALSE;

	struct FunctionDefinitionNode *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
		ASONODE_Size, sizeof (struct FunctionDefinitionNode),
		TAG_DONE);

	if (node_p)
		{
			node_p -> fdn_function_def_p = fd_p;

			IExec->AddTail (& (header_defs_p -> hd_function_definitions), (struct Node *) node_p);
			success_flag = TRUE;
		}

	return success_flag;
}


BOOL HasHeaderDefinitions (const struct HeaderDefinitions *hdr_defs_p)
{
	return (!IsListEmpty (& (hdr_defs_p -> hd_function_definitions)));
}


void ClearHeaderDefinitionsList (struct List *hdr_defs_list_p)
{
	struct HeaderDefinitionsNode *curr_node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (hdr_defs_list_p);
	struct HeaderDefinitionsNode *next_node_p = NULL;

	while (curr_node_p != NULL)
		{
			next_node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc (& (curr_node_p -> hdn_node));

			FreeHeaderDefinitionsNode (curr_node_p);
			curr_node_p = next_node_p;
		}
}



static BOOL WriteIncludes (BPTR out_p, CONST_STRPTR header_name_s)
{
	return (IDOS->FPrintf (out_p, "#include \"%s\"\n\n", header_name_s) > 0);
}


static BOOL WriteFunctionImplementations (BPTR out_p, const struct HeaderDefinitions *hdr_defs_p, CONST_STRPTR library_s)
{
	BOOL success_flag = TRUE;

	if (HasHeaderDefinitions (hdr_defs_p))
		{
			struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (& (hdr_defs_p -> hd_function_definitions));

			while ((node_p != NULL) && success_flag)
				{
					if (WriteLibraryFunctionImplementation (out_p, node_p -> fdn_function_def_p, library_s))
						{
							node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
						}
					else
						{
							success_flag = FALSE;
						}
				}
		}

	return success_flag;
}


BOOL WriteSourceForAllHeaderDefinitions (struct List *hdr_defs_list_p, CONST_STRPTR output_dir_s, CONST_STRPTR library_s)
{
	BOOL success_flag = TRUE;
	struct HeaderDefinitionsNode *node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (hdr_defs_list_p);
	BPTR makefile_p = GetMakefileHandle (library_s);

	while (node_p && success_flag)
		{
			success_flag  = WriteSourceForHeaderDefinitions (node_p -> hdn_defs_p, output_dir_s, library_s, makefile_p);

			if (success_flag)
				{
					node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc ((struct Node *) node_p);
				}
		}

	if (makefile_p)
		{
			if (!WriteMakefileFooter (makefile_p))
				{
					IDOS->Printf ("Failed to write footer block to makefile\n");
				}

			if (!CloseMakefile (makefile_p))
				{
					IDOS->Printf ("Failed to close makefile\n");
				}
		}

	return success_flag;
}


BOOL WriteSourceForHeaderDefinitions (const struct HeaderDefinitions *hdr_defs_p, CONST_STRPTR output_dir_s, CONST_STRPTR library_s, BPTR makefile_p)
{
	BOOL success_flag = FALSE;

	if (HasHeaderDefinitions (hdr_defs_p))
		{
			/* Get the .c filename */
			STRPTR filename_s = strdup (IDOS->FilePart (hdr_defs_p -> hd_filename_s));

			if (filename_s)
				{
					STRPTR suffix_p = strrchr (filename_s, '.');

					if (suffix_p)
						{
							++ suffix_p;

							if (*suffix_p != '\0')
								{
									STRPTR full_name_s = NULL;

									*suffix_p = 'c';
									* (++ suffix_p) = '\0';

									/* Make the full filename */
									/* @TODO Make sure output dir already exists */
									full_name_s = MakeFilename (output_dir_s, filename_s);

									if (full_name_s)
										{
											BPTR c_file_p = IDOS->FOpen (full_name_s, MODE_NEWFILE, 0);

											DB (KPRINTF ("%s %ld - Opened source file %s (%lu)\n", __FILE__, __LINE__, full_name_s, (uint32) c_file_p));

											if (c_file_p)
												{
													if (WriteIncludes (c_file_p, hdr_defs_p -> hd_filename_s))
														{
															if (WriteFunctionImplementations (c_file_p, hdr_defs_p, library_s))
																{
																	if (!AddFileToMakefileSources (makefile_p, full_name_s))
																		{
																			IDOS->Printf ("Failed to add %s to list of sources in makefile\n", full_name_s);
																		}

																	success_flag = TRUE;
																}
															else
																{
																	DB (KPRINTF ("%s %ld - Failed to write implementation to %s\n", __FILE__, __LINE__, full_name_s));
																}

														}		/* if (WriteIncludes (c_file_p, hdr_defs_p -> hd_filename_s)) */
													else
														{
															DB (KPRINTF ("%s %ld - Failed to write includes to %s\n", __FILE__, __LINE__, full_name_s));
														}

													IDOS->FClose (c_file_p);
												}		/* if (c_file_p) */

											IExec->FreeVec (full_name_s);
										}		/* if (full_name_s) */

								}		/* if (*suffix_p != '\0') */

						}		/* if (suffix_p) */

					free (filename_s);
				}		/* if (filename_s) */
		}
	else
		{
			success_flag = TRUE;
		}

	return success_flag;
}





