// scanner.c - extracts tokens from code text

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>
#include <sys/types.h>

#include "scanner.h"
#include "token.h"

// maximum expression length (in chars for a token)
#define MAX_EXPR_LENGTH 4096
#define REGEX_FLAGS (REG_EXTENDED | REG_NOSUB)

// map regular expressions to tokens
typedef struct TokenRegex {
    regex_t regex;
    int tokenID;
} TokenRegex;

static TokenRegex *regexes;
static int regex_count = 0;

/* Creates a regex that corresponds to a token.
 *
 * regexstr - the regular expression to use
 * tokenID - the token that the regex matches to
 */
static void ___(const char *regexstr, int tokenID) {
    TokenRegex tr;
    tr.tokenID = tokenID;
    int err;
    if ((err = regcomp(&tr.regex, regexstr, REGEX_FLAGS))) {
        fprintf(stderr, "Regex creation failed! %s\n", regexstr);
        int errStrSize = 1024;
        char errStr[errStrSize];
        regerror(err, &tr.regex, errStr, errStrSize);
        fprintf(stderr, "%s\n", errStr);
        exit(1);
    }
    regexes[regex_count] = tr;
    regex_count++;
}

/* Must be called before using scanner. */
void scanner_init() {
    regexes = malloc(sizeof(TokenRegex) * 32);
    ___("^;", TOK_END);
    ___("^[-.~!$%^&*+=|:?]", TOK_OPERATOR);
    ___("^/[^/]", TOK_OPERATOR_SLASH);
    ___("^,", TOK_LISTOK_SEPARATOR);
    ___("^\\(", TOK_GROUP_OPEN);
    ___("^\\)", TOK_GROUP_CLOSE);
    ___("^\\{", TOK_BLOCK_OPEN);
    ___("^\\}", TOK_BLOCK_CLOSE);
    ___("^[a-zA-Z_][a-zA-Z0-9_]*[^a-zA-Z0-9_]$", TOK_IDENTIFIER);
    ___("^([0-9]+)[.]([0-9]+)[^0-9]$", TOK_FLOATOK_LITERAL);
    ___("^[0-9][^0-9]$", TOK_INTOK_LITERAL);
    ___("^\"([^\\\"]|\\\\.)*\"", TOK_STRING_LITERAL);
    ___("^\\[", TOK_LISTOK_OPEN);
    ___("^\\]", TOK_LISTOK_CLOSE);
    ___("^#include ", TOK_PP_INCLUDE);
    ___("^#define ", TOK_PP_DEFINE);
}

/* Scans and returns the next token from the file.
 *
 * ln - the current line number.
 */
Token *scanner_scan_token(FILE *file, int *ln) {
    static char buf[MAX_EXPR_LENGTH]; // don't need a new buffer between calls
    int bufind = 0;
    int expLn = *ln; // expression starts on this line
    bool commented = 0; // whether scanning a comment. ends on new line
    bool incExpLn = 1; // until non-whitespace begins

    // read chars until a token is found by regex
    while (1) {
        // get character
        char c = fgetc(file);

        // count lines
        if (c == '\n') {
            *ln += 1;
            if (incExpLn) expLn++;
        }
        // handle comments
        if (commented) {
            if (c == '\n') commented = 0;
            else continue; // still in comment
        }
        // ignore leading whitespace
        if (bufind == 0 && c <= ' ') {
            if (c == EOF) return NULL;
            incExpLn = 1;
            continue;
        } else if (c > ' ') {
            incExpLn = 0;
        }
        // ignore comments
        if (bufind >= 1 && c == '/' && buf[bufind - 1] == '/') {
            commented = 1;
            // remove comment from buffer
            bufind--;
            continue;
        }
        buf[bufind] = c;
        buf[bufind + 1] = '\0';

        // find a matching regex
        for (int i = 0; i < regex_count; i++) {
            if (regexec(&regexes[i].regex, buf, 0, NULL, 0)) continue;

            Token *token = malloc(sizeof(Token));
            token->id = regexes[i].tokenID;
            token->ln = expLn;
            int string_size = bufind + 2;
            if (token->id <= TOK_IDENTIFIER) {
                // always ends with an extra unrelated char. trim this char
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
        
        // detect invalid expressions
        if (c == EOF) {
            buf[bufind] = '\0';
            char *newLine = index(buf, '\n');
            if (newLine) *newLine = '\0'; // we only want to print first line
            fprintf(stderr, "Invalid expression at line %i: %s\n", expLn, buf);
            return NULL;
        } 
        // move onto next character
        bufind++;
        if (bufind >= MAX_EXPR_LENGTH - 2) {
            buf[bufind] = '\0';
            char *newLine = index(buf, '\n');
            if (newLine) *newLine = '\0'; // we only want to print first line
            fprintf(stderr, "Expression exceeds maximum length (%i) ",
                MAX_EXPR_LENGTH);
            fprintf(stderr, "at line %i: %s\n", expLn, buf);
            return NULL;
        }
    }
    return NULL;
}
