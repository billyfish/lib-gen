#include <proto/dos.h>
#include <proto/exec.h>

#include "types.h"
#include "idl_writer.h"
#include "memory.h"
#include "parameter.h"
#include "function_definition.h"


static BOOL WriteIDL (struct Writer *writer_p, const struct List *function_definitions_p, BPTR out_p);
static BOOL WriteIDLHeader (BPTR out_p, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s);
static BOOL WriteIDLFunctions (BPTR out_p, const struct List * const fds_list_p);

static BOOL WriteIDLFunction (BPTR out_p, const struct FunctionDefinition * const fd_p);
static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p);

static BOOL WriteIDLDefaultFunctions (BPTR out_p);
static BOOL WriteIDLIncludes (BPTR out_p, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s);
static BOOL WriteIDLFooter (BPTR out_p);




Writer *AllocateIDLWriter (void)
{
	IDLWriter *idl_p = (IDLWriter *) AllocMemory (sizeof (IDLWriter));

	if (idl_p)
		{
			idl_p -> iw_base_writer.wr_write_function_definitions_fn = WriteIDL;
		}

	return ((Writer *) idl_p);
}


void FreeIDLWriter (Writer *writer_p)
{
	IDLWriter *idl_p = (IDLWriter *) writer_p;

	FreeMemory (idl_p);
}


static BOOL WriteIDL (struct Writer *writer_p, const struct List *function_definitions_p, BPTR out_p)
{
	BOOL success_flag = FALSE;
	const char * const name_s = NULL;
	const char * const basename_s = NULL;
	const char * const struct_name_s = NULL;
	const char * const prefix_s = NULL;

	if (WriteIDLHeader (out_p, name_s, basename_s, struct_name_s, prefix_s))
		{
			if (WriteIDLDefaultFunctions (out_p))
				{
					if (WriteIDLFunctions (out_p, function_definitions_p))
						{
							success_flag = TRUE;
						}

				}

		}		/* if (WriteIDLHeader (name_s, basename_s, struct_name_s, prefix_s)) */

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
							if (IDOS->FPrintf (out_p, "<interface name=\"%s\" version=\"1\" flags=\"protected\" struct=\"%s\" prefix=\"%s\">\n", name_s, struct_name_s, prefix_s) >= 0)
								{
									success_flag = TRUE;
								}
						}
				}
		}

	return success_flag;
}



static BOOL WriteIDLFunctions (BPTR out_p, const struct List * const fds_list_p)
{
	BOOL success_flag = TRUE;
	const struct FunctionDefinitionNode *curr_node_p = (const struct FunctionDefinitionNode *) IExec->GetHead (fds_list_p);
	const struct FunctionDefinitionNode *next_node_p = NULL;

	while (((next_node_p = (struct FunctionDefinitionNode *) (curr_node_p -> fdn_node.ln_Succ)) != NULL) && success_flag)
		{
			success_flag = WriteIDLFunction (out_p, curr_node_p -> fdn_function_def_p);
		}


	return success_flag;
}


static BOOL WriteIDLFunction (BPTR out_p, const struct FunctionDefinition * const fd_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "\t<method name=\"%s\" result=\"%s\">\n", fd_p -> fd_definition_p -> pa_name_s, fd_p -> fd_definition_p -> pa_type_s) >= 0)
		{
			struct ParameterNode *curr_node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);
			struct ParameterNode *next_node_p = NULL;
			
			success_flag = TRUE;
			
			while (((next_node_p = (struct ParameterNode *) (curr_node_p -> pn_node.ln_Succ)) != NULL) && success_flag)
				{
					success_flag = WriteIDLParameter (out_p, curr_node_p -> pn_param_p);
				}
		}

	return success_flag;
}


static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = (IDOS->FPrintf (out_p, "\t\t<arg name=\"%s\" type=\"%s\">\n", param_p -> pa_name_s, param_p -> pa_type_s) >= 0);

	return success_flag;
}


static BOOL WriteIDLDefaultFunctions (BPTR out_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "\t<method name=\"Obtain\" result=\"uint32\"></method>\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "\t<method name=\"Release\" result=\"uint32\"></method>\n") >= 0)
				{
					if (IDOS->FPrintf (out_p, "\t<method name=\"Expunge\" result=\"void\" status=\"unimplemented\"></method>\n") >= 0)
						{
							if (IDOS->FPrintf (out_p, "\t<method name=\"Clone\" result=\"struct Interface *\" status=\"unimplemented\"></method>\n") >= 0)
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
	return (IDOS->FPrintf (out_p, "</library>\n") >= 0);
}
