/* compiler.c
 * C-Slim language compiler. c-slim code -> bytecode for usage by interpreter
 * author: Andrew Klinge
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "compiler.h"
#include "token.h"

// enables all debugging output
#define DEBUG_ALL 0
// enables specific debugging output
#define DEBUG_TOKENS 0
#define DEBUG_STATEMENTS 0

const char *VERSION = "0.2.0";

void compiler_init(Compiler *compiler) {
	symtable_init(&compiler->symtable);
	scanner_init(&compiler->scanner);
	parser_init(&compiler->parser);
}

static void print_help() {
	printf("C-Slim compiler help page:\n");
	printf("\targs: <file1> [file2, file3, ...]\n");
	printf("\t-h --help ... print this page\n");
	printf("\t-v --version ... print version\n");
}

static void print_version() {
	printf("C-Slim compiler version %s\n", VERSION);
}

/* Parses command-line arguments for compiler program.
 * Returns: the number of input files returned
 * 
 * input_files - array to store input file names in
 * verbose - will store whether --verbose flag was present
 */
static int parse_program_args(int arg_count, char **args, char **input_files, bool *verbose) {
	int input_files_count = 0;
	for (int i = 1; i < arg_count; i++) {
		char *arg = args[i];
		if (strcmp("-h", arg) == 0 || strcmp("--help", arg) == 0) {
			print_help();
		} else if (strcmp("--version", arg) == 0) {
			print_version();
		} else if (strcmp("-v", arg) == 0 || strcmp("--verbose", arg) == 0) {
			*verbose = true;
		} else {
			input_files[input_files_count] = arg;
			input_files_count++;
		}
	}
	return input_files_count;
}

/* Compiles the file with the given file path to bytecode at the output path.
 * Returns: whether successful.
 */
bool compiler_compile(Compiler *compiler, char *file_name) {
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file %s\n", file_name);
		return 0;
	}

	int ln = 1; // line number
	bool success = false;
	while (1) {
		Token token = scanner_scan(&compiler->scanner, file, &ln);
		if (!token_valid(&token)) break;
		if (token.id == TOKEN_EOF) {
			success = true;
			break;
		}
		if (DEBUG_TOKENS || DEBUG_ALL)
			printf("@%i [%i] %s\n", ln, token.id, token.string);

		Statement stmnt = parser_parse(&compiler->parser, &compiler->symtable, &token);
		if (!statement_valid(&stmnt)) break;
		if (DEBUG_STATEMENTS || DEBUG_ALL)
			printf("@%i  |-> [%i]\n", ln, stmnt.id);
	}
	fclose(file);
	return success;
}

int main(int argc, char **argv) {
	if (argc <= 1) {
		print_help();
		return EXIT_SUCCESS;
	}
	
	char *input_files[argc - 1];
	bool verbose;
	int input_files_count = parse_program_args(argc, argv, input_files, &verbose);
	if (input_files_count == 0) return EXIT_SUCCESS;
	
	Compiler compiler;
	compiler_init(&compiler);

	int compiled_count = 0;
	for (int i = 0; i < input_files_count; i++) {
		if (compiler_compile(&compiler, input_files[i]))
			compiled_count++;
	}

	if (compiled_count == input_files_count) {
		if (verbose)
			printf("SUCCESS! Compiled all %i input files\n", input_files_count);
		return EXIT_SUCCESS;
	} else {
		if (verbose) 
			printf("FAILURE! Successfully compiled %i out of %i input files\n",
				compiled_count, input_files_count);
		return EXIT_FAILURE;
	}
}
