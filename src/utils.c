#include <ctype.h>
#include <string.h>

#include "debugging_utils.h"
#include "memory.h"
#include "string_list.h"
#include "utils.h"
#include "parameter.h"


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

	if (start_p < end_p)
		{
			size_t len = end_p - start_p + 1;

			char *dest_p = (char *) AllocMemory (len + 1);

			DB (KPRINTF ("%s %ld - len %ld\n", __FILE__, __LINE__, len));

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


BOOL AddFullHeaderPathToList (struct List *headers_p, CONST STRPTR dir_s, CONST STRPTR name_s)
{
	BOOL success_flag = FALSE;
	STRPTR full_path_s = NULL;
	size_t l = 2;		/* terminating NULL and path separtor */
	const size_t dir_length = strlen (dir_s);

	l += dir_s;
	l += name_s;

	full_path_s = (STRPTR) IExec->AllocVecTags (l, TAG_END);

	if (full_path_s)
		{
			IUtility->Strlcpy (full_path_s, dir_s, dir_length);

			if (IDOS->AddPart (full_path_s, name_s, l) != 0)
				{
					StringNode *node_p = AllocazteStringNode (full_path_s);

					if (node_p)
						{
							IExec->AddTail (headers_p, (struct Node *) node_p);
							success_flag = TRUE;
						}
				}

			if (!success_flag)
				{
					IExec->FreeVec (full_path_s);
				}
		}

	return success_flag;
}


int32 RecursiveScan (STRPTR name_s, struct List *matching_files_list_p)
{
	int32 success = FALSE;
	APTR context_p = IDOS->ObtainDirContextTags (EX_StringNameInput, name_s,
		EX_DoCurrentDir, TRUE, /* for relative cd lock */
		EX_DataFields, (EXF_NAME | EXF_LINK | EXF_TYPE),
		TAG_END);

	if (context_p)
		{
			struct ExamineData *dat_p;

			while ((dat_p = IDOS->ExamineDir (context_p)))
				{
					if (EXD_IS_FILE (dat_p))
						{
							IDOS->Printf ("filename=%s\n", dat_p -> Name);
						}
					else if (EXD_IS_DIRECTORY (dat_p))
						{
							IDOS->Printf ("dirname=%s\n",  dat_p -> Name);

							if (!RecursiveScan (dat_p -> Name))  /* recurse */
								{
									break;
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


