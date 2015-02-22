#ifndef HEADER_DEFINITIONS_H
#define HEADER_DEFINITIONS_H

#include <exec/lists.h>
#include <exec/types.h>

#include "function_definition.h"


enum InterfaceFlag
{
	IF_PUBLIC,
	IF_PROTECTED,
	IF_PRIVATE
};

/**
 * All of the functions defined in a given header file.
 */
struct HeaderDefinitions
{
	/** The name of the file. */
	STRPTR hd_filename_s;

	uint32 hd_num_files;
};

struct HeaderDefinitionsNode
{
	struct Node hdn_node;
	struct HeaderDefinitions *hdn_defs_p;
};


void ClearHeaderDefinitionsList (struct List *hdr_defs_list_p);


BOOL AddHeaderDefintionsToList (struct List *hdr_defs_list_p, struct HeaderDefinitions *hdr_defs_p);

struct HeaderDefinitions *AllocateHeaderDefinitions (STRPTR filename_s);

void FreeHeaderDefinitions (struct HeaderDefinitions *header_defs_p);

struct HeaderDefinitionsNode *AllocateHeaderDefinitionsNode (struct HeaderDefinitions *hdr_defs_p);

void FreeHeaderDefinitionsNode (struct HeaderDefinitionsNode *node_p);

BOOL AddFunctionDefinitionToHeaderDefinitions (struct HeaderDefinitions *header_defs_p, struct FunctionDefinition *fd_p);

BOOL HasHeaderDefinitions (const struct HeaderDefinitions *header_defs_p);


uint32 GetHeaderDefinitionsListSize (struct List * const list_p);

void ClearHeaderDefinitionsList (struct List *header_definitions_p);


BOOL WriteSourceForHeaderDefinitions (const struct HeaderDefinitions *hdr_defs_p, CONST_STRPTR output_dir_s, CONST_STRPTR library_s, BPTR makefile_p);

BOOL WriteSourceForAllHeaderDefinitions (struct List *hdr_defs_list_p, CONST_STRPTR output_dir_s, CONST_STRPTR library_s);


#endif		/* #ifndef HEADER_DEFINITIONS_H */
