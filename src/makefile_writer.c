#include <proto/dos.h>
#include <proto/exec.h>

#include <string.h>
#include <stdlib.h>

#include "makefile_writer.h"
#include "function_definition.h"
#include "utils.h"
#include "debugging_utils.h"

#include "makefile_utils.h"



STATIC BOOL WriteMakefileAmigaSources (BPTR makefile_p, CONST CONST_STRPTR library_s, struct List * const src_files_p);

STATIC BOOL WriteMakefileHeader (BPTR makefile_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s, struct List *function_defs_p);

STATIC BOOL WriteMakefileFooter (BPTR makefile_p, CONST CONST_STRPTR library_s);

STATIC BOOL WriteMakefileOriginalSources (BPTR makefile_p, struct List * const original_filenames_p);

STATIC BOOL WriteOriginalLibraryIncludePaths (BPTR makefile_p, struct List * const function_defs_p);



BOOL WriteMakefile (CONST CONST_STRPTR makefile_s, CONST CONST_STRPTR root_path_s, CONST CONST_STRPTR library_s, struct List * const function_defs_p, struct List *original_source_filenames_p)
{
	ENTER ();
	BOOL success_flag = FALSE;
	BPTR makefile_p = IDOS->FOpen (makefile_s, MODE_NEWFILE, 0);

	if (makefile_p)
		{
			STRPTR src_dir_s = ConcatenateStrings (library_s, SOURCE_DIR_SUFFIX_S);

			if (src_dir_s)
				{
					if (WriteMakefileHeader (makefile_p, library_s, src_dir_s, function_defs_p))
						{
							if (WriteMakefileAmigaSources (makefile_p, library_s, function_defs_p))
								{
									if (WriteMakefileOriginalSources (makefile_p, original_source_filenames_p))
										{
											if (IDOS->FPrintf (makefile_p, "\n\n%s_SRCS = $(ORIGINAL_LIB_SRC) $(AMIGA_LIB_SRC)\n\n\n", library_s) >= 0)
												{
													if (WriteMakefileFooter (makefile_p, library_s))
														{
															success_flag = TRUE;
														}
													else
														{
															IDOS->Printf ("Failed to write footer block to makefile \"%s\"\n", makefile_s);
														}
												}
											else
												{
													IDOS->Printf ("Failed to write combined sources block to makefile \"%s\"\n", makefile_s);
												}
								
										}
									else
										{
											IDOS->Printf ("Failed to write original sources block to makefile \"%s\"\n", makefile_s);
										}
								}
							else
								{
									IDOS->Printf ("Failed to write amiga sources block to makefile \"%s\"\n", makefile_s);
								}
						}
					else
						{
							IDOS->Printf ("Failed to write header block to makefile \"%s\"\n", makefile_s);
						}


					IExec->FreeVec (src_dir_s);
				}
			else
				{
					IDOS->Printf ("Failed to create sources directory name \"%s\"\n", makefile_s);
				}

			if (IDOS->FClose (makefile_p) == 0)
				{
					IDOS->Printf ("Failed to close makefile \"%s\"\n", makefile_s);
				}
		}
	else
		{
			IDOS->Printf ("Failed to open makefile \"%s\"\n", makefile_s);
		}

	LEAVE ();
	return success_flag;
}


