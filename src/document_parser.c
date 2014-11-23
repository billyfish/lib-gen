

struct DocumentParser
{
	struct ByteBuffer *dp_buffer_p;
	BOOL dp_matched_flag;
	BOOL dp_comment_flag;

};


STRPTR StripComments (struct DocumentParser *parser_p, STRPTR line_p)
{
	/* are we already in a comment? */
	if (parser_p -> dp_comment_flag)
		{
			STRPTR comment_end_p = strstr (line_p, "*/");

			if (comment_end_p)
				{
					/* Is there a new starting comment? */
					STRPTR c_comment_p = strstr (comment_end_p, "/*");
					STRPTR cpp_comment_p = strstr (comment_end_p, "//");
					STRPTR comment_start_p = NULL;

					if (c_comment_p)
						{
							if (cpp_comment_p)
								{
									comment_start_p = (c_comment_p < cpp_comment_p) ? c_comment_p : cpp_comment_p;
								}
							else
								{
									comment_start_p = c_comment_p;
								}
						}
					else
						{
							comment_start_p = cpp_comment_p;
						}

					if (comment_start_p)
						{
						}

				}
			else
				{
					/* still in comment */
				}
		}

}

