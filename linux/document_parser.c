#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "document_parser.h"
#include "string_utils.h"


static char *ScrollPastObject (struct DocumentParser *parser_p);

void ProcessParser (struct DocumentParser *parser_p);


int main (int argc, char *argv [])
{
	if (argc > 1)
		{
			FILE *in_f = fopen (argv [1], "r");

			if (in_f)
				{
					struct DocumentParser *parser_p = AllocateDocumentParser ();

					if (parser_p)
						{
							char *prototype_s = NULL;

							if (SetDocumentToParse (parser_p, in_f))
								{
									if (StripComments (parser_p))
										{
											FreeCopiedString (parser_p -> dp_line_p);
											parser_p -> dp_line_p = NULL;

											ProcessParser (parser_p);

											printf ("doc after comments stripped:\n%s\n", parser_p -> dp_buffer_p -> bb_data_p);

											ParseDocument (parser_p);
										}

									FreeDocumentParser (parser_p);
								}
						}

					fclose (in_f);
				}
		}


	return 0;
}


void ProcessParser (struct DocumentParser *parser_p)
{
	char *prototype_s = NULL;

	parser_p -> dp_current_data_p = parser_p -> dp_buffer_p -> bb_data_p;

	while (GetNextPrototype (parser_p, &prototype_s))
		{
			printf ("found: \"%s\"\n", prototype_s);
			FreeCopiedString (prototype_s);
		}

}


struct DocumentParser *AllocateDocumentParser (void)
{
	ByteBuffer *buffer_p = AllocateByteBuffer (1024);

	if (buffer_p)
		{
			struct DocumentParser *parser_p = (struct DocumentParser *) malloc (sizeof (struct DocumentParser));

			if (parser_p)
				{
					parser_p -> dp_buffer_p = buffer_p;
					parser_p -> dp_state = PS_NORMAL;
					parser_p -> dp_matched_flag = false;
					parser_p -> dp_file_handle_p = NULL;
					parser_p -> dp_line_number = 0;
					parser_p -> dp_line_p = NULL;
					parser_p -> dp_num_open_brackets = 0;
					parser_p -> dp_typedef_flag = false;
					parser_p -> dp_current_data_p = NULL;


					return parser_p;
				}

			FreeByteBuffer (buffer_p);
		}


	return NULL;
}


void FreeDocumentParser (struct DocumentParser *parser_p)
{
	if (parser_p -> dp_line_p)
		{
			FreeCopiedString (parser_p -> dp_line_p);
		}

	FreeByteBuffer (parser_p -> dp_buffer_p);
	free (parser_p);
}


bool SetDocumentToParse (struct DocumentParser *parser_p, FILE *handle_p)
{
	parser_p -> dp_line_p = GetFileContentsAsString (handle_p);

	return ((parser_p -> dp_line_p) != NULL);
}


bool ParseLine (struct DocumentParser *parser_p)
{
	bool success_flag = GetLineFromFile (parser_p -> dp_file_handle_p, & (parser_p -> dp_line_p));

	if (success_flag)
		{
			++ (parser_p -> dp_line_number);
		}

	return success_flag;
}



bool GetNextPrototype (struct DocumentParser *parser_p, char **prototype_ss)
{
	bool loop_flag = true;
	bool more_data_flag = true;

	while (loop_flag)
		{
			char *prototype_s = ParseDocument (parser_p);

			if (prototype_s)
				{
					*prototype_ss = prototype_s;
					loop_flag = false;
				}

		}

	if (parser_p -> dp_current_data_p != '\0')
		{
			parser_p -> dp_current_data_p = ScrollPastWhitespace (parser_p -> dp_current_data_p);
		}

	if (parser_p -> dp_current_data_p == '\0')
		{
			more_data_flag = false;
		}

	return more_data_flag;
}


