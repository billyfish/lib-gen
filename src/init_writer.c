#include <exec/types.h>

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
		"}\n\n";
		
	return FALSE;
}


