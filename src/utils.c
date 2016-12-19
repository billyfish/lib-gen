#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <proto/dos.h>
#include <proto/utility.h>

#include "debugging_utils.h"
#include "utils.h"
#include "parameter.h"
#include "function_definition.h"


#ifdef _DEBUG
#define UTILS_DEBUG (1)
#endif


static enum Verbosity s_verbosity = VB_NORMAL;


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

	DB (KPRINTF ("%s %ld - Copying \"%s\" - \"%s\" to a new string\n", __FILE__, __LINE__, start_p ? start_p : "NULL", end_p ? end_p : "NULL"));

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


BOOL AddFullFilenameToList (struct List *filenames_p, CONST_STRPTR dir_s, CONST_STRPTR name_s)
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

			if ((!success_flag) && full_path_s)
				{
					IExec->FreeVec (full_path_s);
				}
		}


	LEAVE ();
	return success_flag;
}


int32 ScanDirectories (CONST_STRPTR dir_s, struct List *filenames_p, CONST_STRPTR filename_pattern_s, const BOOL recurse_flag)
{
	ENTER ();

	int32 success = FALSE;
	APTR context_p = IDOS->ObtainDirContextTags (EX_StringNameInput, dir_s,
		EX_DataFields, (EXF_NAME | EXF_LINK | EXF_TYPE),
		TAG_END);

	if (context_p)
		{
			struct ExamineData *dat_p;
			const enum Verbosity v = GetVerbosity ();

			if (v >= VB_NORMAL)
				{
					IDOS->Printf ("Scanning %s\n", dir_s);
				}

			while ((dat_p = IDOS->ExamineDir (context_p)))
				{
					if (v >= VB_LOUD)
						{
							IDOS->Printf ("filename=%s\n", dat_p -> Name);
						}

					#if UTILS_DEBUG >= 2
					DB (KPRINTF ("%s %ld - ScanDirectories; scanning \"%s\"\n", __FILE__, __LINE__, dat_p -> Name));
					#endif

					if (EXD_IS_FILE (dat_p))
						{
							BOOL add_flag = TRUE;

							if (filename_pattern_s)
								{
									add_flag = IDOS->MatchPatternNoCase (filename_pattern_s, dat_p -> Name);

									if (!add_flag)
										{
											#if UTILS_DEBUG >= 2
											DB (KPRINTF ("%s %ld - ScanDirectories; no match for %s\n", __FILE__, __LINE__, dat_p -> Name));
											#endif
										}
								}

							if (add_flag)
								{
									if (AddFullFilenameToList (filenames_p, dir_s, dat_p -> Name))
										{
											#if UTILS_DEBUG >= 2
											DB (KPRINTF ("%s %ld - ScanDirectories; added %s size %lu\n", __FILE__, __LINE__, dat_p -> Name, GetHeaderDefinitionsListSize (header_definitions_p)));
											#endif
										}
									else
										{
											IDOS->Printf ("failed to add filename=%s to list of headers files\n", dat_p -> Name);
										}
								}

						}
					else if (EXD_IS_DIRECTORY (dat_p))
						{
							if (recurse_flag)
								{
									STRPTR path_s = MakeFilename (dir_s, dat_p -> Name);

									if (path_s)
										{
											if (!ScanDirectories (path_s, filenames_p, filename_pattern_s, recurse_flag))  /* recurse */
												{
													break;
												}

											IExec->FreeVec (path_s);
										}
									else
										{
											IDOS->Printf ("ScanDirectories: Not enough memory to allocate filename\n");
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
					IDOS->PrintFault (IDOS->IoErr (), NULL); /* failure - why ? */
				}
		}
	else
		{
			IDOS->PrintFault (IDOS->IoErr (), NULL);  /* no context - why ? */
		}

	IDOS->ReleaseDirContext (context_p);          /* NULL safe */

	LEAVE ();
	return success;
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
									IDOS->Printf ("ScanDirectories: failed to copy %s to %s correctly, copied %ld instead of %ld\n", src_s, dest_s, dest_size, src_size);
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
	

