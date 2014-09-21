#ifndef IDL_WRITER_H
#define IDL_WRITER_H

#include "typedefs.h"

Writer *AllocateIDLWriter ();

void FreeIDLWriter (Writer *writer_p);

BOOL WriteIDLHeader (FILE *out_f, const char * const name_s, const char * const basename_s, const char * const name_s, const char * const struct_name_s, const char * const prefix_s);

BOOL WriteIDLFunction (FILE *out_f, const struct FunctionDefinition * const fd_p);

BOOL WriteIDLDefaultFunctions (FILE *out_f);


BOOL WriteIDLIncludes (FILE *out_f, const char * const name_s, const char * const basename_s, const char * const name_s, const char * const struct_name_s, const char * const prefix_s);


BOOL WriteIDLFooter (FILE *out_f);


#endif