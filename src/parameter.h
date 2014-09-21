#ifndef PARAMETER_H
#define PARAMETER_H

#include <dos/dos.h>
#include <exec/lists.h>

#include <exec/types.h>

enum ParameterTypes
{
	PT_PARAMETER = NT_USER - 1
};

struct Parameter
{
	char *pa_name_s;
	char *pa_type_s;
};

struct ParameterArray
{
	struct Parameter *pa_params_p;
	int pa_num_params;
};

struct ParameterNode
{
	struct Node pn_node;

	struct Parameter *pn_param_p;
};


struct FileFunctions
{
	const char * const ff_filename_s;

	/** List of FunctionDefinitionNodes */
	struct List *ff_definitions_p;
};


#ifdef __cplusplus
extern "C" {
#endif


struct Parameter *AllocateParameter (char *type_s, char *name_s);

void FreeParameter (struct Parameter *param_p);


struct Parameter *ParseParameter (const char *start_p, const char *end_p);

struct ParameterNode *AllocateParameterNode (struct Parameter *param_p);

void FreeParameterNode (struct ParameterNode *node_p);

void FreeParameterList (struct List *params_p);

struct List *AllocateParameterList (void);

BOOL FillInParameter (struct Parameter *param_p, const char *start_p, const char *end_p);

BOOL SetParameterName (struct Parameter *param_p, const char *start_p, const char *end_p);

BOOL SetParameterType (struct Parameter *param_p, const char *start_p, const char *end_p);

void ClearParameter (struct Parameter *param_p);

BOOL PrintParameter (BPTR out_p, const struct Parameter * const param_p);

BOOL PrintParameterList (BPTR out_p, struct List * const params_p);


#ifdef __cplusplus
}
#endif


#endif		/* ifndef PARAMETER_H */
