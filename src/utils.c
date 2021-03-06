#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <proto/dos.h>
#include <proto/utility.h>

#include "debugging_utils.h"
#include "utils.h"
#include "parameter.h"
#include "function_definition.h"
#include "list_utils.h"

#ifdef _DEBUG
#define UTILS_DEBUG (3)
#endif


static enum Verbosity s_verbosity = VB_NORMAL;

static BOOL s_newlib_flag = FALSE;


static BOOL IsPathValid (CONST_STRPTR path_s, struct List *paths_to_ignore_p);



void SetVerbosity (enum Verbosity v)
{
	ENTER ();

	s_verbosity = v;

	LEAVE ();
}


enum Verbosity GetVerbosity (void)
{
	ENTER ();

	LEAVE ();
	return s_verbosity;
}


void SetNewlibNeeded (const BOOL value)
{
	s_newlib_flag = value;
}


BOOL IsNewlibNeeded (void)
{
	return s_newlib_flag;
}


BOOL EnsureDirectoryExists (CONST_STRPTR dir_s)
{
	ENTER ();

	BOOL success_flag = FALSE;
	struct ExamineData *dat_p = IDOS->ExamineObjectTags (EX_StringNameInput, dir_s, TAG_END);

	/* check if it already exists */
	if (dat_p)
		{
		  success_flag = EXD_IS_DIRECTORY (dat_p);
		  IDOS->FreeDosObject (DOS_EXAMINEDATA, dat_p);
		}
	else
		{
			BPTR lock_p = IDOS->CreateDirTree (dir_s);

			if (lock_p)
				{
					IDOS->UnLock (lock_p);
					success_flag = TRUE;
				}
		}

	LEAVE ();
	return success_flag;
}



STRPTR MakeFilename (CONST_STRPTR first_s, CONST_STRPTR second_s)
{
	ENTER ();

	const size_t l0 = strlen (first_s);
	const size_t l = l0 + strlen (second_s) + 2;

	STRPTR result_s = (STRPTR) IExec->AllocVecTags (l, TAG_DONE);

	if (result_s)
		{
			strcpy (result_s, first_s);

			if (IDOS->AddPart (result_s, second_s, l) == 0)
				{
					IExec->FreeVec (result_s);
					result_s = NULL;
				}
		}

	LEAVE ();
	return result_s;
}


STRPTR GetParentName (CONST_STRPTR filename_s)
{
	STRPTR parent_name_s = NULL;
	
	ENTER ();
	STRPTR parent_p = strrchr (filename_s, '/');
	
	if (!parent_p)
		{
			parent_p = strrchr (filename_s, ':');
		}

	if (parent_p)
		{
			parent_name_s = CopyToNewString (filename_s, parent_p, FALSE);
		}
	else
		{
			
		}		
	
	LEAVE ();
	
	return parent_name_s;
}


STRPTR ConcatenateStrings (CONST_STRPTR first_s, CONST_STRPTR second_s)
{
	ENTER ();

	const size_t l0 = strlen (first_s);
	const size_t l1 = strlen (second_s);
	STRPTR result_s = (STRPTR) IExec->AllocVecTags (l0 + l1 + 1, TAG_DONE);

	if (result_s)
		{
			strncpy (result_s, first_s, l0);
			strcpy (result_s + l0, second_s);
		}

	LEAVE ();
	return result_s;
}


STRPTR GetSourceFilename (CONST CONST_STRPTR prefix_s, CONST CONST_STRPTR header_filename_s, char file_suffix)
{
	STRPTR result_s = NULL;
	CONST_STRPTR filename_s;
	
	ENTER ();
	
	/* Get the .c filename */
	filename_s = IDOS->FilePart (header_filename_s);

	if (filename_s)
		{
			size_t prefix_length = strlen (prefix_s);
			size_t filename_length = strlen (filename_s);
			
			result_s = (STRPTR) (IExec->AllocVecTags (prefix_length + filename_length + 2, TAG_END));
			
			if (result_s)
				{
					STRPTR suffix_s = NULL;
					
					strncpy (result_s, prefix_s, prefix_length);
					* (result_s + prefix_length) = '_';
					strncpy (result_s + prefix_length + 1, filename_s, filename_length);	
					* (result_s + prefix_length + filename_length + 1) = '\0';
					
					suffix_s = strrchr (result_s, '.');
					
					if (suffix_s)
						{
							++ suffix_s;
							
							if (*suffix_s != '\0')
								{
									*suffix_s = file_suffix;
									
								}
						}
				}
			
		
		}
	else
		{
			DB (KPRINTF ("%s %ld - Failed to get  header filename %s\n", __FILE__, __LINE__, header_filename_s));
		}

	LEAVE ();
	return result_s;
}


