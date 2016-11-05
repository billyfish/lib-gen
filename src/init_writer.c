#define INIT_CODE(lower_case_s) (\
"/* :ts=4\n"  \
" *  $VER: init.c $Revision$ (28-Feb-2015)\n"  \
" *\n"  \
" *  This file is part of " #lower_case_s ".\n"  \
" *\n"  \
" *  Copyright (c) 2015 Hyperion Entertainment CVBA.\n"  \
" *  All Rights Reserved.\n"  \
" *\n"  \
" * $Id$\n"  \
" *\n"  \
" * $Log$\n"  \
" *\n"  \
" *\n"  \
" */\n"  \
"\n"  \
"\n"  \
"#include <exec/exec.h>\n"  \
"#include <proto/exec.h>\n"  \
"#include <dos/dos.h>\n"  \
"#include <test/loader1.h>\n"  \
"#include <proto/" #lower_case_s ".h>\n"  \
"#include <stdarg.h>\n"  \
"\n"  \
"/* Version Tag */\n"  \
"#include "" #lower_case_s ".library_rev.h"\n"  \
"STATIC CONST UBYTE USED verstag[] = VERSTAG;\n"  \
"\n"  \
"struct Library\n"  \
"{\n"  \
"    struct Library libNode;\n"  \
"    BPTR segList;\n"  \
"    /* If you need more data fields, add them here */\n"  \
"};\n"  \
"\n"  \
"/*\n"  \
" * The system (and compiler) rely on a symbol named _start which marks\n"  \
" * the beginning of execution of an ELF file. To prevent others from \n"  \
" * executing this library, and to keep the compiler/linker happy, we\n"  \
" * define an empty _start symbol here.\n"  \
" *\n"  \
" * On the classic system (pre-AmigaOS 4.x) this was usually done by\n"  \
" * moveq #0,d0\n"  \
" * rts\n"  \
" *\n"  \
" */\n"  \
"int32 _start(void);\n"  \
"\n"  \
"int32 _start(void)\n"  \
"{\n"  \
"    /* If you feel like it, open DOS and print something to the user */\n"  \
"    return RETURN_FAIL;\n"  \
"}\n"  \
"\n"  \
"\n"  \
"/* Open the library */\n"  \
"STATIC struct Library *libOpen(struct LibraryManagerInterface *Self, ULONG version)\n"  \
"{\n"  \
"    struct Library *libBase = (struct Library *)Self->Data.LibBase; \n"  \
"\n"  \
"    if (version > VERSION)\n"  \
"    {\n"  \
"        return NULL;\n"  \
"    }\n"  \
"\n"  \
"    /* Add any specific open code here \n"  \
"       Return 0 before incrementing OpenCnt to fail opening */\n"  \
"\n"  \
"\n"  \
"    /* Add up the open count */\n"  \
"    libBase->libNode.lib_OpenCnt++;\n"  \
"    return (struct Library *)libBase;\n"  \
"\n"  \
"}\n"  \
"\n"  \
"\n"  \
"/* Close the library */\n"  \
"STATIC APTR libClose(struct LibraryManagerInterface *Self)\n"  \
"{\n"  \
"    struct Library *libBase = (struct Library *)Self->Data.LibBase;\n"  \
"    /* Make sure to undo what open did */\n"  \
"\n"  \
"\n"  \
"    /* Make the close count */\n"  \
"    ((struct Library *)libBase)->lib_OpenCnt--;\n"  \
"\n"  \
"    return 0;\n"  \
"}\n"  \
"\n"  \
"\n"  \
"/* Expunge the library */\n"  \
"STATIC APTR libExpunge(struct LibraryManagerInterface *Self)\n"  \
"{\n"  \
"    /* If your library cannot be expunged, return 0 */\n"  \
"    struct ExecIFace *IExec\n"  \
"        = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;\n"  \
"    APTR result = (APTR)0;\n"  \
"    struct Library *libBase = (struct Library *)Self->Data.LibBase;\n"  \
"    if (libBase->libNode.lib_OpenCnt == 0)\n"  \
"    {\n"  \
"	     result = (APTR)libBase->segList;\n"  \
"        /* Undo what the init code did */\n"  \
"\n"  \
"        IExec->Remove((struct Node *)libBase);\n"  \
"        IExec->DeleteLibrary((struct Library *)libBase);\n"  \
"    }\n"  \
"    else\n"  \
"    {\n"  \
"        result = (APTR)0;\n"  \
"        libBase->libNode.lib_Flags |= LIBF_DELEXP;\n"  \
"    }\n"  \
"    return result;\n"  \
"}\n"  \
"\n"  \
"/* The ROMTAG Init Function */\n"  \
"STATIC struct Library *libInit(struct Library *LibraryBase, APTR seglist, struct Interface *exec)\n"  \
"{\n"  \
"    struct Library *libBase = (struct Library *)LibraryBase;\n"  \
"    struct ExecIFace *IExec UNUSED = (struct ExecIFace *)exec;\n"  \
"\n"  \
"    libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;\n"  \
"    libBase->libNode.lib_Node.ln_Pri  = 0;\n"  \
"    libBase->libNode.lib_Node.ln_Name = "";\n"  \
"    libBase->libNode.lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;\n"  \
"    libBase->libNode.lib_Version      = VERSION;\n"  \
"    libBase->libNode.lib_Revision     = REVISION;\n"  \
"    libBase->libNode.lib_IdString     = VSTRING;\n"  \
"\n"  \
"    libBase->segList = (BPTR)seglist;\n"  \
"\n"  \
"    /* Add additional init code here if you need it. For example, to open additional\n"  \
"       Libraries:\n"  \
"       libBase->UtilityBase = IExec->OpenLibrary(\"utility.library\", 50L);\n"  \
"       if (libBase->UtilityBase)\n"  \
"       {\n"  \
"           libBase->IUtility = (struct UtilityIFace *)IExec->GetInterface(ElfBase->UtilityBase, \n"  \
"              \"main\", 1, NULL);\n"  \
"           if (!libBase->IUtility)\n"  \
"               return NULL;\n"  \
"       } else return NULL; */\n"  \
"\n"  \
"       return (struct Library *)libBase;\n"  \
"}\n"  \
"\n"  \
"/* ------------------- Manager Interface ------------------------ */\n"  \
"/* These are generic. Replace if you need more fancy stuff */\n"  \
"STATIC uint32 _manager_Obtain(struct LibraryManagerInterface *Self)\n"  \
"{\n"  \
"	uint32 res;\n"  \
"	__asm__ __volatile__(\n"  \
"	"1:	lwarx	%0,0,%1\n"\n"  \
"	"addic	%0,%0,1\n"\n"  \
"	"stwcx.	%0,0,%1\n"\n"  \
"	"bne-	1b"\n"  \
"	: "=&r" (res)\n"  \
"	: "r" (&Self->Data.RefCount)\n"  \
"	: "cc", "memory");\n"  \
"\n"  \
"	return res;\n"  \
"}\n"  \
"\n"  \
"STATIC uint32 _manager_Release(struct LibraryManagerInterface *Self)\n"  \
"{\n"  \
"	uint32 res;\n"  \
"	__asm__ __volatile__(\n"  \
"	"1:	lwarx	%0,0,%1\n"\n"  \
"	"addic	%0,%0,-1\n"\n"  \
"	"stwcx.	%0,0,%1\n"\n"  \
"	"bne-	1b"\n"  \
"	: "=&r" (res)\n"  \
"	: "r" (&Self->Data.RefCount)\n"  \
"	: "cc", "memory");\n"  \
"\n"  \
"	return res;\n"  \
"}\n"  \
"\n"  \
"/* Manager interface vectors */\n"  \
"STATIC CONST APTR lib_manager_vectors[] =\n"  \
"{\n"  \
"	_manager_Obtain,\n"  \
"	_manager_Release,\n"  \
"	NULL,\n"  \
"	NULL,\n"  \
"	libOpen,\n"  \
"	libClose,\n"  \
"	libExpunge,\n"  \
"	NULL,\n"  \
"	(APTR)-1\n"  \
"};\n"  \
"\n"  \
"/* \"__library\" interface tag list */\n"  \
"STATIC CONST struct TagItem lib_managerTags[] =\n"  \
"{\n"  \
"	{ MIT_Name,			(Tag)"__library"		},\n"  \
"	{ MIT_VectorTable,	(Tag)lib_manager_vectors},\n"  \
"	{ MIT_Version,		1						},\n"  \
"	{ TAG_DONE,			0						}\n"  \
"};\n"  \
"\n"  \
"/* ------------------- Library Interface(s) ------------------------ */\n"  \
"\n"  \
"#include \"" #lower_case_s "_vectors.c\"\n"  \
"\n"  \
"/* Uncomment this line (and see below) if your library has a 68k jump table */\n"  \
"/* extern APTR VecTable68K[]; */\n"  \
"\n"  \
"STATIC CONST struct TagItem " #lower_case_s "Tags[] =\n"  \
"{\n"  \
"	{ MIT_Name,			(Tag)\"" #lower_case_s "\"			},\n"  \
"	{ MIT_VectorTable,	(Tag)" #lower_case_s "_vectors	},\n"  \
"	{ MIT_Version,		1					},\n"  \
"	{ TAG_DONE,			0					}\n"  \
"};\n"  \
"\n"  \
"STATIC CONST CONST_APTR libInterfaces[] =\n"  \
"{\n"  \
"	lib_managerTags,\n"  \
"	" #lower_case_s "Tags,\n"  \
"	NULL\n"  \
"};\n"  \
"\n"  \
"STATIC CONST struct TagItem libCreateTags[] =\n"  \
"{\n"  \
"	{ CLT_DataSize,		sizeof(struct Library)	},\n"  \
"	{ CLT_InitFunc,		(Tag)libInit			},\n"  \
"	{ CLT_Interfaces,	(Tag)libInterfaces		},\n"  \
"	/* Uncomment the following line if you have a 68k jump table */\n"  \
"	/* { CLT_Vector68K, (Tag)VecTable68K }, */\n"  \
"	{TAG_DONE,		 0 }\n"  \
"};\n"  \
"\n"  \
"\n"  \
"/* ------------------- ROM Tag ------------------------ */\n"  \
"STATIC CONST struct Resident lib_res USED =\n"  \
"{\n"  \
"	RTC_MATCHWORD,\n"  \
"	(struct Resident *)&lib_res,\n"  \
"	(APTR)(&lib_res + 1),\n"  \
"	RTF_NATIVE|RTF_AUTOINIT, /* Add RTF_COLDSTART if you want to be resident */\n"  \
"	VERSION,\n"  \
"	NT_LIBRARY, /* Make this NT_DEVICE if needed */\n"  \
"	0, /* PRI, usually not needed unless you're resident */\n"  \
"	"",\n"  \
"	VSTRING,\n"  \
"	(APTR)libCreateTags\n"  \
"};\n"  \
"\n"  \)



