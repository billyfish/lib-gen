/* :ts=4
 *  $VER: Release.c $Revision$ (28-Feb-2015)
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

/****** test/test/Release ******************************************
*
*   NAME
*      Release -- Description
*
*   SYNOPSIS
*      uint32 Release(void);
*
*   FUNCTION
*
*   INPUTS
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

uint32 _test_Release(struct TestIFace *Self)
{
    /* Write me. Really, I dare you! */
    ((struct ExecIFace *)((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(
		"Function test::Release not implemented\n");  
    return (uint32)0;

}

