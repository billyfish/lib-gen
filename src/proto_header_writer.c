#define PROTO_HEADER_CODE(upper_case_s,lower_case_s,capitalized_s) ( \
"#ifndef PROTO_" #upper_case_s "_H\n" \
"#define PROTO_" #upper_case_s "_H\n" \
"\n" \
"/*\n" \
"**	$Id$\n" \
"**	Includes Release 50.1\n" \
"**\n" \
"**	Prototype/inline/pragma header file combo\n" \
"**\n" \
"**	Copyright (c) 2010 Hyperion Entertainment CVBA.\n" \
"**	All Rights Reserved.\n" \
"*/\n" \
"\n" \
"/****************************************************************************/\n" \
"\n" \
"#ifndef __NOLIBBASE__\n" \
" #ifndef __USE_BASETYPE__\n" \
"  extern struct Library *" #capitalized_s "Base;\n" \
" #else\n" \
"  extern struct Library *" #capitalized_s "Base;\n" \
" #endif /* __USE_BASETYPE__ */\n" \
"#endif /* __NOLIBBASE__ */\n" \
"\n" \
"/****************************************************************************/\n" \
"\n" \
"#ifdef __amigaos4__\n" \
" #include <interfaces/" #lower_case_s ".h>\n" \
" #ifdef __USE_INLINE__\n" \
"  #include <inline4/" " #upper_case_s "#lower_case_s ".h>\n" \
" #endif /* __USE_INLINE__ */\n" \
" #ifndef CLIB_" #upper_case_s "_PROTOS_H\n" \
"  #define CLIB_" #upper_case_s "_PROTOS_H 1\n" \
" #endif /* CLIB_" #upper_case_s "_PROTOS_H */\n" \
" #ifndef __NOGLOBALIFACE__\n" \
" #endif /* __NOGLOBALIFACE__ */\n" \
"#else /* __amigaos4__ */\n" \
" #ifndef CLIB_" #upper_case_s "_PROTOS_H\n" \
"  #include <clib/" #lower_case_s "_protos.h>\n" \
" #endif /* CLIB_" #upper_case_s "_PROTOS_H */\n" \
" #if defined(__GNUC__)\n" \
"  #ifndef __PPC__\n" \
"   #include <inline/" #lower_case_s ".h>\n" \
"  #else\n" \
"   #include <ppcinline/" #lower_case_s ".h>\n" \
"  #endif /* __PPC__ */\n" \
" #elif defined(__VBCC__)\n" \
"  #ifndef __PPC__\n" \
"   #include <inline/" #lower_case_s "_protos.h>\n" \
"  #endif /* __PPC__ */\n" \
" #else\n" \
"  #include <pragmas/" #lower_case_s "_pragmas.h>\n" \
" #endif /* __GNUC__ */\n" \
"#endif /* __amigaos4__ */\n" \
"\n" \
"/****************************************************************************/\n" \
"\n" \
"#endif /* PROTO_" #upper_case_s "_H */\n")


