/* parser.c
 * Parses statements from sequences of tokens.
 * author: Andrew Klinge
*/

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "token.h"
#include "symtable.h"

#define PARSER_TOKENBUF_SIZE 4096

void parser_init(struct Parser *parser) {
	hashtable_init(&parser->included_files, 32, 0);
	parser->tokenbuf = malloc(PARSER_TOKENBUF_SIZE * sizeof(struct Token));
	parser->tokenbuf_count = 0;

	// so we don't free random memory when overwriting in tokenbuf
	parser->tokenbuf[0].string = NULL;
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

/* Prints the parser's current line info (formatted to be appended after some message). */
static void print_line_info(struct Parser *parser) {
	// recreate string line of code from tokens buffer
	int string_length;
	for (int i = 0; i < parser->tokenbuf_count; i++) {
		string_length += strlen(parser->tokenbuf[i].string) + 1; // +1 for spacing
	}
	
	char string[string_length];
	int string_index = 0;
	for (int i = 0; i < parser->tokenbuf_count; i++) {
		char *token_string = parser->tokenbuf[i].string;
		int j = 0;
		for (; token_string[j] != '\0'; j++) {
			string[string_index + j] = token_string[j];
		}
		string[string_index + j] = ' ';
		string_index += j + 1;
	}
	string[string_index - 1] = '\0';
	fprintf(stderr, " at line %i: \n\t%s\n", parser->tokenbuf[0].ln, string);
}

/* Checks if the condition is true, and if not, then prints the error message, along with the current line and token information.
 * Variable arguments at end are for error_string format args. 
 * Returns: whether the check failed (condition was false)
 */
static bool assert(bool condition, struct Parser *parser, const char *error_string, ...) {
	if (condition) return false;

	va_list va;
	va_start(va, error_string);
	vfprintf(stderr, error_string, va);
	va_end(va);

	print_line_info(parser);
	return true;
}

/* Gets the next statement given repeated calls providing a sequence of tokens.
 * Automatically updates the symbol table as well.
 * Returns:
 *   PARSE_NULL ...  no statement parsed yet
 *   PARSE_ERROR ... error, invalid/illegal statement detected
 *   PARSE_VALID ... valid statement parsed
 *
 * token - the next token in the sequence
 * output - where to store the resulting statement data
 */
int parser_parse(struct Parser *parser, struct SymTable *symtable, struct Token *token, struct Statement *output) {
	if (assert(parser->tokenbuf_count < PARSER_TOKENBUF_SIZE, parser, 
		"Exceeded maximum number of tokens per statement (%i)", PARSER_TOKENBUF_SIZE))
		return PARSE_ERROR;
	
	if (token->id == TOKEN_BLOCK_OPEN) {
		if (assert(!symtable_push_scope(symtable), parser, 
			"Exceeded maximum number of nested scopes (%i)", SYMTABLE_MAX_SCOPES))
			return PARSE_ERROR;
		return PARSE_NULL;
	} else if(token->id == TOKEN_BLOCK_CLOSE) {
		if (assert(!symtable_pop_scope(symtable), parser, 
			"Unexpected scope block closing statement, no scope to close!"))
			return PARSE_ERROR;
		return PARSE_NULL;
	}

	// free string memory when overwriting previous token
	free(parser->tokenbuf[parser->tokenbuf_count].string); 
	parser->tokenbuf[parser->tokenbuf_count] = *token;
	parser->tokenbuf_count++;

	if (token->id == TOKEN_END_OF_STATEMENT) {
		if (parser->tokenbuf_count == 1) {
			parser->tokenbuf_count = 0;
			return PARSE_NULL;
		}
		
		switch (parser->tokenbuf[0].id) {
		case TOKEN_PREPROCESSOR_CMD: {
			const int token_count = parser->tokenbuf_count - 1;
			const struct Token *buf = parser->tokenbuf;
			const char *cmd = buf[0].string + 1;

			if (strcmp("include", cmd) == 0) {
				if (assert(token_count == 2 && buf[1].id == TOKEN_STRING_LITERAL, parser,
					"Invalid include statement (expected `#include \"path\";`)"))
					return PARSE_ERROR;

				char *path = buf[1].string;
				char *string = strtok_to_string(path);
				hashtable_add(&parser->included_files, hash_string(string), string);
			} else if (strcmp("define", cmd) == 0) {
				// #define identifier definition...
				// TODO
			}

			parser->tokenbuf_count = 0;
			return PARSE_NULL;
		}
		case TOKEN_IDENTIFIER: {
			const int token_count = parser->tokenbuf_count - 1;
			struct Token *buf = parser->tokenbuf;
			const char *identifier = buf[0].string;

			if (strcmp("break", identifier) == 0) {
				if (assert(token_count <= 2, parser, 
					"Invalid break statement (expected `break;` or `break label;`)"))
					return PARSE_ERROR;

				if (token_count == 2) {
					if (assert(buf[1].id == TOKEN_IDENTIFIER, parser,
						"Invalid break statement (expected label identifier, ex: `break label;`)") ||
						assert(symtable_get(symtable, buf[1].string) != NULL, parser,
						"Undefined label identifier: %s", buf[1].string))
						return PARSE_ERROR;

					output->id = STATEMENT_BREAK_LABEL;
					output->args = &buf[1].string;
					output->arg_count = 1;
					return PARSE_VALID;
				} else {
					output->id = STATEMENT_BREAK;
					output->args = NULL;
					output->arg_count = 0;
					return PARSE_VALID;
				}
			} else {
				if (token_count >= 2) {
					if (buf[0].id == TOKEN_IDENTIFIER && buf[1].id == TOKEN_IDENTIFIER) {
						if (token_count == 2) {
							// uninitialized variable declaration
							// TODO
						}
					}
				}
			}
			// fall through to default case, error
		}
		default:
			fprintf(stderr, "Invalid statement");
			print_line_info(parser);
			return PARSE_ERROR;
		}
	}
	return PARSE_NULL;
}
