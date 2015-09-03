#ifndef AUTO_INIT_WRITER_H
#define AUTO_INIT_WRITER_H

#include <dos/dos.h>
#include <exec/types.h>


#ifdef __cplusplus
extern "C" {
#endif


BOOL WriteAutoInitSource (CONST CONST_STRPTR library_s, CONST CONST_STRPTR header_s);

#ifdef __cplusplus
}
#endif


#endif		/* ifndef AUTO_INIT_WRITER_HH */
