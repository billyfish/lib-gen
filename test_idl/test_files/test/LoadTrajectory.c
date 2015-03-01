/* :ts=4
 *  $VER: LoadTrajectory.c $Revision$ (28-Feb-2015)
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

/****** test/test/LoadTrajectory ******************************************
*
*   NAME
*      LoadTrajectory -- Description
*
*   SYNOPSIS
*      Trajectory * LoadTrajectory(LoaderModule * loader_p, 
*          const char * const path, const double64 first_frame_time, 
*          const double64 chunk_time, const uint32 num_frames_to_skip, 
*          EventListener * events_listener_p);
*
*   FUNCTION
*
*   INPUTS
*       loader_p - 
*       path - 
*       first_frame_time - 
*       chunk_time - 
*       num_frames_to_skip - 
*       events_listener_p - 
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

Trajectory * _test_LoadTrajectory(struct TestIFace *Self,
       LoaderModule * loader_p,
       const char * const path,
       const double64 first_frame_time,
       const double64 chunk_time,
       const uint32 num_frames_to_skip,
       EventListener * events_listener_p)
{
    /* Write me. Really, I dare you! */
    ((struct ExecIFace *)((*(struct ExecBase **)4)->MainInterface))->DebugPrintF(
		"Function test::LoadTrajectory not implemented\n");  
    return (Trajectory *)0;

}

