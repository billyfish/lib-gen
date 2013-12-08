#ifndef TYPES_H
#define TYPES_H

#ifdef AMIGA
	#include <exec/types.h>
#else
	typedef unsigned char BOOL;
	typedef char * STRPTR;
	typedef const char * CONST_STRPTR;
	typedef unsigned char uint8;
	typedef unsigned long uint32;
	#define TRUE (1)
	#define FALSE (0)

	#include <stdio.h>
	typedef FILE * BPTR;
#endif

#endif		/* ifndef TYPES_H */
