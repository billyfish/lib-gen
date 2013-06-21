#ifndef TYPES_H
#define TYPES_H

#ifdef AMIGA
	#include <exec/types.h>
#else
	typedef unsigned char BOOL;

	#define TRUE (1)
	#define FALSE (0)
#endif

#endif		/* ifndef TYPES_H */
