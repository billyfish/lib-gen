#ifndef PROTO_TEST_H
#define PROTO_TEST_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	Copyright (c) 2010 Hyperion Entertainment CVBA.
**	All Rights Reserved.
*/

#ifndef TEST_LOADER1_H
#include <test/loader1.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * TestBase;
 #else
  extern struct Library * TestBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/test.h>
 #ifdef __USE_INLINE__
  #include <inline4/test.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_TEST_PROTOS_H
  #define CLIB_TEST_PROTOS_H 1
 #endif /* CLIB_TEST_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_TEST_PROTOS_H
  #include <clib/test_protos.h>
 #endif /* CLIB_TEST_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/test.h>
  #else
   #include <ppcinline/test.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/test_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/test_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_TEST_H */
