#ifndef LISTS_H
#define LISTS_H

#ifdef AMIGA
	#include <exec/lists.h>
	#include <exec/nodes.h>

	#define ADD_HEAD (IExec->AddHead) 
	#define ADD_TAIL (IExec->AddTail) 
	#define GET_HEAD (IExec->GetHead) 
	#define GET_TAIL (IExec->GetTail) 
	#define GET_SUCC (IExec->GetSucc)
#elif defined WIN32
	#define NT_USER	(254)
	#include "linked_list.h"

	#define ADD_HEAD (ListAddHead) 
	#define ADD_TAIL (ListAddTail) 
	#define GET_HEAD (ListGetHead) 
	#define GET_TAIL (ListGetTail) 
	#define GET_SUCC (ListGetNext) 

#endif

#endif