bool StripComments (struct DocumentParser *parser_p)
{
	char * data_start_p = parser_p -> dp_line_p;
	bool loop_flag = true;
	bool success_flag = true;


	while (loop_flag && success_flag)
		{

			switch (parser_p -> dp_state)
				{
					case PS_C_COMMENT:
						{
							char * comment_p = strstr (data_start_p, "*/");


							if (comment_p)
								{
									/* move past the end of comment */
									data_start_p = comment_p + 2;
									parser_p -> dp_state = PS_NORMAL;
									loop_flag = ((*data_start_p) != '\0');
								}
							else
								{
									loop_flag = false;
								}
						}
						break;

					case PS_NORMAL:
						{
							char * comment_p = strstr (data_start_p, "/*");
							char * cpp_comment_p = strstr (data_start_p, "//");


							if (comment_p)
								{
									if (data_start_p < comment_p)
										{
											success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (comment_p - data_start_p));

										}

									parser_p -> dp_state = PS_C_COMMENT;
									data_start_p = comment_p + 2;
									loop_flag = ((*data_start_p) != '\0');
								}
							else if (cpp_comment_p && (data_start_p < cpp_comment_p))
								{
									if (data_start_p < cpp_comment_p)
										{
											success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (cpp_comment_p - data_start_p));

										}


									loop_flag = false;
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

													loop_flag = false;
												}
										}
									else
										{
											success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, strlen (data_start_p));
										}
									loop_flag = false;
								}
						}
						break;


					default:
						break;
				}


		}		/* while (loop_flag && success_flag) */


	if (success_flag)
		{
			success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, " ", 1);
		}




	return success_flag;
}


static bool CheckNextToken (struct DocumentParser *parser_p, const char *value_s)
{
	bool match_flag = false;
	const size_t value_length = strlen (value_s);

	if (strncmp (parser_p -> dp_current_data_p, value_s, value_length) == 0)
		{
			parser_p -> dp_current_data_p += value_length;
			match_flag = true;
		}

	return match_flag;
}


char *ParseDocument (struct DocumentParser *parser_p)
{
	char *prototype_s = NULL;

	char *data_s = ScrollPastWhitespace (parser_p -> dp_current_data_p);

	if (data_s)
		{
			if (CheckNextToken (parser_p, "#include"))
				{
					data_s = ScrollPastWhitespace (parser_p -> dp_current_data_p);

				}

			if (CheckNextToken (parser_p, "typedef"))
				{
					parser_p -> dp_typedef_flag = true;
				}

			if (CheckNextToken (parser_p, "enum"))
				{
					ScrollPastObject (parser_p);
				}

			if (CheckNextToken (parser_p, "struct"))
				{
					ScrollPastObject (parser_p);
				}

			if (CheckNextToken (parser_p, "static"))
				{
					parser_p -> dp_typedef_flag = true;
				}


		}


	return prototype_s;
}



static char *ScrollPastObject (struct DocumentParser *parser_p)
{
	char *result_s = NULL;
	int32_t num_brackets = parser_p -> dp_num_open_brackets;
	char *cursor_p = parser_p -> dp_buffer_p -> bb_data_p;
	bool loop_flag;

	while (loop_flag)
		{
			switch (*cursor_p)
				{
					case '\0':
						loop_flag = false;
						break;

					case '{':
						++ num_brackets;
						break;

					case '}':
						-- num_brackets;

						if (num_brackets == 0)
							{
								cursor_p = ScrollPastWhitespace (cursor_p);

								if (parser_p -> dp_typedef_flag)
									{
										loop_flag = true;

										while (loop_flag)
											{
												if (*cursor_p == '\0')
													{
														loop_flag = false;

													}
												else
													{
														if (isalnum (*cursor_p))
															{
																++ cursor_p;
															}
														else
															{
																cursor_p = ScrollPastWhitespace (cursor_p);
																loop_flag = false;
															}
													}
											}

									}

								if (*cursor_p == ';')
									{
										result_s = cursor_p + 1;
									}

								loop_flag = false;
							}
						else if (num_brackets < 0)
							{
								printf ("error ScrollPastObject for \"%s\"\n", parser_p -> dp_buffer_p -> bb_data_p);
							}
						break;
				}

		}



	return result_s;
}


