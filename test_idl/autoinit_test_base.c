/*
**	$Id$
**	Generated by IDLTool 53.1
**	Do not edit
**
**	AutoInit stub for test
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

#include <test/loader1.h>

#include <interfaces/test.h>
#include <proto/exec.h>
#include <assert.h>

/****************************************************************************/

struct Library * TestBase = NULL;
static struct Library * __TestBase;

/****************************************************************************/

void test_base_constructor(void)
{
    if (TestBase != NULL)
    {
        return; /* Someone was quicker, e.g. an interface constructor */
    }
    __TestBase = TestBase = (struct Library *)IExec->OpenLibrary("", 0L);
    assert(TestBase != NULL);
}
__attribute__((section(".ctors.zzzz"))) static void
(*test_base_constructor_ptr)(void) USED = test_base_constructor;

/****************************************************************************/

void test_base_destructor(void)
{
    if (__TestBase)
    {
        IExec->CloseLibrary((struct Library *)__TestBase);
    }
}
__attribute__((section(".dtors.zzzz"))) static void
(*test_base_destructor_ptr)(void) USED = test_base_destructor;

/****************************************************************************/

