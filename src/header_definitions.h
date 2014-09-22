#ifndef HEADER_DEFINITIONS_H
#define HEADER_DEFINITIONS_H

#include <exec/lists.h>
#include <exec/types.h>

/**
 * All of the functions defined in a given header file.
 */
struct HeaderDefinitions
{
	/** The name of the file. */
	STRPTR hd_filename_s;
	
	/** 
	 * List of FunctionDefinitionNodes from this file.
	 */
	struct List hd_function_definitions;
};

struct HeaderDefinitionsNode 
{
	struct Node hdn_node;
	struct HeaderDefinitions *hdn_defs_p;
};



struct HeaderDefinitions *AllocateHeaderDefinitions (STRPTR filename_s);

void FreeHeaderDefinitions (struct HeaderDefinitions *header_defs_p);

struct HeaderDefinitionsNode *AllocateHeaderDefinitionsNode (HeaderDefinitions *hdr_defs_p);

void FreeHeaderDefinitionsNode (struct HeaderDefinitionsNode *node_p);




#endif		/* #ifndef HEADER_DEFINITIONS_H */
