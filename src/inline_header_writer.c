#define INLINE_HEADER_CODE(header_s, guard_s) ( \
"#ifndef INLINE4_" #guard_s "_H\n" \
"#define INLINE4_" #guard_s "_H\n" \
"\n" \
"/*\n" \
"** This file was auto generated by idltool 53.1.\n" \
"**\n" \
"** It provides compatibility to OS3 style library\n" \
"** calls by substituting functions.\n" \
"**\n" \
"** Do not edit manually.\n" \
"*/ \n" \
"\n" \
"\n" \
"#include <exec/types.h>\n" \
"#include <exec/exec.h>\n" \
"#include <exec/interfaces.h>\n" \
"\n" \
"#include <interfaces/" #header_s  ".h>\n" \
"\n" \
"/* No inline4 macros are defined here. */\n" \
"\n" \
"#endif /* INLINE4_" #guard_s "_H */\n" \
)