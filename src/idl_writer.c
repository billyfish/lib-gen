#include <ctype.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>


#include <exec/types.h>

#include "idl_writer.h"
#include "memory.h"
#include "parameter.h"
#include "function_definition.h"
#include "header_definitions.h"
#include "debugging_utils.h"



static BOOL WriteIDL (struct Writer *writer_p, struct List *header_definitions_list_p, CONST_STRPTR library_s, const int32 version, const enum InterfaceFlag flag, BPTR out_p);

static CONST_STRPTR GetIDLWriterFileSuffix (struct Writer *writer_p);

static BOOL WriteIDLHeader (BPTR out_p, CONST CONST_STRPTR library_s, const int32 version, const enum InterfaceFlag flag, struct List *header_definitions_list_p);

static BOOL WriteIDLFunction (BPTR out_p, const struct FunctionDefinition * const fd_p);
static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p);

static BOOL WriteIDLDefaultFunctions (BPTR out_p);
static BOOL WriteIDLIncludes (BPTR out_p, struct List *includes_p);
static BOOL WriteIDLFooter (BPTR out_p);


static BOOL WriteIDLHeaderDefinitionsList (BPTR out_p, struct List * const header_definitions_list_p);



static BOOL WriteIDLHeaderDefinitions (BPTR out_p, struct HeaderDefinitions * const header_definitions_p);


static CONST_STRPTR GetIDLWriterFileSuffix (struct Writer *writer_p)
{
	return ".xml";
}


Writer *AllocateIDLWriter (void)
{
	IDLWriter *idl_p = (IDLWriter *) AllocMemory (sizeof (IDLWriter));

	if (idl_p)
		{
			idl_p -> iw_base_writer.wr_write_header_definitions_list_fn = WriteIDL;
			idl_p -> iw_base_writer.wr_get_file_suffix_fn = GetIDLWriterFileSuffix;
		}

	return ((Writer *) idl_p);
}


void FreeIDLWriter (Writer *writer_p)
{
	IDLWriter *idl_p = (IDLWriter *) writer_p;

	FreeMemory (idl_p);
}


static BOOL WriteIDL (struct Writer *writer_p, struct List *header_definitions_list_p, CONST_STRPTR library_s, const int32 version, const enum InterfaceFlag flag, BPTR out_p)
{
	BOOL success_flag = FALSE;

	if (WriteIDLHeader (out_p, library_s, version, flag, header_definitions_list_p))
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




static BOOL WriteIDLHeader (BPTR out_p, CONST CONST_STRPTR library_s, const int32 version, const enum InterfaceFlag flag, struct List *header_definitions_list_p)
{
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n") >= 0)
		{
			if (IDOS->FPrintf (out_p, "<!DOCTYPE library SYSTEM \"library.dtd\">\n") >= 0)
				{
					if (library_s)
						{
							if (IDOS->FPrintf (out_p, "<library name=\"%s\" basename=\"", library_s) >= 0)
								{
									int32 c = IUtility->ToUpper (*library_s);

									if (IDOS->FPutC (out_p, c) == c)
										{
											if (IDOS->FPrintf (out_p, "%sBase\">\n", library_s + 1) >= 0)
												{
													CONST_STRPTR flags_s = NULL;

													switch (flag)
														{
															case IF_PRIVATE:
																flags_s = "private";
																break;

															case IF_PROTECTED:
																flags_s = "protected";
																break;

															case IF_PUBLIC:
															default:
																flags_s = "none";
																break;
														}

													if (WriteIDLIncludes (out_p, header_definitions_list_p))
														{
															if (IDOS->FPrintf (out_p, "\n\t<interface name=\"%s\" version=\"%lu.0\" flags=\"%s\" prefix=\"_%s_\" struct=\"", library_s, version, flags_s, library_s) >= 0)
																{
																	if (IDOS->FPutC (out_p, c) == c)
																		{
																			if (IDOS->FPrintf (out_p, "%sIFace\">\n", library_s + 1) >= 0)
																				{
																					success_flag = TRUE;
																				}
																		}
																}
														}
												}
										}
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


	if (IDOS->FPrintf (out_p, "\n\t\t<!-- %s = %lu definitions -->\n", header_definitions_p -> hd_filename_s, GetFunctionDefinitionsListSize (& (header_definitions_p -> hd_function_definitions))) >= 0)
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
			if (fd_p -> fd_args_p)
				{
					struct ParameterNode *node_p = (struct ParameterNode *) IExec->GetHead (fd_p -> fd_args_p);
					
					success_flag = TRUE;
					
					while (node_p && success_flag)
						{
							if (WriteIDLParameter (out_p, node_p -> pn_param_p))
								{
									node_p = (struct ParameterNode *) IExec->GetSucc ((struct Node *) node_p);
								}
							else
								{
									success_flag = FALSE;
								}
						}


					if (success_flag)
						{
							success_flag = (IDOS->FPrintf (out_p, "\t\t</method>\n") >= 0);
						}
				}
		}

	return success_flag;
}


static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p)
{
	BOOL success_flag = TRUE;
	
	if (!IsVoidParameter (param_p))
		{		
			success_flag = (IDOS->FPrintf (out_p, "\t\t\t<arg name=\"%s\" type=\"%s\" />\n", param_p -> pa_name_s, param_p -> pa_type_s) >= 0);
		}

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



static BOOL WriteIDLIncludes (BPTR out_p, struct List *header_definitions_list_p)
{
	BOOL success_flag = TRUE;
	struct HeaderDefinitionsNode *node_p = (struct HeaderDefinitionsNode *) IExec->GetHead (header_definitions_list_p);

	while ((node_p != NULL) && success_flag)
		{
			if (IDOS->FPrintf (out_p, "\t<include>%s</include>\n", node_p -> hdn_defs_p -> hd_filename_s) >= 0)
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




static BOOL WriteIDLFooter (BPTR out_p)
{
	return (IDOS->FPrintf (out_p, "\t</interface>\n</library>\n") >= 0);
}
