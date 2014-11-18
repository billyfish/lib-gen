#include <ctype.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/utility.h>

#include "debugging_utils.h"
#include "utils.h"
#include "parameter.h"
#include "header_definitions.h"


#ifdef _DEBUG
#define UTILS_DEBUG (1)
#endif


static BOOL s_verbose_flag = FALSE;


void SetVerboseFlag (BOOL b)
{
	s_verbose_flag = b;
}


BOOL GetVerboseFlag (void)
{
	return s_verbose_flag;
}


BOOL EnsureDirectoryExists (CONST_STRPTR dir_s)
{
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

	return success_flag;
}



STRPTR MakeFilename (CONST_STRPTR first_s, CONST_STRPTR second_s)
{
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

	return result_s;
}



STRPTR ConcatenateStrings (CONST_STRPTR first_s, CONST_STRPTR second_s)
{
	const size_t l0 = strlen (first_s);
	const size_t l1 = strlen (second_s);
	STRPTR result_s = (STRPTR) IExec->AllocVecTags (l0 + l1 + 1, TAG_DONE);

	if (result_s)
		{
			strncpy (result_s, first_s, l0);
			strcpy (result_s + l0, second_s);
		}

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

			char *dest_p = (char *) IExec->AllocVec (len + 1, TAG_DONE);

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

	return NULL;
}


BOOL AddFullHeaderPathToList (struct List *header_definitions_p, CONST_STRPTR dir_s, CONST_STRPTR name_s)
{
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
					struct HeaderDefinitions *hdr_defs_p = AllocateHeaderDefinitions (full_path_s);

					#if UTILS_DEBUG >= 1
					DB (KPRINTF ("%s %ld - full_path_s \"%s\" dir \"%s\" name \"%s\"\n", __FILE__, __LINE__, full_path_s, dir_s, name_s));
					#endif

					if (hdr_defs_p)
						{
							if (AddHeaderDefintionsToList (header_definitions_p, hdr_defs_p))
								{
									success_flag = TRUE;

									#if UTILS_DEBUG >= 1
									DB (KPRINTF ("%s %ld - Added full_path_s \"%s\"\n", __FILE__, __LINE__, full_path_s));
									#endif
								}
							else
								{
									FreeHeaderDefinitions (hdr_defs_p);
									full_path_s = NULL;
								}
						}

				}

			if ((!success_flag) && full_path_s)
				{
					IExec->FreeVec (full_path_s);
				}
		}


	return success_flag;
}


int32 ScanDirectories (CONST_STRPTR dir_s, struct List *header_definitions_p, CONST_STRPTR filename_pattern_s, const BOOL recurse_flag)
{
	int32 success = FALSE;
	APTR context_p = IDOS->ObtainDirContextTags (EX_StringNameInput, dir_s,
		EX_DataFields, (EXF_NAME | EXF_LINK | EXF_TYPE),
		TAG_END);

	if (context_p)
		{
			struct ExamineData *dat_p;
			const BOOL verbose_flag = GetVerboseFlag ();

			if (verbose_flag)
				{
					IDOS->Printf ("Scanning %s\n", dir_s);
				}

			while ((dat_p = IDOS->ExamineDir (context_p)))
				{
					if (verbose_flag)
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
									if (AddFullHeaderPathToList (header_definitions_p, dir_s, dat_p -> Name))
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
											if (!ScanDirectories (path_s, header_definitions_p, filename_pattern_s, recurse_flag))  /* recurse */
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
	return success;
}
