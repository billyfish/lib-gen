#ifndef AUTO_INIT_WRITER_H
#define AUTO_INIT_WRITER_H

#include <dos/dos.h>
#include <exec/types.h>


#ifdef __cplusplus
extern "C" {
#endif



BOOL WriteAutoInitFiles (CONST_STRPTR library_s, CONST_STRPTR out_dir_s);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef AUTO_INIT_WRITER_HH */
