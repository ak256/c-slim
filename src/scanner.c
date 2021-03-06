/* scanner.c
 * Extracts tokens from code character sequences
 * author: Andrew Klinge
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>

#include "scanner.h"
#include "token.h"

// maximum expression length (in chars for a token)
#define CHARBUF_SIZE 4096
#define REGEX_FLAGS (REG_EXTENDED | REG_NOSUB)

void scanner_init(struct Scanner *scanner, struct TokenRegex *token_regexes, int token_regexes_count) {
	scanner->buf = malloc(sizeof(char) * CHARBUF_SIZE);
	scanner->token_regexes = token_regexes;
	scanner->token_regexes_count = token_regexes_count;
}

/* Checks if the condition is true, and if not, then prints the error message, along with the current line and token information.
 * Variable arguments at end are for error_string format args. 
 * Returns: whether the check failed (condition was false)
 */
static bool assert(bool condition, char *buf, int buflen, int ln, const char *error_string, ...) {
	if (condition) return false;

	// only print current line
	char *next_newline = index(buf, '\n');
	if (next_newline != NULL) {
		*next_newline = '\0';
	} else {
		buf[buflen] = '\0';
	}

	va_list va;
	va_start(va, error_string);
	vfprintf(stderr, error_string, va);
	va_end(va);

	fprintf(stderr, " at line %i: \n\t%s\n", ln, buf);
	return true;
}

/* Reads characters until getting the next token from the file.
 * Returns:
 *   SCAN_NULL ...  no token scanned yet
 *   SCAN_ERROR ... error, invalid/illegal token detected
 *   SCAN_VALID ... valid token scanned
 *
 * file - the file to scan from
 * ln - pointer to current line number, updated as lines are counted
 * output - where to store scanned token data
 */
int scanner_scan(struct Scanner *scanner, FILE *file, int *ln, struct Token *output) {
	char *buf = scanner->buf;
	int buf_index = 0;
	int start_ln = *ln;
	bool commented = false;
	bool leading_whitespace = true;

	while (true) {
		char c = fgetc(file);
		if (c == '\n') {
			*ln += 1;
			commented = false;
			if (leading_whitespace) {
				start_ln++;
			}
		}
		if (commented) continue;
		if (buf_index == 0 && c <= ' ') {
			if (c == EOF) {
				output->id = TOKEN_EOF;
				output->ln = start_ln;
				output->string = NULL;
				return SCAN_VALID;
			}
			leading_whitespace = true;
			continue;
		} else {
			leading_whitespace = false;
		}
		if (buf_index >= 1 && c == '/' && buf[buf_index - 1] == '/') {
			commented = true;
			buf_index--;
			continue;
		}

		buf[buf_index] = c;
		buf[buf_index + 1] = '\0';
		
		for (int i = 0; i < scanner->token_regexes_count; i++) {
			const TokenRegex *tr = &scanner->token_regexes[i];
			if (regexec(tr, buf, 0, NULL, 0) != 0) continue;

			int token_string_size = buf_index + 2;
			if (token_end_marked_by_next(tr->tokenID)) {
				buf[buf_index] = '\0';
				token_string_size--;
				fseek(file, -1, SEEK_CUR); 
			}

			output->id = tr->tokenID;
			output->ln = start_ln;
			char *token_string = malloc(sizeof(char) * token_string_size);
			strncpy(token_string, buf, token_string_size);
			output->string = token_string;
			return SCAN_VALID;
		}
		if (assert(c != EOF, buf, buf_index, start_ln, "Invalid expression"))
			return SCAN_ERROR;

		buf_index++;
		if (assert(buf_index < CHARBUF_SIZE - 2, buf, buf_index, start_ln,
			"Expression exceeds maximum length (%i)", CHARBUF_SIZE))
			return SCAN_ERROR;
	}
	return SCAN_NULL;
}
