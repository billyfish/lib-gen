

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

		}

}

