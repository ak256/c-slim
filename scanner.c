/* scanner.c
 * Extracts tokens from code character sequences
 * author: Andrew Klinge
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#include "scanner.h"
#include "token.h"

// maximum expression length (in chars for a token)
#define CHARBUF_SIZE 4096
#define REGEX_FLAGS (REG_EXTENDED | REG_NOSUB)

// map regular expressions to tokens
typedef struct TokenRegex {
	regex_t regex;
	int tokenID;
} TokenRegex;

static TokenRegex regexes[32];
static int regex_count = 0;

/* Creates a regex that corresponds to a token and stores it in regexes[].
 *
 * regexstr - the regular expression to use
 * tokenID - the token that the regex matches to
 */
static void register_regex(const char *regexstr, int tokenID) {
	TokenRegex tr;
	tr.tokenID = tokenID;
	int error = regcomp(&tr.regex, regexstr, REGEX_FLAGS);
	if (error) {
		fprintf(stderr, "Regex creation failed! %s\n", regexstr);
		int errStrSize = 1024;
		char errStr[errStrSize];
		regerror(error, &tr.regex, errStr, errStrSize);
		fprintf(stderr, "%s\n", errStr);
		exit(1);
	}
	regexes[regex_count] = tr;
	regex_count++;
}

/* Initializes the given scanner. */
void scanner_init(Scanner *scanner) {
	scanner->buf = malloc(sizeof(char) * CHARBUF_SIZE);
	if (regex_count == 0) {
		register_regex("^;", TOKEN_END);
		register_regex("^[-.~!$%^&*+=|:?]", TOKEN_OPERATOR);
		register_regex("^/[^/]", TOKEN_OPERATOR_SLASH);
		register_regex("^,", TOKEN_LIST_SEPARATOR);
		register_regex("^\\(", TOKEN_GROUP_OPEN);
		register_regex("^\\)", TOKEN_GROUP_CLOSE);
		register_regex("^\\{", TOKEN_BLOCK_OPEN);
		register_regex("^\\}", TOKEN_BLOCK_CLOSE);
		register_regex("^[a-zA-Z_][a-zA-Z0-9_]*[^a-zA-Z0-9_]$", TOKEN_IDENTIFIER);
		register_regex("^([0-9]+)[.]([0-9]+)[^0-9]$", TOKEN_FLOAT_LITERAL);
		register_regex("^([0-9]+)[^0-9.]$", TOKEN_INT_LITERAL);
		register_regex("^\"([^\\\"]|\\\\.)*\"", TOKEN_STRING_LITERAL);
		register_regex("^\\[", TOKEN_LIST_OPEN);
		register_regex("^\\]", TOKEN_LIST_CLOSE);
		register_regex("^#([a-zA-Z]+) ", TOKEN_PREPROCESSOR_CMD);
	}
}

/* Scans and returns the next token from the file.
 * WARNING: ensure that the returned token is valid!
 * ln - the current line number.
 */
Token scanner_scan(Scanner *scanner, FILE *file, int *ln) {
	char *buf = scanner->buf;
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
			if (c == EOF) INVALID_TOKEN;
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

			int tokenID = regexes[i].tokenID;
			int string_size = bufind + 2; // for token's text
			if (tokenID == TOKEN_PREPROCESSOR_CMD) {
				// preprocessor command. always ends with a space (ignore)
				buf[bufind] = '\0';
				string_size--;
			} else if (token_char_terminated(tokenID)) {
				// always ends with an extra unrelated char. trim this char
				buf[bufind] = '\0';
				string_size--;
				// still want it to be scanned, though
				fseek(file, -1, SEEK_CUR); 
			}

			Token token;
			token.id = tokenID;
			token.ln = expLn;
			char *string = malloc(sizeof(char) * string_size);
			strncpy(string, buf, string_size);
			token.string = string;
			return token;
		}
		
		// detect invalid expressions
		if (c == EOF) {
			buf[bufind] = '\0';
			char *newLine = index(buf, '\n');
			if (newLine) *newLine = '\0'; // we only want to print first line
			fprintf(stderr, "Invalid expression at line %i: %s\n", expLn, buf);
			return INVALID_TOKEN;
		} 
		// move onto next character
		bufind++;
		if (bufind >= CHARBUF_SIZE - 2) {
			buf[bufind] = '\0';
			char *newLine = index(buf, '\n');
			if (newLine) *newLine = '\0'; // we only want to print first line
			fprintf(stderr, "Expression exceeds maximum length (%i) ", CHARBUF_SIZE);
			fprintf(stderr, "at line %i: %s\n", expLn, buf);
			return INVALID_TOKEN;
		}
	}
	return INVALID_TOKEN;
}
