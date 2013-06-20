#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdio.h>

#ifdef AMIGA
	#include <exec/types.h>
#else
	typedef unsigned char BOOL;

	#define TRUE (1)
	#define FALSE (0)
#endif


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

struct FunctionDefinition
{
	struct Parameter *fd_definition_p;
	struct ParameterArray *fs_args_p;
};

#ifdef __cplusplus
extern "C" {
#endif


struct ParameterArray *AllocateParameterArray (int num_params);

void FreeParameterArray (struct ParameterArray *params_p);

struct Parameter *AllocateParameter (const char *name_s, const char *type_s);

BOOL FillInParameter (struct Parameter *param_p, const char *start_p, const char *end_p);

BOOL SetParameterName (struct Parameter *param_p, const char *start_p, const char *end_p);

BOOL SetParameterType (struct Parameter *param_p, const char *start_p, const char *end_p);

void FreeParameter (struct Parameter *param_p);

void ClearParameter (struct Parameter *param_p);

BOOL PrintParameter (FILE *out_f, const struct Parameter * const param_p);

BOOL PrintParameterArray (FILE *out_f, const struct ParameterArray * const params_p);

BOOL PrintFunctionDefinition (FILE *out_f, const struct FunctionDefinition * const fn_p);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef PARAMETER_H */
