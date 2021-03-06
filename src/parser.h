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

enum parse_code {
	PARSE_NULL,
	PARSE_ERROR,
	PARSE_VALID
};

struct Parser {
    struct HashTable included_files;
    struct Token *tokenbuf;
    int tokenbuf_count;
};

void parser_init(struct Parser *parser);

int parser_parse(struct Parser *parser, struct SymTable *tbl, struct Token *next_token, struct Statement *output);

#endif
