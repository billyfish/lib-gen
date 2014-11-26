/*

************************************************************
**
** Created by: CodeBench 0.23 (17.09.2011)
**
** Project: libgen
**
** File: 
**
** Date: 25-11-2014 23:03:17
**
************************************************************

*/

#ifndef DOCUMENT_PARSER_H
#define DOCUMENT_PARSER_H

#include <exec/types.h>

#include "byte_buffer.h"

struct DocumentParser
{
	struct ByteBuffer *dp_buffer_p;
	BOOL dp_matched_flag;
	BOOL dp_comment_flag;

};


#endif