STATIC BOOL WriteMakefileHeader (BPTR makefile_p, CONST CONST_STRPTR library_s, CONST CONST_STRPTR output_dir_s, struct List *function_defs_p)
{
	ENTER ();
	BOOL success_flag = FALSE;
	
	
	CONST CONST_STRPTR part0_s =
		"# Adapted by libgen and based upon the makefile\n" \
		"# automatically generated by idltool\n" \
		"# \n" \
		"# $Id$\n" \
		"# \n" \
		"\n" \
		"# Uncomment the next line if you are cross compiling\n" \
		"# CROSS_COMPILE = ppc-amigaos-\n" \
		"\n" \
		"CC     = $(CROSS_COMPILE)gcc \n" \
		"CXX    = $(CROSS_COMPILE)c++ \n" \
		"AS     = $(CROSS_COMPILE)as \n" \
		"LD     = $(CROSS_COMPILE)ld \n" \
		"RANLIB = $(CROSS_COMPILE)ranlib \n" \
		"AR     = $(CROSS_COMPILE)ar \n" \
		"#RM     = delete\n" \
		"CP     = copy\n" \
		"RM     = rm -f\n" \
		"\n" \
		"# Change these as required\n";
		
		
	CONST CONST_STRPTR part1_s = "OPTIMIZE = -O3\n" \
		"DEBUG    = # -gstabs -DDEBUG\n" \
		"CFLAGS   = -Wall $(OPTIMIZE) $(DEBUG) -I$(DIR_AMIGA_LIB_SRC)/include -I$(DIR_AMIGA_LIB_SRC)\n" \
		"\n" \
		"# Flags passed to gcc during linking\n" \
		"LINK = \n" \
		"\n" \
		"# Name of the \"thing\" to build\n" \
		"TARGET = ";

	CONST CONST_STRPTR part2_s =
		".library\n" \
		"\n" \
		"# Additional linker libraries\n" \
		"LIBS = \n" \
		"\n" \
		"# Version of the library to build\n" \
		"VERSION = 53\n" \
		"\n";

	if (IDOS->FPrintf (makefile_p, 
		"# Makefile for project %s\n"
		"%sDIR_AMIGA_LIB_SRC = %s\n"
		"%s%s%s", 
		library_s, part0_s, output_dir_s, part1_s, library_s, part2_s) >= 0)
		{
			success_flag = WriteOriginalLibraryIncludePaths (makefile_p, function_defs_p);
		}

	LEAVE ();
	return success_flag;
}

/*
 Get a list of all of the include dircteories to include
*/
STATIC BOOL WriteOriginalLibraryIncludePaths (BPTR makefile_p, struct List * const function_defs_p)
{
	BOOL success_flag = (IDOS->FPrintf (makefile_p, "\n# Adding the include paths for the original library\n") > 0);	

	ENTER ();	

	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_defs_p);
	STRPTR old_parent_s = NULL;
	
	
	
	while (node_p && success_flag)
		{
			STRPTR parent_s = GetParentName (node_p -> fdn_function_def_p -> fd_header_filename_s);
			
			if (parent_s)
				{
					if ((!old_parent_s) || (strcmp (old_parent_s, parent_s) != 0))	
						{
							if (GetVerbosity () >= VB_LOUD)
								{
									IDOS->Printf ("Adding \"%s\" to original include path from \"%s\"\n", parent_s, node_p -> fdn_function_def_p -> fd_header_filename_s);
								}
								
							success_flag = (IDOS->FPrintf (makefile_p, "CFLAGS += -I%s\n", parent_s) >= 0);
						}
						
					if (old_parent_s)
						{
							IExec->FreeVec (old_parent_s);
						}
						
					old_parent_s = parent_s;
				}
			

			if (success_flag)
				{
					node_p = (struct FunctionDefinitionNode *) IExec->GetSucc (& (node_p -> fdn_node));
				}
		}	
	
	
	if (old_parent_s)
		{
			IExec->FreeVec (old_parent_s);
		}
	
	
	LEAVE ();
	
	return success_flag;
}


STATIC BOOL WriteMakefileFooter (BPTR makefile_p, CONST CONST_STRPTR library_s)
{
	ENTER ();
	BOOL success_flag = FALSE;
	
	if (IDOS->FPrintf (makefile_p,
		"# -------------------------------------------------------------\n" \
		"# Nothing should need changing below this line\n" \
		"\n" \
		"OBJS = $(%s_SRCS:.c=.o)\n" \
		"# Rules for building\n\n" \
		".PHONY: all\n" \
		"all: clean revision lib autoinit \n\n" \
		".PHONY: lib\n" \
		"lib: $(DIR_AMIGA_LIB_SRC)/$(TARGET)\n\n" \
		"$(DIR_AMIGA_LIB_SRC)/$(TARGET): $(OBJS)\n" \
		"	$(CC) $(LINK) -nostartfiles -o $(TARGET) $(OBJS) $(LIBS)\n" \
		"\n" \
		".PHONY: clean\n" \
		"clean:\n" \
		"	$(RM) $(TARGET) $(OBJS)\n" \
		"\n" \
		".PHONY: revision\n" \
		"revision:\n" \
		"\tbumprev $(VERSION) $(DIR_AMIGA_LIB_SRC)/$(TARGET)\n\n" \
		".PHONY: autoinit\n" \
		"autoinit:\n" \
		"\t$(CC) $(CFLAGS) $(DIR_AMIGA_LIB_SRC)/%s_interface_auto_init.c -c -o $(DIR_AMIGA_LIB_SRC)/%s_interface_auto_init.o\n" \
		"\t$(CC) $(CFLAGS) $(DIR_AMIGA_LIB_SRC)/%s_library_auto_init.c -c -o $(DIR_AMIGA_LIB_SRC)/%s_library_auto_init.o\n" \
		"\t$(AR) -crv -o lib%s_auto.a $(DIR_AMIGA_LIB_SRC)/%s_library_auto_init.o $(DIR_AMIGA_LIB_SRC)/%s_interface_auto_init.o\n" \
		"\t$(CP) lib%s_auto.a SDK:local/newlib/lib\n\n", 
		library_s,
		library_s, library_s, library_s, library_s, 
		library_s, library_s, library_s,
		library_s) >= 0)
		{	
			success_flag = TRUE;
		}
		

	LEAVE ();
	return success_flag;
}


