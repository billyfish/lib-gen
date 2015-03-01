/* :ts=4
 *  $VER: ClearLoaderData.c $Revision$ (28-Feb-2015)
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

/****** test/test/ClearLoaderData ******************************************
*
*   NAME
*      ClearLoaderData -- Description
*
*   SYNOPSIS
*      void ClearLoaderData(LoaderModuleData * data_p);
*
*   FUNCTION
*
*   INPUTS
*       data_p - 
*
*   RESULT
*       This function does not return a result
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

void _test_ClearLoaderData(struct TestIFace *Self,
       LoaderModuleData * data_p)
{
    /* Write me. Really, I dare you! */
    ((struct ExecIFace *)((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(
		"Function test::ClearLoaderData not implemented\n");  

}

