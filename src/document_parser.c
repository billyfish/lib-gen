#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "document_parser.h"
#include "utils.h"


struct DocumentParser *AllocateDocumentParser (void)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
	{
		struct DocumentParser *parser_p = (struct DocumentParser *) IExec->AllocVecTags (sizeof (struct DocumentParser), TAG_DONE);

		if (parser_p)
		{
			parser_p -> dp_buffer_p = buffer_p;
			parser_p -> dp_comment_flag = FALSE;
			parser_p -> dp_matched_flag = FALSE;

			return parser_p;
		}

		FreeByteBuffer (buffer_p);
	}

	return NULL;
}


void FreeDocumentParser (struct DocumentParser *parser_p)
{
	FreeByteBuffer (parser_p -> dp_buffer_p);
	IExec->FreeVec (parser_p);
}


BOOL StripComments (struct DocumentParser *parser_p, STRPTR line_p)
{
	STRPTR cpp_comment_p = strstr (line_p, "//");
	STRPTR data_start_p = line_p;
	BOOL loop_flag = TRUE;
	BOOL success_flag = TRUE;

	while (loop_flag && success_flag)
		{
			if (parser_p -> dp_comment_flag)
				{
					STRPTR comment_p = strstr (data_start_p, "*/");

					if (comment_p)
						{
							data_start_p = comment_p + 2;
							parser_p -> dp_comment_flag = FALSE;
							loop_flag = ((*data_start_p) != '\0');
						}
				}
			else
				{
					STRPTR comment_p = strstr (data_start_p, "/*");

					if (comment_p)
						{
							success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (comment_p - data_start_p));

							parser_p -> dp_comment_flag = TRUE;
							data_start_p = comment_p + 2;
							loop_flag = ((*data_start_p) != '\0');
						}
					else if (cpp_comment_p)
						{
							success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (cpp_comment_p - data_start_p));
							loop_flag = FALSE;
						}
					else
						{
							char *line_end_p = strrchr (data_start_p, '\n');

							if (line_end_p)
								{
									if (line_end_p > data_start_p)
										{
											success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (line_end_p - data_start_p));
										}
									else
										{
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

	if (success_flag)
		{
			success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, " ", 1);
		}

	return success_flag;
}



BOOL ParseDocument (struct DocumentParser *parser_p, STRPTR line_p)
{
	BOOL success_flag = FALSE;

	if (StripComments (parser_p, line_p))
		{
			if (MakeByteBufferDataValidString (parser_p -> dp_buffer_p))
				{
					char *delim_p = strchr (parser_p -> dp_buffer_p -> bb_data_p, ';');

					if (delim_p)
						{
							/* cut the string from the buffer */
							char *prototype_s = ExtractSubstring (parser_p -> dp_buffer_p, delim_p);

							if (prototype_s)
								{
									printf (">>> \"%s\"\n", prototype_s);

									IExec->FreeVec (prototype_s);
								}
						}
				}
		}

	return success_flag;
}
