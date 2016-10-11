#ifdef __cplusplus
		extern "C" {
#endif

#ifndef DEBUGGING_UTILS_H
#define DEBUGGING_UTILS_H

#ifdef _AMIGA_
	#include <clib/debug_protos.h>

	#ifdef __amigaos4__
		#include <proto/exec.h>
		#define KPRINTF IExec->DebugPrintF
	//#define	KPRINTF(format, args...)((struct ExecIFace *) ((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(format, ## args)
	#else
		#define KPRINTF KPrintF
	#endif
#else
	#include <stdio.h>
	#define KPRINTF printf
#endif

#ifdef _DEBUG
	#define DB(X)	(X)
	#define ENTER() IExec->DebugPrintF("Entered %s in %s at line %d", __func__, __FILE__, __LINE__)
	#define LEAVE() IExec->DenugPrintF("Leaving %s in %s at line %d", __func__, __FILE__, __LINE__)
#else
	#define DB(X)
#define ENTER()
#define LEAVE()
#endif



#endif	/* #ifndef DEBUGGING_UTILS_H */

#ifdef __cplusplus
}
#endif

