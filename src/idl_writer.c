#include <proto/exec.h>
#include <proto/dos.h>

#include <exec/types.h>

#include "idl_writer.h"
#include "memory.h"
#include "parameter.h"
#include "function_definition.h"
#include "header_definitions.h"
#include "debugging_utils.h"



static BOOL WriteIDL (struct Writer *writer_p, struct List *header_definitions_list_p, BPTR out_p);
static BOOL WriteIDLHeader (BPTR out_p, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s);
static BOOL WriteIDLFunctions (BPTR out_p, struct List * const fds_list_p);

static BOOL WriteIDLFunction (BPTR out_p, const struct FunctionDefinition * const fd_p);
static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p);

static BOOL WriteIDLDefaultFunctions (BPTR out_p);
static BOOL WriteIDLIncludes (BPTR out_p, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s);
static BOOL WriteIDLFooter (BPTR out_p);


static BOOL WriteIDLHeaderDefinitionsList (BPTR out_p, struct List * const header_definitions_list_p);

static BOOL WriteIDLHeaderDefinitions (BPTR out_p, struct HeaderDefinitions * const header_definitions_p);



Writer *AllocateIDLWriter (void)
{
	IDLWriter *idl_p = (IDLWriter *) AllocMemory (sizeof (IDLWriter));

	if (idl_p)
		{
			idl_p -> iw_base_writer.wr_write_header_definitions_list_fn = WriteIDL;
		}

	return ((Writer *) idl_p);
}


void FreeIDLWriter (Writer *writer_p)
{
	IDLWriter *idl_p = (IDLWriter *) writer_p;

	FreeMemory (idl_p);
}


static BOOL WriteIDL (struct Writer *writer_p, struct List *header_definitions_list_p, BPTR out_p)
{
	BOOL success_flag = FALSE;
	const char * const name_s = "name";
	const char * const basename_s = "basename";
	const char * const struct_name_s = "struct_name";
	const char * const prefix_s = "prefix";

	if (WriteIDLHeader (out_p, name_s, basename_s, struct_name_s, prefix_s))
		{
			if (WriteIDLDefaultFunctions (out_p))
				{
					if (WriteIDLHeaderDefinitionsList (out_p, header_definitions_list_p))
						{
							if (WriteIDLFooter (out_p))
								{
									success_flag = TRUE;
								}
							else
								{
									DB (KPRINTF ("%s %ld - Failed to write idl footer", __FILE__, __LINE__));
								}
						}
					else
						{
							DB (KPRINTF ("%s %ld - Failed to write idl header definitions\n", __FILE__, __LINE__));
						}
				}
			else
				{
					DB (KPRINTF ("%s %ld - Failed to write idl default_functions\n", __FILE__, __LINE__));
				}
		}		/* if (WriteIDLHeader (name_s, basename_s, struct_name_s, prefix_s)) */
	else
		{
			DB (KPRINTF ("%s %ld - Failed to write idl header\n", __FILE__, __LINE__));
		}

	return success_flag;
}





static BOOL WriteIDLHeader (BPTR out_p, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "<!DOCTYPE library SYSTEM \"library.dtd\">\n") >= 0)
				{
					if (IDOS->FPrintf (out_p, "<library name=\"%s\" basename=\"%s\">\n", name_s, basename_s) >= 0)
						{
							if (IDOS->FPrintf (out_p, "\t<interface name=\"%s\" version=\"1\" flags=\"protected\" struct=\"%s\" prefix=\"%s\">\n", name_s, struct_name_s, prefix_s) >= 0)
								{
									success_flag = TRUE;
								}
						}
				}
		}

	return success_flag;
}


static BOOL WriteIDLHeaderDefinitionsList (BPTR out_p, struct List * const header_definitions_list_p)
{
	BOOL success_flag = TRUE;
	struct HeaderDefinitionsNode *node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (header_definitions_list_p); 
	
	
	while ((node_p != NULL) && success_flag)	
		{
			if (WriteIDLHeaderDefinitions (out_p, node_p -> hdn_defs_p))
				{
					node_p = (struct HeaderDefinitionsNode *) IExec->GetSucc ((struct Node *) node_p);
				}
			else
				{
					success_flag = FALSE;
				}
		}


	return success_flag;
}




static BOOL WriteIDLHeaderDefinitions (BPTR out_p, struct HeaderDefinitions * const header_definitions_p)
{
	BOOL success_flag = FALSE;


	if (IDOS->FPrintf (out_p, "\n\t\t<-- %lu definitions in %s -->\n", GetFunctionDefinitionsListSize (& (header_definitions_p -> hd_function_definitions)), header_definitions_p -> hd_filename_s) >= 0)
		{
			struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (& (header_definitions_p -> hd_function_definitions));
			
			success_flag = TRUE;
			
			
			while ((node_p != NULL) && success_flag)	
				{
					if (WriteIDLFunction (out_p, node_p -> fdn_function_def_p))
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


static BOOL WriteIDLFunction (BPTR out_p, const struct FunctionDefinition * const fd_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "\t\t<method name=\"%s\" result=\"%s\">\n", fd_p -> fd_definition_p -> pa_name_s, fd_p -> fd_definition_p -> pa_type_s) >= 0)
		{
			struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);
			struct ParameterNode *next_node_p = NULL;

			success_flag = TRUE;

			while (((next_node_p = (struct ParameterNode *) IExec->GetSucc (& (curr_node_p -> pn_node))) != NULL) && success_flag)
				{
					success_flag = WriteIDLParameter (out_p, curr_node_p -> pn_param_p);
					curr_node_p = next_node_p;
				}
				
			if (success_flag)
				{
					success_flag = (IDOS->FPrintf (out_p, "\t\t</method>\n") >= 0);
				}
		}

	return success_flag;
}


static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = (IDOS->FPrintf (out_p, "\t\t\t<arg name=\"%s\" type=\"%s\">\n", param_p -> pa_name_s, param_p -> pa_type_s) >= 0);

	return success_flag;
}


static BOOL WriteIDLDefaultFunctions (BPTR out_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "\t\t<method name=\"Obtain\" result=\"uint32\"></method>\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "\t\t<method name=\"Release\" result=\"uint32\"></method>\n") >= 0)
				{
					if (IDOS->FPrintf (out_p, "\t\t<method name=\"Expunge\" result=\"void\" status=\"unimplemented\"></method>\n") >= 0)
						{
							if (IDOS->FPrintf (out_p, "\t\t<method name=\"Clone\" result=\"struct Interface *\" status=\"unimplemented\"></method>\n") >= 0)
								{
									success_flag = TRUE;
								}
						}
				}
		}

	return success_flag;
}



static BOOL WriteIDLIncludes (BPTR out_p, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s)
{
	BOOL success_flag = TRUE;

	return success_flag;
}




static BOOL WriteIDLFooter (BPTR out_p)
{
	return (IDOS->FPrintf (out_p, "\t</interface>\n</library>\n") >= 0);
}
