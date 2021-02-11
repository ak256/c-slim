/* parser.c
 * author: Andrew Klinge
*/

#include <stdlib.h>
#include <stdarg.h>

#include "parser.h"
#include "token.h"
#include "symtable.h"

const int PARSER_TOKENBUF_SIZE = 4096;

/* Initializes the given parser. */ 
void parser_init(Parser *parser) {
    hashtable_init(&parser->included_files);
    parser->tokenbuf = malloc(sizeof(Token) * PARSER_TOKENBUF_SIZE);
    parser->tokenbuf_count = 0;
}

/* Extracts the string contents from the string token.
 * example: "abc123" -> abc123
 */
static char *strtok_to_string(char *strtok) {
    int new_length = strlen(strtok) - 2 + 1; // -2 for "", +1 for \0
    char *string = malloc(sizeof(char) * new_length);
    for (int i = 0; i < new_length - 1; i++) {
        string[i] = strtok[i + 1];
    }
    strtok[new_length - 1] = '\0';
    return string;
}

/* Checks if the condition is true, and if not, then prints the error message,
 * the current line and token information, and exits.
 */
void assert(int condition, Token *current_token, const char *error_string, ...) {
    if (!condition) {
        // print custom error message format string
        va_list ap;
        va_start(ap);
        vfprintf(stderr, error_string, ap);
        va_end(ap);
        // print line occurrence and token
        fprintf(stderr, " at line %i: %s\n", current_token->ln, current_token->string);
        exit(1);
    }
}

/* Copies the next token to the parser's buffer and will attempt to parse
 * and return a statement as soon as it recognizes one. Also creates.
 * symbols in the SymTable.
 * Returns null until then.
 */
Statement *parser_parse_statement(Parser *parser, SymTable *tbl, Token *token) {
    assert(parser->tokenbuf_count < PARSER_TOKENBUF_SIZE,
        "Exceeded maximum number of tokens per statement (%i)", PARSER_TOKENBUF_SIZE);
    
    // manage scopes
    if (token->id == TOK_BLOCK_OPEN) {
        assert(!symtable_push_scope(tbl),
            token, "Exceeded maximum number of nested scopes (%i)", SYMTABLE_MAX_SCOPES);
        return NULL;
    } else if(token->id == TOK_BLOCK_CLOSE) {
        assert(!symtable_pop_scope(tbl),
            token, "Unexpected scope block closing statement, no scope to close!");
        return NULL;
    }

    // add token to buffer
    parser->tokenbuf[parser->tokenbuf_count] = *token;
    parser->tokenbuf_count++;

    // form statement if at end of one
    if (token->id == TOK_END) {
        int count = parser->tokenbuf_count - 1; // ignore END
        if (count == 0) return NULL; // empty statement

        Token *buf = parser->tokenbuf;
        Statement *stmnt = malloc(sizeof(Statement));
        if (buf[0].id > TOKSEC_PP) {
            switch (buf[0].id) {
                // #include "path"
                case TOK_PP_INCLUDE: {
                    assert(count == 2 && buf[1].id == TOK_STRING_LITERAL,
                        &buf[0], "Expected string file path (%s \"path\")", buf[0].string);

                    char *string = strtok_to_string(buf[1].string);
                    hashtable_add(&parser->included_files, hash(string), string);
                    break;
                }
                // #define "name" "definition"
                case TOK_PP_DEFINE:
                    // TODO
                    break;
            }
        }
        return stmnt;
    }
    return NULL;
}
