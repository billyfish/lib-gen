#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdio.h>

#include <exec/lists.h>

#include "types.h"

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

BOOL PrintParameter (FILE *out_f, const struct Parameter * const param_p);

BOOL PrintParameterList (FILE *out_f, struct List * const params_p);


#ifdef __cplusplus
}
#endif


#endif		/* ifndef PARAMETER_H */