/**
 * Copy a string to a newly created string.
 *
 * @param src_p The string to copy.
 * @param trim Whether to trim left and right trailing whitespace or not.
 * @return A newly allocated copy of the source string or NULL if there was an error.
 */
char *CopyToNewString (const char *start_p, const char *end_p, const BOOL trim_flag)
{
	ENTER ();

	if (trim_flag)
		{
			BOOL loop_flag = TRUE;

			while (loop_flag)
				{
					if (isspace (*start_p))
						{
							++ start_p;
							loop_flag = (start_p < end_p);
						}
					else
						{
							loop_flag = FALSE;
						}
				}

			if (end_p)
				{
					loop_flag = start_p < end_p;
					while (loop_flag)
						{
							if (isspace (*end_p))
								{
									-- end_p;
									loop_flag = (start_p < end_p);
								}
							else
								{
									loop_flag = FALSE;
								}
						}
				}
		}

	#if UTILS_DEBUG > 4
	DB (KPRINTF ("%s %ld - Copying \"%s\" - \"%s\" to a new string\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));
	#endif
	
	if (start_p <= end_p)
		{
			size_t len = end_p - start_p + 1;

			char *dest_p = (char *) IExec->AllocVecTags (len + 1, TAG_DONE);

			//DB (KPRINTF ("%s %ld - len %ld\n", __FILE__, __LINE__, len));

			if (dest_p)
				{
					memcpy (dest_p, start_p, len);
					* (dest_p + len) = '\0';

					return dest_p;
				}
		}
	else
		{
			DB (KPRINTF ("%s %ld - ERROR: \"%s\" is after \"%s\"\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));
		}

	LEAVE ();
	return NULL;
}


BOOL AddFullFilenameToList (struct List *filenames_p, CONST_STRPTR dir_s, CONST_STRPTR name_s, struct List *paths_to_ignore_p)
{
	ENTER ();

	BOOL success_flag = FALSE;
	STRPTR full_path_s = NULL;
	size_t l = 2;		/* terminating NULL and path separtor */
	const size_t dir_length = strlen (dir_s);

	l += dir_length;
	l += strlen (name_s);

	full_path_s = (STRPTR) IExec->AllocVecTags (l, TAG_END);

	if (full_path_s)
		{
			IUtility->Strlcpy (full_path_s, dir_s, dir_length + 1);

			if (IDOS->AddPart (full_path_s, name_s, l) != 0)
				{
					if (IsPathValid (full_path_s, paths_to_ignore_p))
						{
							struct Node *node_p = IExec->AllocSysObjectTags (ASOT_NODE,
								ASONODE_Name, full_path_s,
								ASONODE_Size, sizeof (struct Node),
								TAG_DONE);
		
							#if UTILS_DEBUG >= 1
							DB (KPRINTF ("%s %ld - full_path_s \"%s\" dir \"%s\" name \"%s\"\n", __FILE__, __LINE__, full_path_s, dir_s, name_s));
							#endif
		
							if (node_p)
								{
									IExec->AddTail (filenames_p, node_p);
									success_flag = TRUE;
								}
						}

				}

			if ((!success_flag) && full_path_s)
				{
					IExec->FreeVec (full_path_s);
				}
		}


	LEAVE ();
	return success_flag;
}


static BOOL CheckAndAddHeaderFile (CONST_STRPTR filename_s, CONST_STRPTR dir_s, CONST_STRPTR filename_pattern_s, struct List *filenames_p, struct List *paths_to_ignore_p)
{
	BOOL success_flag = TRUE;
	BOOL add_flag = TRUE;

	ENTER ();
				
	if (filename_pattern_s)
		{
			add_flag = IDOS->MatchPatternNoCase (filename_pattern_s, filename_s);

			if (!add_flag)
				{
					#if UTILS_DEBUG >= 2
					DB (KPRINTF ("%s %ld - ScanDirectories; no match for %s\n", __FILE__, __LINE__, filename_s));
					#endif
				}
		}

	if (add_flag)
		{
			if (AddFullFilenameToList (filenames_p, dir_s, filename_s, paths_to_ignore_p))
				{
					#if UTILS_DEBUG >= 2
					DB (KPRINTF ("%s %ld - ScanDirectories; added %s size %lu\n", __FILE__, __LINE__, filename_s, GetListSize (filenames_p)));
					#endif
				}
			else
				{
					IDOS->Printf ("failed to add filename=%s to list of headers files\n", filename_s);
					success_flag = FALSE;
				}
		}
	
	LEAVE ();	
		
	return success_flag;
}


int32 ScanPath (CONST_STRPTR path_s, struct List *filenames_p, CONST_STRPTR filename_pattern_s, const BOOL recurse_flag, struct List *paths_to_ignore_p)
{
	ENTER ();

	int32 success = FALSE;
	
	IDOS->Printf ("*** ObtainDirContextTags with path \"%s\"\n", path_s);
	struct ExamineData *data_p = IDOS->ExamineObjectTags (EX_StringNameInput, path_s, TAG_END);
	
	if (data_p)
		{
			if (EXD_IS_FILE (data_p))
				{
					STRPTR dir_s = GetParentName (path_s);
					
					if (dir_s)
						{
							CONST_STRPTR filename_s = IDOS->FilePart (path_s);
							
							if (CheckAndAddHeaderFile (filename_s, dir_s, filename_pattern_s, filenames_p, paths_to_ignore_p))
								{
									success = TRUE;
								}
							else
								{
									IDOS->Printf ("Failed to add header file \"%s\"\n", path_s);
								}
								
							IExec->FreeVec (dir_s);
						}
				}
			else if (EXD_IS_DIRECTORY (data_p))
				{
					APTR context_p = NULL;
					IDOS->Printf ("ObtainDirContextTags for \"%s\"\n", path_s);
					
					context_p = IDOS->ObtainDirContextTags (EX_StringNameInput, path_s,
						EX_DataFields, (EXF_NAME | EXF_LINK | EXF_TYPE),
						TAG_END);
				
					if (context_p)
						{
							struct ExamineData *dat_p;
							const enum Verbosity v = GetVerbosity ();
				
							if (v >= VB_LOUD)
								{
									IDOS->Printf ("Scanning \"%s\" with pattern %s\n", data_p -> Name, filename_pattern_s);
								}
				
							/*
								context_p takes care of deleting the returned dat_p objects
								when we call ReleaseDirContext, so we mustn't call 
								FreeDosObject on them
							*/
							while ((dat_p = IDOS->ExamineDir (context_p)))
								{
									if (v >= VB_LOUD)
										{
											IDOS->Printf ("filename=%s\n", dat_p -> Name);
										}
				
									#if UTILS_DEBUG >= 2
									DB (KPRINTF ("%s %ld - ScanPath; scanning \"%s\"\n", __FILE__, __LINE__, dat_p -> Name));
									#endif
				
									if (EXD_IS_FILE (dat_p))
										{
											if (CheckAndAddHeaderFile (dat_p -> Name, path_s, filename_pattern_s, filenames_p, paths_to_ignore_p))
												{
													success = TRUE;
												}
											else
												{
													IDOS->Printf ("Failed to add header file \"%s\" in \"%s\"\n", dat_p -> Name, path_s);
												}			
										}
									else if (EXD_IS_DIRECTORY (dat_p))
										{
											if (recurse_flag)
												{
													STRPTR new_path_s = MakeFilename (path_s, dat_p -> Name);
				
													if (new_path_s)
														{
															IDOS->Printf ("Got new path \"%s\"\n", new_path_s);
															
															if (IsPathValid (new_path_s, paths_to_ignore_p))
																{
																	if (!ScanPath (new_path_s, filenames_p, filename_pattern_s, recurse_flag, paths_to_ignore_p))  /* recurse */
																		{
																			break;
																		}
																}
															else
																{
																	IDOS->Printf ("IsPathValid ignoring \"%s\"\n", new_path_s);
																}
				
															IExec->FreeVec (new_path_s);
														}
													else
														{
															IDOS->Printf ("MakeFilename failed for \"%s\" and \"%s\"\n", path_s, dat_p -> Name);
														}
												}
										}
								}
				
							if (ERROR_NO_MORE_ENTRIES == IDOS->IoErr ())
								{
									success = TRUE;           /* normal exit */
								}
							else
								{
									IDOS->Printf ("Failed to obtain directory context for \"%s\"\n", data_p -> Name);
									IDOS->PrintFault (IDOS->IoErr (), NULL); /* failure - why ? */
								}						
											
							IDOS->ReleaseDirContext (context_p);          /* NULL safe */
						}		/* if (context_p) */
					else
						{
							IDOS->Printf ("oops\n");
							IDOS->PrintFault (IDOS->IoErr (), NULL);  /* no context - why ? */
						}					
					
				}	/* else if (EXD_IS_DIRECTORY (data_p)) */	
		
			IDOS->FreeDosObject (DOS_EXAMINEDATA, data_p);
		}		/* if (data_p) */
	
	LEAVE ();
	return success;
}


static BOOL IsPathValid (CONST_STRPTR path_s, struct List *paths_to_ignore_p)
{
	BOOL valid_flag = TRUE;
	
	ENTER ();
	
	if (paths_to_ignore_p && valid_flag)
		{
			struct Node *curr_node_p = IExec->GetHead (paths_to_ignore_p);
		
			while (curr_node_p)
				{
					struct Node *next_node_p = IExec->GetSucc (curr_node_p);
					
					if (strcmp (path_s, curr_node_p -> ln_Name) == 0)
						{
							valid_flag = FALSE;
						}
												
					curr_node_p = next_node_p;
				}		/* while (curr_node_p) */			
			
		}
	
	
	LEAVE ();
	
	return valid_flag;
}



BOOL CopyFile (CONST CONST_STRPTR src_s, CONST CONST_STRPTR dest_s)
{
	BOOL success_flag = FALSE;
	BPTR src_f;
	
	ENTER ();
	
	src_f = IDOS->FOpen (src_s, MODE_OLDFILE, 0);
	
	if (src_f)
		{
			BPTR dest_f = IDOS->FOpen (dest_s, MODE_NEWFILE, 0);

			if (dest_f)
				{
					BOOL loop_flag = TRUE;
					const int32 buffer_size = 1024;
					char buffer_p [buffer_size];
					
					success_flag = TRUE;
					
					while (loop_flag && success_flag)
						{
							int32 res = IDOS->Read (src_f, buffer_p, buffer_size);
							
							if (res > 0)
								{
									IDOS->Write (dest_f, buffer_p, res);
								}
							else if (res == 0)
								{
									/* EOF */
									loop_flag = FALSE;
								}
							else if (res == -1)
								{
									/* error */
									success_flag = FALSE;
								}
													
						}
					
					
					if (success_flag)
						{
							int64 src_size = IDOS->GetFileSize (src_f);
							int64 dest_size = IDOS->GetFileSize (dest_f);
						
							if (src_size != dest_size)
								{
									IDOS->Printf ("CopyFile: failed to copy %s to %s correctly, copied %ld instead of %ld\n", src_s, dest_s, dest_size, src_size);
								}
						
							
						}
			
					IDOS->FClose (dest_f);
				}	
			
			IDOS->FClose (src_f);
		}
	
	LEAVE ();
	
	return success_flag;
}


BOOL PrintUpperCase (BPTR out_p, CONST_STRPTR value_s)
{
	ENTER ();
		
	if (value_s)
		{
			while (*value_s != '\0')
				{
					char c = toupper (*value_s);
					
					DB (KPRINTF ("%s %ld - Converting from \"%s\" %d\n", __FILE__, __LINE__, value_s, (int) c));
					
					if (IDOS->FPrintf (out_p, "%lc", c) >= 0)
						{
							++ value_s;
						}
					else
						{
							return FALSE;
						}
				}
			
		}
	
	LEAVE ();
	
	return TRUE;
}


BOOL PrintCaptitalizedString (BPTR out_p, CONST_STRPTR value_s)
{
	BOOL success_flag = FALSE;
	size_t l;
	
	ENTER ();

	l = strlen (value_s);
	
	if (l > 0)
		{
			char c = toupper (*value_s);		
			
			if (l > 1)
				{
					success_flag = (IDOS->FPrintf (out_p, "%lc%s", c, value_s + 1) >= 0);	
				}
			else
				{
					success_flag = (IDOS->FPrintf (out_p, "%lc", c) >= 0);		
				}	
		}		
	
	LEAVE ();
	
	return success_flag;
}


STRPTR GetInterfaceName (CONST CONST_STRPTR library_s)
{
	STRPTR interface_s = NULL;
	
	ENTER ();
	
	interface_s = ConcatenateStrings (library_s, "IFace");
	
	if (interface_s)
		{
			/* Captialize the interface */
			*interface_s = toupper (*interface_s);	
		}

	LEAVE ();
	return interface_s;
}


STRPTR GetGlobalInterfaceName (CONST CONST_STRPTR library_s)
{
	STRPTR interface_s = NULL;
	
	ENTER ();
	
	interface_s = ConcatenateStrings ("I", library_s);
	
	if (interface_s)
		{
			/* Captialize the first letter after the "I" */
			* (interface_s + 1) = toupper (* (interface_s + 1));	
		}

	LEAVE ();
	return interface_s;
}


STRPTR GetUpperCaseString (CONST_STRPTR src_s)
{
	STRPTR dest_s = NULL;
	
	ENTER ();
	
	if (src_s)
		{
			size_t l = strlen (src_s);	
			
			dest_s = (STRPTR) IExec->AllocVecTags (l + 1, TAG_END);
			
			if (dest_s)
				{
					STRPTR value_s = dest_s;
					
					while (*src_s != '\0')
						{
							*value_s = toupper (*src_s);
							
							++ src_s;
							++ value_s;
						}
						
					*value_s = '\0';
				}
		}
	
	
	LEAVE ();
	
	return dest_s;
}


STRPTR GetCapitalizedString (CONST_STRPTR src_s)
{
	STRPTR dest_s = NULL;
	
	ENTER ();
	
	if (src_s)
		{
			size_t l = strlen (src_s);	
			
			dest_s = (STRPTR) IExec->AllocVecTags (l + 1, TAG_END);
			
			if (dest_s)
				{
					strcpy (dest_s, src_s); 
					
					*dest_s = toupper (*dest_s);
					* (dest_s + l) = '\0';
				}
		}
	
	
	LEAVE ();
	
	return dest_s;
}


void ReplaceChars (STRPTR value_s, CONST CONST_STRPTR old_values_s, const char new_value)
{
	while (*value_s != '\0')
		{
			if (strchr (old_values_s, *value_s))
				{
					*value_s = new_value;	
				}
		
			++ value_s;	
		}

}
	
	
STRPTR ScrollPastWhitespace (CONST_STRPTR data_s)
{
	CONST_STRPTR start_s = NULL;
	
	ENTER ();


	start_s = data_s;

	if (start_s)
		{
			/*
				Check to see whether possible function prototype 
			*/	
			BOOL loop_flag = TRUE;
			
			/* scroll to the start of the next token */		
			while (loop_flag)
				{
					if (*start_s == '\0')
						{
							start_s = NULL;
							loop_flag = FALSE;
						}
					else if (isspace (*start_s))
						{
							++ start_s;
						}							
					else
						{
							loop_flag = FALSE;
						}		
				}
		
		}		/* if (start_s) */	


	LEAVE ();
	
	return (STRPTR) start_s;
}	



STRPTR CreateRegEx (CONST_STRPTR pattern_s, BOOL capture_flag)
{
	STRPTR reg_ex_s = NULL;

	ENTER ();

	if (pattern_s)
		{
			size_t l = (2 * strlen (pattern_s)) + 2;

			reg_ex_s = (STRPTR) IExec->AllocVecTags (l, TAG_DONE);

			if (reg_ex_s)
				{
					int32 is_wild;

					if (capture_flag)
						{
							is_wild = IDOS->ParseCapturePattern (pattern_s, reg_ex_s, l, TRUE);
						}
					else
						{
							is_wild = IDOS->ParsePatternNoCase (pattern_s, reg_ex_s, l);
						}

					if (is_wild < 0)
						{
							IDOS->Printf ("Error creating pattern from \"%s\"\n", pattern_s);
						}
				}
			else
				{
					IDOS->Printf ("Not enough memory to create regular expression from \"%s\"\n", pattern_s);
				}
		}

	LEAVE ();
	return reg_ex_s;
}
