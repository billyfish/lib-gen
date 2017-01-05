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

#include "byte_buffer.h"
#include <stddef.h>
#include <inttypes.h>

enum ParserState
{
	PS_NORMAL,
	PS_STRUCT,
	PS_ENUM,
	PS_C_COMMENT,
	PS_CPP_COMMENT
};


struct DocumentParser
{
	struct ByteBuffer *dp_buffer_p;
	bool dp_matched_flag;
	bool dp_typedef_flag;
	enum ParserState dp_state;
	FILE *dp_file_handle_p;
	char *dp_line_p;
	int32_t dp_line_number;
	int32_t dp_num_open_brackets;
	char *dp_current_data_p;
};


struct DocumentParser *AllocateDocumentParser (void);

void FreeDocumentParser (struct DocumentParser *parser_p);

bool StripComments (struct DocumentParser *parser_p);

char *ParseDocument (struct DocumentParser *parser_p);



bool GetNextPrototype (struct DocumentParser *parser_p, char **prototype_ss);

bool ParseLine (struct DocumentParser *parser_p);

bool SetDocumentToParse (struct DocumentParser *parser_p, FILE *handle_p);

#endif
