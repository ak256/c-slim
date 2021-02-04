/* parser.c
 * author: Andrew Klinge
*/

#include <stdlib.h>

#include "parser.h"
#include "token.h"
#include "symtable.h"

const int PARSER_TOKENBUF_SIZE = 4096;

/* Initializes the given parser. */ 
void parser_init(Parser *parser) {
    parser->tokenbuf = malloc(sizeof(Token) * PARSER_TOKENBUF_SIZE);
    parser->tokenbuf_count = 0;
}

/* Copies the next token to the parser's buffer and will attempt to parse
 * and return a statement as soon as it recognizes one. Also creates.
 * symbols in the SymTable.
 * Returns null until then.
 */
Statement *parser_parse_statement(Parser *parser, SymTable *tbl, Token *token) {
    if (parser->tokenbuf_count >= PARSER_TOKENBUF_SIZE) {
        fprintf(stderr, 
            "Exceeded maximum number of tokens per statement (%i) ",
            PARSER_TOKENBUF_SIZE);
        fprintf(stderr, "at line %i: %s\n", token->ln, token->string);
        exit(1);
    }
    
    // manage scopes
    if (token->id == TOK_BLOCK_OPEN) {
        if (symtable_push_scope(tbl)) {
            fprintf(stderr, "Exceeded maximum number of nested scopes (%i) ",
                SYMTABLE_MAX_SCOPES);
            fprintf(stderr, "at line %i: %s\n", token->ln, token->string);
            exit(1);
        }
        return NULL;
    } else if(token->id == TOK_BLOCK_CLOSE) {
        if (symtable_pop_scope(tbl)) {
            fprintf(stderr, "Exceeded maximum number of nested scopes (%i) ",
                SYMTABLE_MAX_SCOPES);
            fprintf(stderr, "at line %i: %s\n", token->ln, token->string);
            exit(1);
        }
        return NULL;
    }

    // add token to buffer
    parser->tokenbuf[parser->tokenbuf_count] = *token;
    parser->tokenbuf_count++;

    // form statement if at end of one
    if (token->id == TOK_END) {
        // aliases
        int count = parser->tokenbuf_count;
        Token *buf = parser->tokenbuf;
        
        if (count == 1) return NULL; // empty statement

        Statement *stmnt = malloc(sizeof(Statement));
        if (buf[0].id > TOKSEC_PP) {
            switch (buf[0].id) {
                case TOK_PP_INCLUDE:
                    // TODO
                    break;
                case TOK_PP_DEFINE:
                    // TODO
                    break;
            }
        }
        return stmnt;
    }
    return NULL;
}
