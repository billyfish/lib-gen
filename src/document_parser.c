#include <string.h>

#include "document_parser.h"



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
					STRPTR comment_p = strstr (line_p, "*/");
					
					if (comment_p)
						{
							data_start_p = comment_p + 2;
							parser_p -> dp_comment_flag = FALSE;
							loop_flag = ((*data_start_p) != '\0');
						}
				}
			else
				{
					STRPTR comment_p = strstr (line_p, "/*");
					
					if (comment_p)
						{
							parser_p -> dp_comment_flag = TRUE;

							success_flag = AppendToByteBuffer (parser_p -> dp_buffer_p, data_start_p, (size_t) (comment_p - data_start_p));
							loop_flag = ((* (comment_p + 2)) != '\0');
						}				
				}		
		}		/* while (loop_flag && success_flag) */ 
	

	return success_flag;
}



BOOL DoStuff (struct DocumentParser *parser_p, STRPTR line_p)
{
	BOOL success_flag = FALSE;
	
	if (StripComments (parser_p, line_p))
		{
			if (MakeByteBufferDataValidString (parser_p -> dp_buffer_p))
				{
					char *delim_p = strchr (parser_p -> dp_buffer_p, ';');				
				
					if (delim_p)
						{
							/* cut the string from the buffer */
							
						}
				}

		
		}

	return success_flag;
}
