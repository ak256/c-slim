/* parser.h
 * author: Andrew Klinge
*/

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>

#include "token.h"
#include "symtable.h"
#include "statement.h"

// size of input tokens buffer
extern const int PARSER_TOKENBUF_SIZE;

typedef struct Parser {
    Token *tokenbuf;
    int tokenbuf_count;
} Parser;

void parser_init(Parser *parser);

Statement *parser_parse_statement(Parser *parser, SymTable *tbl,
    Token *next_token);

#endif