STATIC BOOL WriteMakefileAmigaSources (BPTR makefile_p, CONST CONST_STRPTR library_s, struct List * const function_defs_p)
{
	ENTER ();

	struct FunctionDefinitionNode *node_p = (struct FunctionDefinitionNode *) IExec->GetHead (function_defs_p);
	CONST_STRPTR current_source_filename_s = "";
	BOOL success_flag = (IDOS->FPrintf (makefile_p, 
		"\n\n#Add the library initialization code\n"
		"AMIGA_LIB_SRC = \\\n\t$(DIR_AMIGA_LIB_SRC)/init.c \\\n"
		"\t$(DIR_AMIGA_LIB_SRC)/lib_init.c\n") >= 0);
		
	if (success_flag)
		{
			success_flag = (IDOS->FPrintf (makefile_p,
				"\nAUTO_INIT_LIB_SRC = \\\n"
				"\t$(DIR_AMIGA_LIB_SRC)/library_auto_init.c\\\n"
				"\t$(DIR_AMIGA_LIB_SRC)/interface_auto_init.c\\\n\n") >= 0);
							
				
			if (success_flag)
				{
					if (IDOS->FPrintf (makefile_p, "#Add the source files\n") >= 0)
						{
							while (node_p && success_flag)
								{
									CONST_STRPTR function_source_filename_s = node_p -> fdn_function_def_p -> fd_header_filename_s;
						
									if (strcmp (current_source_filename_s, function_source_filename_s) != 0)
										{
											STRPTR c_filename_s = GetSourceFilename (library_s, function_source_filename_s, 'c');
						
											if (c_filename_s)
												{							
													success_flag = IDOS->FPrintf (makefile_p, "AMIGA_LIB_SRC += $(DIR_AMIGA_LIB_SRC)/%s\n", c_filename_s) >= 0;
													
													if (success_flag)
														{
															current_source_filename_s = function_source_filename_s;
														}
													else
														{
															IDOS->Printf ("Failed to write src filename %s to makefile\n", c_filename_s);
														}
						
													IExec->FreeVec (c_filename_s);
												}
											else
												{
													IDOS->Printf ("Failed to make src filename from %s\n", function_source_filename_s);
													success_flag = FALSE;
												}
						
										}
						
									if (success_flag)
										{
											node_p = (struct FunctionDefinitionNode *) IExec->GetSucc (& (node_p -> fdn_node));
										}
								}							
						}
					else
						{
							success_flag = FALSE;	
						}
				}
		}	
		
	


	LEAVE ();
	return success_flag;
}



STATIC BOOL WriteMakefileOriginalSources (BPTR makefile_p, struct List * const original_filenames_p)
{
	ENTER ();

	struct Node *node_p = IExec->GetHead (original_filenames_p);
	BOOL success_flag = (IDOS->FPrintf (makefile_p,"\n\n#Add the original source files\n") >= 0);
	BOOL first_time_flag = TRUE;
	
	while (node_p && success_flag)
		{
			if (first_time_flag)
				{
					success_flag = IDOS->FPrintf (makefile_p, "ORIGINAL_LIB_SRC = %s\n", node_p -> ln_Name) >= 0;
					first_time_flag = FALSE;
				}
			else
				{
					success_flag = IDOS->FPrintf (makefile_p, "ORIGINAL_LIB_SRC += %s\n", node_p -> ln_Name) >= 0;
				}
					
			if (success_flag)
				{
					node_p = IExec->GetSucc (node_p);
				}
		}

	LEAVE ();
	return success_flag;
}
