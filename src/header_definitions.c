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


struct HeaderDefinitionsNode *AllocateHeaderDefinitionsNode (HeaderDefinitions *hdr_defs_p)
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
