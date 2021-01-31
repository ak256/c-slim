// parser.c - handles parsing tokens from text

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <sys/types.h>

#include "parser.h"
#include "token.h"

// expression length (for a token)
#define MAX_EXPR_LENGTH 4096

typedef struct TokenRegex {
    regex_t regex;
    int tokenID;
} TokenRegex;

static TokenRegex *regexes;
static int regex_count = 0;

static void ___(const char *regexstr, int tokenID) {
    TokenRegex tr;
    tr.tokenID = tokenID;
    if (regcomp(&tr.regex, regexstr, REG_NOSUB)) {
        fprintf(stderr, "Regex creation failed! %s\n", regexstr);
        exit(1);
    }
    regexes[regex_count] = tr;
    regex_count++;
}

void parser_init() {
    regexes = malloc(sizeof(TokenRegex) * 32);
    ___("^;", T_END);
    ___("^\\$.*;", T_COMMENT);
    ___("^\"([^\\\"]|\\.)*\"", T_STRING_LITERAL);
    ___("^#ignore", T_MEGACOMMENT_OPEN);
    ___("^#!ignore", T_MEGACOMMENT_CLOSE);
    ___("^\\s*[a-zA-z_][a-zA-Z0-9]*[({\\[ ]$", T_IDENTIFIER);
    ___("^,", T_LIST_SEPARATOR);
    ___("^(", T_GROUP_OPEN);
    ___("^)", T_GROUP_CLOSE);
    ___("^{", T_BLOCK_OPEN);
    ___("^}", T_BLOCK_CLOSE);
    ___("^\\[", T_LIST_OPEN);
    ___("^\\]", T_LIST_CLOSE);
}

/* Parses and returns the next token from the file.
 *
 * ln - the current line number.
 */
Token *parse_token(FILE *file, int *ln) {
    static char buf[MAX_EXPR_LENGTH];
    int bufind = 0;

    // read chars until a token is found by regex
    while ((buf[bufind] = fgetc(file)) != EOF) {
        // all whitespace is equal
        if (buf[bufind] < ' ') {
            buf[bufind] = ' '; 
        }
        // ignore leading whitespace
        if (bufind == 0 && buf[bufind] == ' ') continue;
        buf[bufind + 1] = '\0';

        // find a matching regex
        for (int i = 0; i < regex_count; i++) {
            if (!regexec(&regexes[i].regex, buf, 0, NULL, 0)) {
                Token *token = malloc(sizeof(Token));
                token->id = regexes[i].tokenID;
                int string_size = bufind + 2;
                if (token->id == T_IDENTIFIER) {
                    // always ends with ' ', '(', '[', or '{'. trim this
                    buf[bufind] = '\0';
                    string_size--;
                    // still want it to be scanned, though
                    fseek(file, -1, SEEK_CUR); 
                }

                char *string = malloc(sizeof(char) * string_size);
                strncpy(string, buf, string_size);
                token->string = string;
                return token;
            }
        }
        
        // move onto next character
        if (buf[bufind] == '\n') *ln += 1; // count lines
        bufind++;
        if (bufind >= MAX_EXPR_LENGTH - 2) {
            fprintf(stderr, "Exceeded maximum expression length (%i)!\n",
                MAX_EXPR_LENGTH);
            exit(1);
        }
    }
    return NULL;
}
