/* compiler.c
 * C-Slim language compiler. c-slim code -> bytecode for usage by interpreter
 * author: Andrew Klinge
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#include "compiler.h"
#include "token.h"

// enables all debugging output
#define DEBUG_ALL 1
// enables specific debugging output
#define DEBUG_TOKENS 0
#define DEBUG_STATEMENTS 0

#define VERSION "0.2.3"
#define REGEX_FLAGS (REG_EXTENDED | REG_NOSUB)
#define CSLIM_TOKEN_REGEXES_COUNT 15

/* Compiles the file with the given file path to bytecode at the output path.
 * Returns: whether successful.
 */
bool compiler_compile(struct Compiler *compiler, char *file_name) {
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file %s\n", file_name);
		return false;
	}

	int ln = 1; // line number
	bool success = false;
	while (true) {
		struct Token token;
		int scan_result = scanner_scan(&compiler->scanner, file, &ln, &token);
		if (scan_result == SCAN_ERROR) break;
		if (scan_result == SCAN_NULL) continue;
		if (token.id == TOKEN_EOF) {
			success = true;
			break;
		}
		if (DEBUG_TOKENS || DEBUG_ALL)
			printf("@%i [%i] %s\n", ln, token.id, token.string);

		struct Statement statement;
		int parse_result = parser_parse(&compiler->parser, &compiler->symtable, &token, &statement);
		if (parse_result == PARSE_ERROR) break;
		if (parse_result == PARSE_NULL) continue;
		if (DEBUG_STATEMENTS || DEBUG_ALL)
			printf("@%i  |-> [%i]\n", ln, statement.id);
	}
	fclose(file);
	return success;
}

static inline void print_help() {
	printf("C-Slim compiler usage:\n"
		"\targs: <file1> [file2, file3, ...]\n"
		"\t--help ... print this page\n"
		"\t--version ... print version\n");
}

/* Attempts to compile a regular expression string to the given regex data struct.
 * Exits if fails.
 *
 * tokenID - corresponding token that the regex will identify in code
 * regexstr - the regular expression to match code
 */
static struct TokenRegex create_token_regex(int tokenID, const char *regexstr) {
	struct TokenRegex tr;
	tr.tokenID = tokenID;
	int error = regcomp(&tr.regex, regexstr, REGEX_FLAGS);
	if (error) {
		fprintf(stderr, "Regex creation failed! %s\n", regexstr);
		const int errStrSize = 1024;
		char errStr[errStrSize];
		regerror(error, &tr.regex, errStr, errStrSize);
		fprintf(stderr, "%s\n", errStr);
		exit(EXIT_FAILURE);
	}
	return tr;
}

/* Compiles C-Slim input files. */
int main(int arg_count, char **args) {
	char *input_files[arg_count - 1];
	int input_files_count = 0;
	for (int i = 1; i < arg_count; i++) {
		char *arg = args[i];
		if (strcmp("--help", arg) == 0) {
			print_help();
		} else if (strcmp("--version", arg) == 0) {
			printf("C-Slim compiler version %s\n", VERSION);
		} else if (arg[0] == '-') {
			fprintf(stderr, "Unknown option %s\nTry --help\n", arg);
			return EXIT_FAILURE;
		} else {
			input_files[input_files_count] = arg;
			input_files_count++;
		}
	}
	if (input_files_count == 0) {
		fprintf(stderr, "No input files\n");
		print_help();
		return EXIT_FAILURE;
	}

	struct TokenRegex cslim_token_regexes[CSLIM_TOKEN_REGEXES_COUNT] = {
		create_token_regex(TOKEN_END_OF_STATEMENT, "^;"),
		create_token_regex(TOKEN_OPERATOR_DIVIDE, "^/[^/]"),
		create_token_regex(TOKEN_OPERATOR, "^[-.~!$%^&*+=|:?]"),
		create_token_regex(TOKEN_LIST_SEPARATOR, "^,"),
		create_token_regex(TOKEN_GROUP_OPEN, "^\\("),
		create_token_regex(TOKEN_GROUP_CLOSE, "^\\)"),
		create_token_regex(TOKEN_BLOCK_OPEN, "^\\{"),
		create_token_regex(TOKEN_BLOCK_CLOSE, "^\\}"),
		create_token_regex(TOKEN_IDENTIFIER, "^[a-zA-Z_][a-zA-Z0-9_]*[^a-zA-Z0-9_]$"),
		create_token_regex(TOKEN_FLOAT_LITERAL, "^([0-9]+)[.]([0-9]+)[^0-9]$"),
		create_token_regex(TOKEN_INT_LITERAL, "^([0-9]+)[^0-9.]$"),
		create_token_regex(TOKEN_STRING_LITERAL, "^\"([^\\\"]|\\\\.)*\""),
		create_token_regex(TOKEN_LIST_OPEN, "^\\["),
		create_token_regex(TOKEN_LIST_CLOSE, "^\\]"),
		create_token_regex(TOKEN_PREPROCESSOR_CMD, "^#([a-zA-Z]+) ")
	};

	struct Compiler compiler;
	scanner_init(&compiler.scanner, cslim_token_regexes, CSLIM_TOKEN_REGEXES_COUNT);
	parser_init(&compiler.parser);
	symtable_init(&compiler.symtable);

	int compiled_count = 0;
	for (int i = 0; i < input_files_count; i++) {
		if (compiler_compile(&compiler, input_files[i]))
			compiled_count++;
	}

	if (compiled_count == input_files_count) {
		printf("SUCCESS! Compiled all %i input files\n", input_files_count);
		return EXIT_SUCCESS;
	} else {
		printf("FAILURE! Compiled %i out of %i input files\n", compiled_count, input_files_count);
		return EXIT_FAILURE;
	}
}
