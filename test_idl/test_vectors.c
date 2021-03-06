
/* This file was generated by idltool 53.1. Do not edit */

#ifndef EXEC_TYPES_H
#include <exec/types.h>

#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#ifndef TEST_LOADER1_H
#include <test/loader1.h>
#endif

extern uint32               VARARGS68K _test_Obtain(struct TestIFace *);
extern uint32               VARARGS68K _test_Release(struct TestIFace *);
extern BOOLEAN              VARARGS68K _test_InitialiseLoaderData(struct TestIFace *, LoaderModuleData * data_p, const HashTable * const config_p);
extern BOOLEAN              VARARGS68K _test_InitialiseLoaderData(struct TestIFace *, LoaderModuleData * data_p, const HashTable * const config_p);
extern void                 VARARGS68K _test_ClearLoaderData(struct TestIFace *, LoaderModuleData * data_p);
extern Trajectory *         VARARGS68K _test_LoadTrajectory(struct TestIFace *, LoaderModule * loader_p, const char * const path, const double64 first_frame_time, const double64 chunk_time, const uint32 num_frames_to_skip, EventListener * events_listener_p);


STATIC CONST APTR test_vectors[] =
{
    _test_Obtain,
    _test_Release,
    NULL,
    NULL,
    _test_InitialiseLoaderData,
    _test_InitialiseLoaderData,
    _test_ClearLoaderData,
    _test_LoadTrajectory,
    (APTR)-1
};

