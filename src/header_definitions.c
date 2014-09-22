#include <proto/exec.h>

#include <string.h>

#include "header_definitions.h"


struct HeaderDefinitions *AllocateHeaderDefinitions (STRPTR filename_s)
{
	struct HeaderDefinitions *hdr_defs_p = (struct HeaderDefinitions *) IExec->AllocVecTags (sizeof (struct HeaderDefinitions), TAG_END);
	
	if (hdr_defs_p)
		{
			STRPTR name_s = strdup (filename_s);
			
			if (name_s)
				{
					hdr_defs_p -> hd_filename_s = name_s;
					
					IExec->NewList (& (hdr_defs_p -> hd_function_definitions));
				}
			else
				{
					IExec->FreeVec (hdr_defs_p);
					hdr_defs_p = NULL;
				}
		}

	return hdr_defs_p;
}


void FreeHeaderDefinitions (struct HeaderDefinitions *header_defs_p)
{
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
		}

	return success_flag;
}

struct HeaderDefinitionsNode *AllocateHeaderDefinitionsNode (struct HeaderDefinitions *hdr_defs_p)
{
	struct HeaderDefinitionsNode *node_p = (struct HeaderDefinitionsNode *) IExec->AllocVecTags (sizeof (struct HeaderDefinitionsNode), TAG_END);
	
	if (node_p)
		{
			node_p -> hdn_defs_p = hdr_defs_p;
		}
		
	return node_p;
}


void FreeHeaderDefinitionsNode (struct HeaderDefinitionsNode *node_p)
{
	FreeHeaderDefinitions (node_p -> hdn_defs_p);
	IExec->FreeVec (node_p);
}


BOOL AddFunctionDefinitionToList (struct HeaderDefinitions *header_defs_p, struct FunctionDefinition *fd_p)
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

