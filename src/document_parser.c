#include <string.h>
#include <stdlib.h>
#include <stdio.h>



#include <proto/dos.h>
#include <proto/exec.h>


#include "document_parser.h"
#include "utils.h"
#include "debugging_utils.h"

#ifdef _DEBUG
#define DOCUMENT_PARSER_DEBUG (11)
#endif


struct DocumentParser *AllocateDocumentParser (void)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			struct FReadLineData *line_data_p = IDOS->AllocDosObjectTags (DOS_FREADLINEDATA, TAG_DONE);

			if (line_data_p)
				{
					struct DocumentParser *parser_p = (struct DocumentParser *) IExec->AllocVecTags (sizeof (struct DocumentParser), TAG_DONE);

					if (parser_p)
						{
							parser_p -> dp_buffer_p = buffer_p;
							parser_p -> dp_line_p = line_data_p;
							parser_p -> dp_comment_flag = FALSE;
							parser_p -> dp_matched_flag = FALSE;
							parser_p -> dp_file_handle_p = ZERO;
							parser_p -> dp_line_number = 0;

							return parser_p;
						}

					IDOS->FreeDosObject (DOS_FREADLINEDATA, line_data_p);
				}
			FreeByteBuffer (buffer_p);
		}

	return NULL;
}


void FreeDocumentParser (struct DocumentParser *parser_p)
{
	IDOS->FreeDosObject (DOS_FREADLINEDATA, parser_p -> dp_line_p);
	FreeByteBuffer (parser_p -> dp_buffer_p);
	IExec->FreeVec (parser_p);
}


void SetDocumentToParse (struct DocumentParser *parser_p, BPTR handle_p)
{
	parser_p -> dp_file_handle_p = handle_p;
	parser_p -> dp_line_number = 0;
}


int32 ParseLine (struct DocumentParser *parser_p)
{
	int32 count = IDOS->FReadLine (parser_p -> dp_file_handle_p, parser_p -> dp_line_p);
	++ (parser_p -> dp_line_number);

	return count;
}


int32 GetNextPrototype (struct DocumentParser *parser_p, STRPTR *prototype_ss)
{
	int32 count = 0;
	BOOL loop_flag = TRUE;

	while (loop_flag)
		{
			count = ParseLine (parser_p);

			#if DOCUMENT_PARSER_DEBUG > 10
			DB (KPRINTF ("%s %ld - count %ld \"%s\"\n", __FILE__, __LINE__, count, parser_p -> dp_line_p -> frld_Line));
			#endif

			if (count > 0)
				{
					char *prototype_s = ParseDocument (parser_p);

					if (prototype_s)
						{
							*prototype_ss = prototype_s;
							loop_flag = FALSE;
						}
				}
			else
				{
					loop_flag = FALSE;
				}
		}

	return count;
}


BOOL StripComments (struct DocumentParser *parser_p)
{
	STRPTR data_start_p = parser_p -> dp_line_p -> frld_Line;
	STRPTR cpp_comment_p = strstr (data_start_p, "//");
	BOOL loop_flag = TRUE;
	BOOL success_flag = TRUE;

	while (loop_flag && success_flag)
		{
			#if DOCUMENT_PARSER_DEBUG > 10
			DB (KPRINTF ("%s %ld - data_start \"%s\"\n", __FILE__, __LINE__, data_start_p));
			#endif


			if (parser_p -> dp_comment_flag)
				{
					STRPTR comment_p = strstr (data_start_p, "*/");

					#if DOCUMENT_PARSER_DEBUG > 10
					DB (KPRINTF ("%s %ld - comment_flag on \"%s\" sub \"%s\"\n", __FILE__, __LINE__, data_start_p, comment_p));
					#endif

					if (comment_p)
						{
							data_start_p = comment_p + 2;
							parser_p -> dp_comment_flag = FALSE;
							loop_flag = ((*data_start_p) != '\0');
						}
					else
						{
							loop_flag = FALSE;
						}
				}
			else
				{
					STRPTR comment_p = strstr (data_start_p, "/*");

					#if DOCUMENT_PARSER_DEBUG > 10
					DB (KPRINTF ("%s %ld - comment_flag off \"%s\" sub \"%s\"\n", __FILE__, __LINE__, data_start_p, comment_p));
					#endif

					if (comment_p)
						{
							if (data_start_p < comment_p)
								{
									success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (comment_p - data_start_p));

									#if DOCUMENT_PARSER_DEBUG > 10
									DB (KPRINTF ("%s %ld - appended c \"%d\" sub \"%s\"\n", __FILE__, __LINE__, success_flag, comment_p));
									#endif
								}

							parser_p -> dp_comment_flag = TRUE;
							data_start_p = comment_p + 2;
							loop_flag = ((*data_start_p) != '\0');
						}
					else if (cpp_comment_p && (data_start_p < cpp_comment_p))
						{
							if (data_start_p < cpp_comment_p)
								{
									success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (cpp_comment_p - data_start_p));

									#if DOCUMENT_PARSER_DEBUG > 10
									DB (KPRINTF ("%s %ld - appended c \"%d\" sub \"%s\"\n", __FILE__, __LINE__, success_flag, cpp_comment_p));
									#endif
								}


							loop_flag = FALSE;
						}
					else
						{
							char *line_end_p = strrchr (data_start_p, '\n');

							if (line_end_p)
								{
									if (line_end_p > data_start_p)
										{
											#if DOCUMENT_PARSER_DEBUG > 10
											DB (KPRINTF ("%s %ld - appended line end\"%s\"\n", __FILE__, __LINE__, line_end_p));
											#endif

											success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (line_end_p - data_start_p));
										}
									else
										{
											#if DOCUMENT_PARSER_DEBUG > 10
											DB (KPRINTF ("%s %ld - terminating with line end\"%s\"\n", __FILE__, __LINE__, line_end_p));
											#endif

											loop_flag = FALSE;
										}
								}
							else
								{
									success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, strlen (data_start_p));
								}
							loop_flag = FALSE;
						}
				}

		}		/* while (loop_flag && success_flag) */

	#if DOCUMENT_PARSER_DEBUG > 10
	DB (KPRINTF ("%s %ld - finished looping with success\"%d\"\n", __FILE__, __LINE__, success_flag));
	#endif

	if (success_flag)
		{
			success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, " ", 1);
		}

	#if DOCUMENT_PARSER_DEBUG > 10
	DebugPrintByteBuffer (parser_p -> dp_buffer_p);
	#endif

	return success_flag;
}



char *ParseDocument (struct DocumentParser *parser_p)
{
	char *prototype_s = NULL;

	if (StripComments (parser_p))
		{
			char *delim_p = strchr (parser_p -> dp_buffer_p -> bb_data_p, ';');

			if (delim_p)
				{
					/* cut the string from the buffer */
					prototype_s = ExtractSubstring (parser_p -> dp_buffer_p, delim_p);

					if (!prototype_s)
						{
							printf ("Failed to extract prototype from \"%s\"\n", parser_p -> dp_buffer_p -> bb_data_p);
						}
				}
			else
				{
					#if DOCUMENT_PARSER_DEBUG > 10
					DB (KPRINTF ("%s %ld - no delim in \"%s\"\n", __FILE__, __LINE__, parser_p -> dp_buffer_p -> bb_data_p));
					#endif
				}
		}

	return prototype_s;
}
