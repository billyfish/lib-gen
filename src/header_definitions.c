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
	ENTER ();

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

	LEAVE ();
	return hdr_defs_p;
}


BOOL AddHeaderDefintionsToList (struct List *hdr_defs_list_p, struct HeaderDefinitions *hdr_defs_p)
{
	ENTER ();

	BOOL success_flag = FALSE;
	struct HeaderDefinitionsNode *node_p = AllocateHeaderDefinitionsNode (hdr_defs_p);

	if (node_p)
		{
			IExec->AddTail (hdr_defs_list_p, (struct Node *) node_p);
			success_flag = TRUE;
		}

	LEAVE ();
	return success_flag;
}

struct HeaderDefinitionsNode *AllocateHeaderDefinitionsNode (struct HeaderDefinitions *hdr_defs_p)
{
	ENTER ();

	struct HeaderDefinitionsNode *node_p = (struct HeaderDefinitionsNode *) IExec->AllocVecTags (sizeof (struct HeaderDefinitionsNode), TAG_END);

	if (node_p)
		{
			node_p -> hdn_defs_p = hdr_defs_p;
			node_p -> hdn_node.ln_Pred = NULL;
			node_p -> hdn_node.ln_Succ = NULL;
		}

	LEAVE ();

	return node_p;
}


void FreeHeaderDefinitionsNode (struct HeaderDefinitionsNode *node_p)
{
	ENTER ();

	FreeHeaderDefinitions (node_p -> hdn_defs_p);
	IExec->FreeVec (node_p);

	LEAVE ();

}


BOOL AddFunctionDefinitionToHeaderDefinitions (struct HeaderDefinitions *header_defs_p, struct FunctionDefinition *fd_p)
{
	ENTER ();

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

	LEAVE ();
	return success_flag;
}


BOOL HasHeaderDefinitions (const struct HeaderDefinitions *hdr_defs_p)
{
	ENTER ();

	LEAVE ();

	return (!IsListEmpty (& (hdr_defs_p -> hd_function_definitions)));
}


void ClearHeaderDefinitionsList (struct List *hdr_defs_list_p)
{
	ENTER ();

	struct HeaderDefinitionsNode *curr_node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (hdr_defs_list_p);
	struct HeaderDefinitionsNode *next_node_p = NULL;


	while (curr_node_p != NULL)
		{
			next_node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc (& (curr_node_p -> hdn_node));

			FreeHeaderDefinitionsNode (curr_node_p);
			curr_node_p = next_node_p;
		}

	LEAVE ();
}
