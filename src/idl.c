#include <proto/exec.h>

#include "types.h"
#include "idl_writer.h"
#include "memory.h"
#include "parameter.h"
#include "function_definition.h"


static BOOL WriteIDL (struct Writer *writer_p, const struct List *function_definitions_p, FILE *out_f);
static BOOL WriteIDLHeader (FILE *out_f, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s);
static BOOL WriteIDLFunctions (FILE *out_f, const struct List * const fds_list_p);

static BOOL WriteIDLFunction (FILE *out_f, const struct FunctionDefinition * const fd_p);

static BOOL WriteIDLDefaultFunctions (FILE *out_f);
static BOOL WriteIDLIncludes (FILE *out_f, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s);
static BOOL WriteIDLFooter (FILE *out_f);





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


static BOOL WriteIDL (struct Writer *writer_p, const struct List *function_definitions_p, FILE *out_f)
{
	BOOL success_flag = FALSE;
	const char * const name_s = NULL;
	const char * const basename_s = NULL;
	const char * const struct_name_s = NULL;
	const char * const prefix_s = NULL;
	
	if (WriteIDLHeader (out_f, name_s, basename_s, struct_name_s, prefix_s))
		{
			if (WriteIDLDefaultFunctions (out_f))
				{
					if (WriteIDLFunctions (out_f, function_definitions_p))
						{
							success_flag = TRUE;
						}
						
				}
				
		}		/* if (WriteIDLHeader (name_s, basename_s, struct_name_s, prefix_s)) */
	
	return success_flag;
}



static BOOL WriteIDLHeader (FILE *out_f, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s)
{
	BOOL success_flag = FALSE;

	if (fprintf (out_f, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n") >= 0)
		{
			if (fprintf (out_f, "<!DOCTYPE library SYSTEM \"library.dtd\">\n") >= 0)
				{
					if (fprintf (out_f, "<library name=\"%s\" basename=\"%s\">\n", name_s, basename_s) >= 0)
						{
							if (fprintf (out_f, "<interface name=\"%s\" version=\"1\" flags=\"protected\" struct=\"%s\" prefix=\"%s\">\n", name_s, struct_name_s, prefix_s) >= 0)
								{
									success_flag = TRUE;
								}
						}
				}
		}

	return success_flag;
}



static BOOL WriteIDLFunctions (FILE *out_f, const struct List * const fds_list_p)
{
	BOOL success_flag = TRUE;
	const struct FunctionDefinitionNode *curr_node_p = (const struct FunctionDefinitionNode *) IExec->GetHead (fds_list_p);
	const struct FunctionDefinitionNode *next_node_p = NULL;
	
	while (((next_node_p = (struct FunctionDefinitionNode *) (curr_node_p -> fdn_node.ln_Succ)) != NULL) && success_flag)
		{
			success_flag = WriteIDLFunction (out_f, curr_node_p -> fdn_function_def_p);
		}
	 

	return success_flag;
}



static BOOL WriteIDLFunction (FILE *out_f, const struct FunctionDefinition * const fd_p)
{
	BOOL success_flag = FALSE;

	if (fprintf (out_f, "\t<method name=\"%s\" result=\"%s\">\n", fd_p -> fd_definition_p -> pa_name_s, fd_p -> fd_definition_p -> pa_type_s) >= 0)
		{
			struct ParameterNode *node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);

			success_flag = TRUE;

			while (success_flag)
				{
					
				}

		}

	return success_flag;
}


static BOOL WriteIDLDefaultFunctions (FILE *out_f)
{
	BOOL success_flag = FALSE;

	if (fprintf (out_f, "\t<method name=\"Obtain\" result=\"uint32\"></method>\n") >= 0)
		{
			if (fprintf (out_f, "\t<method name=\"Release\" result=\"uint32\"></method>\n") >= 0)
				{
					if (fprintf (out_f, "\t<method name=\"Expunge\" result=\"void\" status=\"unimplemented\"></method>\n") >= 0)
						{
							if (fprintf (out_f, "\t<method name=\"Clone\" result=\"struct Interface *\" status=\"unimplemented\"></method>\n") >= 0)
								{
									success_flag = TRUE;
								}
						}
				}
		}

	return success_flag;
}



static BOOL WriteIDLIncludes (FILE *out_f, const char * const name_s, const char * const basename_s, const char * const struct_name_s, const char * const prefix_s)
{
	BOOL success_flag = TRUE;

	return success_flag;
}




static BOOL WriteIDLFooter (FILE *out_f)
{
	return (fprintf (out_f, "</library>\n") >= 0);
}
