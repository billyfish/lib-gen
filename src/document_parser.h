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
	struct FReadLineData *dp_line_p;
	BOOL dp_matched_flag;
	BOOL dp_comment_flag;
	BPTR dp_file_handle_p;
	int32 dp_line_number;
};


struct DocumentParser *AllocateDocumentParser (void);

void FreeDocumentParser (struct DocumentParser *parser_p);

BOOL StripComments (struct DocumentParser *parser_p);

char *ParseDocument (struct DocumentParser *parser_p);

int32 GetNextPrototype (struct DocumentParser *parser_p, STRPTR *prototype_ss);

int32 ParseLine (struct DocumentParser *parser_p);

void SetDocumentToParse (struct DocumentParser *parser_p, BPTR handle_p);

#endif
