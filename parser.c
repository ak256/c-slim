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
	// so we know not to free random memory when overwriting in tokenbuf
	parser->tokenbuf[0] = INVALID_TOKEN;
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
 * Variable arguments at end are for error_string format args. 
 */
static void assert(int condition, Parser *parser, const char *error_string, ...) {
	if (condition) return;

	// print custom error message format string
	va_list va;
	va_start(va, error_string);
	vfprintf(stderr, error_string, va);
	va_end(va);
	// print line occurrence and token
	char *line_string = tokens_to_line(parser->tokenbuf, parser->tokenbuf_count);
	fprintf(stderr, " at line %i: \n\t%s\n", parser->tokenbuf[0].ln, line_string);

	exit(1);
}

/* Rebuilds string from list of tokens' strings. */
static char *tokens_to_line(Token *buf, int buflen) {
	// get length of string
	// NOTE: we put insert between tokens
	int length;
	for (int i = 0; i < buflen; i++) {
		length += strlen(buf[i].string) + 1; // +1 for space
	}
	
	// build string
	char *string = malloc(sizeof(char) * length);
	int at = 0; // index in string
	for (int i = 0; i < buflen; i++) {
		// copy token string into our line string
		char *copy_string = buf[i].string;
		int j = 0; // index in copy_string
		while (copy_string[j] != '\0') {
			string[at + j] = copy_string[j];
			j++;
		}
		string[at + j] = ' '; // insert space between tokens
		at += j + 1;
	}
	string[at - 1] = '\0';
	return string;
}

/* Copies the next token to the parser's buffer and will attempt to parse
 * and return a statement as soon as it recognizes one. Also creates.
 * symbols in the SymTable.
 * WARNING: always check that the returned statement is valid!
 */
Statement parser_parse(Parser *parser, SymTable *symtable, Token *token) {
	assert(parser->tokenbuf_count < PARSER_TOKENBUF_SIZE,
		parser, "Exceeded maximum number of tokens per statement (%i)", PARSER_TOKENBUF_SIZE);
	
	// manage scopes
	if (token->id == TOKEN_BLOCK_OPEN) {
		assert(!symtable_push_scope(symtable),
			parser, "Exceeded maximum number of nested scopes (%i)", SYMTABLE_MAX_SCOPES);
		return INVALID_STATEMENT;
	} else if(token->id == TOKEN_BLOCK_CLOSE) {
		assert(!symtable_pop_scope(symtable),
			parser, "Unexpected scope block closing statement, no scope to close!");
		return INVALID_STATEMENT;
	}

	// free string memory when overwriting previous token
	free(parser->tokenbuf[parser->tokenbuf_count].string); 
	// add token to buffer
	parser->tokenbuf[parser->tokenbuf_count] = *token;
	parser->tokenbuf_count++;

	// form statement if at end of one
	if (token->id == TOKEN_END) {
		int count = parser->tokenbuf_count - 1; // ignore END
		if (count == 0) {
			parser->tokenbuf_count = 0; // 'clear' buffer
			return INVALID_STATEMENT; // empty statement
		}
		
		// parse statement
		Token *buf = parser->tokenbuf;
		Statement stmnt = INVALID_STATEMENT;
		if (buf[0].id == TOKEN_PREPROCESSOR_CMD) {
			// stmnt is always null for preprocessor commands
			char *cmd = buf[0].string + 1; // skip '#' char

			if (!strcmp("include", cmd)) {
				// #include "path"
				assert(count == 2 && buf[1].id == TOKEN_STRING_LITERAL,
					parser, "Invalid include statement! Expected: #include \"path\";");
				char *string = strtok_to_string(buf[1].string);
				hashtable_add(&parser->included_files, hash_string(string), string);
				goto _RETURN_;
			}
			if (!strcmp("define", cmd)) {
				// #define identifier "definition"
				// TODO
				goto _RETURN_;
			}
		} else {
			// other statement
			if (buf[0].id == TOKEN_IDENTIFIER) {
				char *identifier = buf[0].string;
				if (!strcmp("break", identifier)) {
					assert(count <= 2,
						parser, "Invalid break statement! Expected: break; OR: break label;");

					if (count == 2) {
						// break label;
						assert(buf[1].id == TOKEN_IDENTIFIER,
							parser, "Invalid break statement! Expected label identifier, ex: break label;");
						assert(symtable_get(symtable, buf[1].string) != NULL,
							parser, "Undefined label identifier \"%s\"", buf[1].string);

						stmnt.id = STATEMENT_BREAK_LABEL;
						stmnt.args = buf[1].string;
						stmnt.arg_count = 1;
					} else {
						// break;
						stmnt.id = STATEMENT_BREAK;
					}
					goto _RETURN_;
				}
			}
		}

		_RETURN_: // reset token buffer and return statement
		parser->tokenbuf_count = 0; // 'clear' buffer
		return stmnt;
	}
	return INVALID_STATEMENT;
}