static BOOL WriteLibInterfaceCode ()
{
	CONST CONST_STRPTR part0_s =
		"struct Library\n" \
		"{\n" \
	  "\tstruct Library libNode;\n" \
	  "\tBPTR segList;\n" \
	  "/* If you need more data fields, add them here */\n" \
		"};\n" \
		"\n" \
		"/*\n" \
 		"* The system (and compiler) rely on a symbol named _start which marks\n" \
 		"* the beginning of execution of an ELF file. To prevent others from \n" \
 		"* executing this library, and to keep the compiler/linker happy, we\n" \
 		"* define an empty _start symbol here.\n" \
 		"*\n" \
 		"* On the classic system (pre-AmigaOS 4.x) this was usually done by\n" \
 		"* moveq #0,d0\n" \
 		"* rts\n" \
 		"*\n" \
 		"*/\n" \
		"int32 _start(void);\n" \
		"\n" \
		"int32 _start(void)\n" \
		"{\n" \
    "\t/* If you feel like it, open DOS and print something to the user */\n" \
    "\treturn RETURN_FAIL;\n" \
		"}\n\n\n" \
		"/* Open the library */\n" \
		"STATIC struct Library *libOpen(struct LibraryManagerInterface *Self, ULONG version)\n" \
		"{\n" \
    "\tstruct Library *libBase = (struct Library *)Self->Data.LibBase; \n" \
		"\n" \
    "\tif (version > VERSION)\n" \
    "\t\t{\n" \
    "\t\t\treturn NULL;\n" \
    "\t}\n" \
		"\t\n" \
    "\t/* Add any specific open code here \n" \
    "\t   Return 0 before incrementing OpenCnt to fail opening */\n" \
		"\n" \
    "\t/* Add up the open count */\n" \
    "\tlibBase->libNode.lib_OpenCnt++;\n" \
    "\treturn (struct Library *)libBase;\n" \
		"}\n\n\n" \
		"/* Close the library */\n" \
		"STATIC APTR libClose(struct LibraryManagerInterface *Self)\n" \
		"{\n" \
		"struct Library *libBase = (struct Library *)Self->Data.LibBase;\n" \
		"/* Make sure to undo what open did */\n" \
		"\n" \
    "/* Make the close count */\n" \
    "((struct Library *)libBase)->lib_OpenCnt--;\n" \
		"\n" \
    "return 0;\n" \
		"}\n\n" \


/* Expunge the library */
STATIC APTR libExpunge(struct LibraryManagerInterface *Self)
{
    /* If your library cannot be expunged, return 0 */
    struct ExecIFace *IExec
        = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
    APTR result = (APTR)0;
    struct Library *libBase = (struct Library *)Self->Data.LibBase;
    if (libBase->libNode.lib_OpenCnt == 0)
    {
	     result = (APTR)libBase->segList;
        /* Undo what the init code did */

        IExec->Remove((struct Node *)libBase);
        IExec->DeleteLibrary((struct Library *)libBase);
    }
    else
    {
        result = (APTR)0;
        libBase->libNode.lib_Flags |= LIBF_DELEXP;
    }
    return result;
}

/* The ROMTAG Init Function */
STATIC struct Library *libInit(struct Library *LibraryBase, APTR seglist, struct Interface *exec)
{
    struct Library *libBase = (struct Library *)LibraryBase;
    struct ExecIFace *IExec UNUSED = (struct ExecIFace *)exec;

    libBase->libNode.lib_Node.ln_Type = NT_LIBRARY;
    libBase->libNode.lib_Node.ln_Pri  = 0;
    libBase->libNode.lib_Node.ln_Name = "";
    libBase->libNode.lib_Flags        = LIBF_SUMUSED|LIBF_CHANGED;
    libBase->libNode.lib_Version      = VERSION;
    libBase->libNode.lib_Revision     = REVISION;
    libBase->libNode.lib_IdString     = VSTRING;

    libBase->segList = (BPTR)seglist;

    /* Add additional init code here if you need it. For example, to open additional
       Libraries:
       libBase->UtilityBase = IExec->OpenLibrary("utility.library", 50L);
       if (libBase->UtilityBase)
       {
           libBase->IUtility = (struct UtilityIFace *)IExec->GetInterface(ElfBase->UtilityBase, 
              "main", 1, NULL);
           if (!libBase->IUtility)
               return NULL;
       } else return NULL; */

       return (struct Library *)libBase;
}

/* ------------------- Manager Interface ------------------------ */
/* These are generic. Replace if you need more fancy stuff */
STATIC uint32 _manager_Obtain(struct LibraryManagerInterface *Self)
{
	uint32 res;
	__asm__ __volatile__(
	"1:	lwarx	%0,0,%1\n"
	"addic	%0,%0,1\n"
	"stwcx.	%0,0,%1\n"
	"bne-	1b"
	: "=&r" (res)
	: "r" (&Self->Data.RefCount)
	: "cc", "memory");

	return res;
}

STATIC uint32 _manager_Release(struct LibraryManagerInterface *Self)
{
	uint32 res;
	__asm__ __volatile__(
	"1:	lwarx	%0,0,%1\n"
	"addic	%0,%0,-1\n"
	"stwcx.	%0,0,%1\n"
	"bne-	1b"
	: "=&r" (res)
	: "r" (&Self->Data.RefCount)
	: "cc", "memory");

	return res;
}

/* Manager interface vectors */
STATIC CONST APTR lib_manager_vectors[] =
{
	_manager_Obtain,
	_manager_Release,
	NULL,
	NULL,
	libOpen,
	libClose,
	libExpunge,
	NULL,
	(APTR)-1
};

/* "__library" interface tag list */
STATIC CONST struct TagItem lib_managerTags[] =
{
	{ MIT_Name,			(Tag)"__library"		},
	{ MIT_VectorTable,	(Tag)lib_manager_vectors},
	{ MIT_Version,		1						},
	{ TAG_DONE,			0						}
};

	
}


