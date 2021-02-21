/* parser.h
 * author: Andrew Klinge
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>

#include "token.h"
#include "symtable.h"
#include "statement.h"
#include "utils/hashtable.h"

// size of input tokens buffer
extern const int PARSER_TOKENBUF_SIZE;

enum parse_code {
	PARSE_NULL,
	PARSE_ERROR,
	PARSE_VALID
};

typedef struct Parser {
    HashTable included_files;
    Token *tokenbuf;
    int tokenbuf_count;
} Parser;

void parser_init(Parser *parser);

int parser_parse(Parser *parser, SymTable *tbl, Token *next_token, Statement *output);

#endif