/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: LibraryGenerator
**
** File: 
**
** Date: 16-06-2013 14:13:11
**
************************************************************

*/

#include <stdio.h>
#include <string.h>

#include <dos/dos.h>

#include <exec/types.h>
#include <exec/exectags.h>
#include <exec/lists.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>



static BOOL OpenLibs (void);
static void CloseLibs (void);

BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p);
BOOL ParseFiles (CONST_STRPTR pattern_s, CONST_STRPTR filename_s);

struct FunctionData *GetFunctionData (CONST_STRPTR line_s);
STRPTR GetReturnType (CONST_STRPTR *text_ss);


int main (int argc, char *argv [])
{
	if (OpenLibs ())
		{
			if (argc > 2)
				{
					ParseFiles (argv [1], argv [2]);
				}
			else
				{
					IDOS->Printf ("LibraryGenerator <pattern> <filename>\n");
				}
		
			CloseLibs ();		
		}
	else
		{
			printf ("no libs\n");
		}

	return 0;
}



BOOL GetMatchingPrototypes (CONST_STRPTR filename_s, CONST_STRPTR pattern_s, const size_t pattern_length, struct FReadLineData *line_p)
{
	BOOL success_flag = FALSE;
	BPTR handle_p = IDOS->FOpen (filename_s, MODE_OLDFILE, 0);

	if (handle_p)
		{
			int32 count;
			
			while ((count = IDOS->FReadLine (handle_p, line_p)) > 0)
				{
					if (IUtility->Strnicmp (pattern_s, line_p -> frld_Line, pattern_length) == 0)
						{
							IDOS->Printf (">>> matched line:= %s", line_p -> frld_Line); 
						} 
					else
						{
							IDOS->Printf ("line:= %s", line_p -> frld_Line); 
						}
				}
			
			success_flag = (count == 0);
			
			IDOS->FClose (handle_p);
		}		/* if (handle_p) */
	else
		{
			IDOS->Printf ("No handle for %s\n", filename_s);
		}

	return success_flag;
}



struct FunctionData *GetFunctionData (CONST_STRPTR line_s)
{
	struct FunctionData *fd_p = IExec->AllocVecTags (sizeof (struct FunctionData), TAG_DONE);
	
	if (fd_p)
		{
			struct List *list_p = IExec->AllocSysObject (ASOT_LIST, TAG_DONE);
			
			if (list_p)
				{
					CONST_STRPTR start_p = line_s;
					STRPTR return_s = GetReturnType (&start_p);
					
					if (return_s)
						{
						
						}
				
					IExec->FreeSysObject (ASOT_LIST, list_p);
				}
				
			IExec->FreeVec (fd_p);
		}
	
	return NULL;
}


STRPTR GetReturnType (CONST_STRPTR *text_ss)
{
	const size_t text_length = strlen (*text_ss);
	STRPTR buffer_s = IExec->AllocVecTags (text_length + 1, AVT_ClearWithValue, 0, TAG_DONE);
	
	if (buffer_s)
		{
			CONST_STRPTR text_p = text_ss;
			STRPTR buffer_text_p = buffer_s;
			
			/* 
				Copy all text from text_ss to buffer_s whilst concatenating
				all whitespace chunks to single null characters
			*/
			
			while (*text_p)
				{
					while ((*text_p != '\0') && (isspace (*text_p)))
						{
							++ text_p;
						}
				
					if (text_p)
						{
							*buffer_text_p = *text_p;	
							
							++ buffer_text_p;
							++ text_p;
						}
				}
			
			
			if (*text_p)
				{
					STRPTR compound_types [] = { "struct", "union", "enum", NULL };
					uint8 i = 0;
					BOOL loop_flag = (* (compound_types + i)) != NULL; 
					STRPTR compound_type_s = NULL;
					
					while (loop_flag)
						{
							size_t l = strlen (* (compound_types + i));
							
							if (strncmp (text_p, * (compound_types + i), l) == 0)
								{
									compound_type_s = * (compound_types + i);
									text_p += l;
									 
									loop_flag = FALSE;
								}
							else
								{ 	
									++ i;
								}
						}
					
					if (compound_type_s)
						{
						
					
				}
			
			
			
			IExec->FreeVec (buffer_s);
		}

	return NULL;
}





BOOL ParseFiles (CONST_STRPTR pattern_s, CONST_STRPTR filename_s)
{
	struct FReadLineData *line_data_p = IDOS->AllocDosObject (DOS_FREADLINEDATA, 0);
	BOOL success_flag = FALSE;
	
	if (line_data_p)
		{
			BOOL read_file_flag;
			const size_t pattern_length = strlen (pattern_s);
			
			read_file_flag = GetMatchingPrototypes (filename_s, pattern_s, pattern_length, line_data_p);

			if (read_file_flag)
				{
					IDOS->Printf ("read \"%s\" successfully\n", filename_s);
				}
			else
				{
					IDOS->Printf ("failed to read \"%s\"\n", filename_s);
				}
				

			IDOS->FreeDosObject (DOS_FREADLINEDATA, line_data_p);
		}		/* if (line_data_p) */

	return success_flag;
}


																

static BOOL OpenLibs (void)
{
	if (OpenLib (&DOSBase, "dos.library", 52L, (struct Interface **) &IDOS, "main", 1))
		{
			if (OpenLib (&UtilityBase, "utility.library", 52L, (struct Interface **) &IUtility, "main", 1))
				{
					return TRUE;
				} 		
				
			CloseLib (DOSBase, (struct Interface *) IDOS);    
		}     
		
	return FALSE;
}

static void CloseLibs (void)
{
	CloseLib (UtilityBase, (struct Interface *) IUtility);    
	CloseLib (DOSBase, (struct Interface *) IDOS);    
}

