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

const int PARSER_TOKENBUF_SIZE = 4096;

static char *tokens_to_line(Token *buf, int buflen);

/* Initializes the given parser. */ 
void parser_init(Parser *parser) {
	hashtable_init(&parser->included_files, 32, 0);
	parser->tokenbuf = malloc(PARSER_TOKENBUF_SIZE * sizeof(Token));
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
static void print_line_info(Parser *parser) {
	char *line_string = tokens_to_line(parser->tokenbuf, parser->tokenbuf_count);
	fprintf(stderr, " at line %i: \n\t%s\n", parser->tokenbuf[0].ln, line_string);
	free(line_string);
}

/* Checks if the condition is true, and if not, then prints the error message,
 * along with the current line and token information.
 * Variable arguments at end are for error_string format args. 
 * Returns: whether the check failed (condition was false)
 */
static bool check(bool condition, Parser *parser, const char *error_string, ...) {
	if (condition) return false;

	va_list va;
	va_start(va, error_string);
	vfprintf(stderr, error_string, va);
	va_end(va);

	print_line_info(parser);
	return true;
}

/* Rebuilds code line from list of tokens.
 * Note: inserts spaces between individual tokens.
 */
static char *tokens_to_line(Token *buf, int buflen) {
	int length;
	for (int i = 0; i < buflen; i++) {
		length += strlen(buf[i].string) + 1; // +1 for spacing
	}
	
	char *string = malloc(sizeof(char) * length);
	int string_index = 0;
	for (int i = 0; i < buflen; i++) {
		char *token_string = buf[i].string;
		int j = 0;
		for (; token_string[j] != '\0'; j++) {
			string[string_index + j] = token_string[j];
		}
		string[string_index + j] = ' ';
		string_index += j + 1;
	}
	string[string_index - 1] = '\0';
	return string;
}

/* Parses a preprocessor command and executes it.
 * See parent parser_parse(). 
 */
static inline int parser_parse_preprocessor_cmd(Parser *parser, SymTable *symtable) {
	const Token *buf = parser->tokenbuf;
	const char *cmd = buf[0].string + 1;

	if (strcmp("include", cmd) == 0) {
		if (check(parser->tokenbuf_count == 2 && buf[1].id == TOKEN_STRING_LITERAL, parser,
			"Invalid include statement! Expected: `#include \"path\";`"))
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

/* Parses a statement that begins with a word/identifier.
 * See parent parser_parse(). 
 */
static inline int parser_parse_word_prefixed(Parser *parser, SymTable *symtable, Statement *output) {
	const Token *buf = parser->tokenbuf;
	const char *identifier = buf[0].string;

	if (strcmp("break", identifier) == 0) {
		if (check(parser->tokenbuf_count <= 2, parser, 
			"Invalid break statement! Expected: `break;` OR: `break label;`"))
			return PARSE_ERROR;

		if (parser->tokenbuf_count == 2) {
			if (check(buf[1].id == TOKEN_IDENTIFIER, parser,
				"Invalid break statement! Expected label identifier, ex: `break label;`") ||
				check(symtable_get(symtable, buf[1].string) != NULL, parser,
				"Undefined label identifier: %s", buf[1].string))
				return PARSE_ERROR;

			output->id = STATEMENT_BREAK_LABEL;
			output->args = buf[1].string;
			output->arg_count = 1;
			return PARSE_VALID;
		} else {
			output->id = STATEMENT_BREAK;
			output->args = NULL;
			output->arg_count = 0;
			return PARSE_VALID;
		}
	}

	return PARSE_ERROR;
}

/* Gets the next statement given repeated calls providing a sequence of tokens.
 * Automatically updates the symbol table as well.
 * Returns:
 *   PARSE_NULL ...  no token scanned yet
 *   PARSE_ERROR ... error, invalid/illegal token detected
 *   PARSE_VALID ... valid token scanned
 *
 * token - the next token in the sequence
 * output - where to store the resulting statement data
 */
int parser_parse(Parser *parser, SymTable *symtable, Token *token, Statement *output) {
	if (check(parser->tokenbuf_count < PARSER_TOKENBUF_SIZE, parser, 
		"Exceeded maximum number of tokens per statement (%i)", PARSER_TOKENBUF_SIZE))
		return PARSE_ERROR;
	
	if (token->id == TOKEN_BLOCK_OPEN) {
		if (check(!symtable_push_scope(symtable), parser, 
			"Exceeded maximum number of nested scopes (%i)", SYMTABLE_MAX_SCOPES))
			return PARSE_ERROR;
		else
			return PARSE_NULL;
	} else if(token->id == TOKEN_BLOCK_CLOSE) {
		if (check(!symtable_pop_scope(symtable), parser, 
			"Unexpected scope block closing statement, no scope to close!"))
			return PARSE_ERROR;
		else
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
		case TOKEN_PREPROCESSOR_CMD:
			return parser_parse_preprocessor_cmd(parser, symtable);
		case TOKEN_IDENTIFIER:
			return parser_parse_word_prefixed(parser, symtable, output);
		default:
			fprintf(stderr, "Unknown/invalid statement");
			print_line_info(parser);
			return PARSE_ERROR;
		}
	} else {
		return PARSE_NULL;
	}
}
