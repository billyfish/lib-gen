/* :ts=4
 *  $VER: InitialiseLoaderData.c $Revision$ (28-Feb-2015)
 *
 *  This file is part of test.
 *
 *  Copyright (c) 2015 Hyperion Entertainment CVBA.
 *  All Rights Reserved.
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <test/loader1.h>
#include <proto/test.h>
#include <stdarg.h>

/****** test/test/InitialiseLoaderData ******************************************
*
*   NAME
*      InitialiseLoaderData -- Description
*
*   SYNOPSIS
*      BOOLEAN InitialiseLoaderData(LoaderModuleData * data_p, 
*          const HashTable * const config_p);
*
*   FUNCTION
*
*   INPUTS
*       data_p - 
*       config_p - 
*
*   RESULT
*       The result ...
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/

BOOLEAN _test_InitialiseLoaderData(struct TestIFace *Self,
       LoaderModuleData * data_p,
       const HashTable * const config_p)
{
    /* Write me. Really, I dare you! */
    ((struct ExecIFace *)((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(
		"Function test::InitialiseLoaderData not implemented\n");  
    return (BOOLEAN)0;

}

