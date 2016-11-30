#include <ctype.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>


#include <exec/types.h>

#include "idl_writer.h"
#include "parameter.h"
#include "function_definition.h"
#include "debugging_utils.h"



static BOOL WriteIDL (struct Writer *writer_p, struct List *function_definitions_p, CONST_STRPTR library_s, CONST_STRPTR prefix_s, const int32 version, const enum InterfaceFlag flag, BPTR out_p);

static CONST_STRPTR GetIDLWriterFileSuffix (struct Writer *writer_p);

static BOOL WriteIDLHeader (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, const int32 version, const enum InterfaceFlag flag, struct List *function_definitions_list_p);

static BOOL WriteIDLFunctionDefinition (BPTR out_p, const struct FunctionDefinition * const fd_p);

static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p);

static BOOL WriteIDLDefaultFunctions (BPTR out_p);
static BOOL WriteIDLIncludes (BPTR out_p, struct List *includes_p);
static BOOL WriteIDLFooter (BPTR out_p);


static BOOL WriteIDLFunctionDefinitionsList (BPTR out_p, struct List * const function_definitions_list_p);


static CONST_STRPTR GetIDLWriterFileSuffix (struct Writer *writer_p)
{
	ENTER ();

	LEAVE ();

	return ".xml";
}


Writer *AllocateIDLWriter (void)
{
	ENTER ();
	IDLWriter *idl_p = (IDLWriter *) IExec->AllocVecTags (sizeof (IDLWriter), TAG_DONE);

	if (idl_p)
		{
			idl_p -> iw_base_writer.wr_write_function_definitions_list_fn = WriteIDL;
			idl_p -> iw_base_writer.wr_get_file_suffix_fn = GetIDLWriterFileSuffix;
		}

	LEAVE ();
	return ((Writer *) idl_p);
}


void FreeIDLWriter (Writer *writer_p)
{
	ENTER ();
	IDLWriter *idl_p = (IDLWriter *) writer_p;

	IExec->FreeVec (idl_p);
	LEAVE ();
}


static BOOL WriteIDL (struct Writer *writer_p, struct List *function_definitions_p, CONST_STRPTR library_s, CONST_STRPTR prefix_s, const int32 version, const enum InterfaceFlag flag, BPTR out_p)
{
	ENTER ();
	BOOL success_flag = FALSE;

	DB (KPRINTF ("%s %ld - Entering WriteIDL", __FILE__, __LINE__));

	if (WriteIDLHeader (out_p, library_s, prefix_s, version, flag, function_definitions_p))
		{
			if (WriteIDLDefaultFunctions (out_p))
				{
					if (WriteIDLFunctionDefinitionsList (out_p, function_definitions_p))
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


	DB (KPRINTF ("%s %ld - Exiting  WriteIDL %ld", __FILE__, __LINE__, success_flag));

	LEAVE ();
	return success_flag;
}




static BOOL WriteIDLHeader (BPTR out_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR prefix_s, const int32 version, const enum InterfaceFlag flag, struct List *function_definitions_p)
{
	ENTER ();
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

													if (WriteFunctionDefinitionListIncludes (out_p, function_definitions_p, "\t<include>", "</include>"))
														{
															if (IDOS->FPrintf (out_p, "\n\t<interface name=\"%s\" version=\"%lu.0\" flags=\"%s\" prefix=\"%s\" struct=\"", library_s, version, flags_s, prefix_s) >= 0)
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

	LEAVE ();
	return success_flag;
}


static BOOL WriteIDLFunctionDefinitionsList (BPTR out_p, struct List * const function_definitions_list_p)
{
	ENTER ();
	BOOL success_flag = TRUE;
	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_definitions_list_p);

	while ((node_p != NULL) && success_flag)
		{
			if (WriteIDLFunctionDefinition (out_p, node_p -> fdn_function_def_p))
				{
					node_p = (struct FunctionDefinitionNode *) IExec->GetSucc ((struct Node *) node_p);
				}
			else
				{
					success_flag = FALSE;
				}
		}

	LEAVE ();
	return success_flag;
}


static BOOL WriteIDLFunctionDefinition (BPTR out_p, const struct FunctionDefinition * const fd_p)
{
	ENTER ();
	BOOL success_flag = FALSE;

	if (IDOS->FPrintf (out_p, "\n\t\t<!-- %s:%ld -->\n", fd_p -> fd_header_filename_s, fd_p -> fd_line_number) >= 0)
		{
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

				}		/* if (IDOS->FPrintf (out_p, "\t\t<method name=\"%s\" result=\"%s\">\n", fd_p -> fd_definition_p -> pa_name_s, fd_p -> fd_definition_p -> pa_type_s) >= 0) */

		}		/* if (IDOS->FPrintf (out_p, "\n\t\t<!-- %s:%ld -->\n", fd_p -> fd_filename_s, fd_p -> fd_line_number) >= 0) */


	LEAVE ();
	return success_flag;
}


static BOOL WriteIDLParameter (BPTR out_p, const struct Parameter * const param_p)
{
	ENTER ();

	BOOL success_flag = TRUE;

	if (!IsVoidParameter (param_p))
		{
			success_flag = (IDOS->FPrintf (out_p, "\t\t\t<arg name=\"%s\" type=\"%s\" />\n", param_p -> pa_name_s, param_p -> pa_type_s) >= 0);
		}

	LEAVE ();
	return success_flag;
}


static BOOL WriteIDLDefaultFunctions (BPTR out_p)
{
	BOOL success_flag = FALSE;
	ENTER ();

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

	LEAVE ();
	return success_flag;
}


static BOOL WriteIDLFooter (BPTR out_p)
{
	ENTER ();

	LEAVE ();
	return (IDOS->FPrintf (out_p, "\t</interface>\n</library>\n") >= 0);
}
