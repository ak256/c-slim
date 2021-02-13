/* compiler.c
 * author: Andrew Klinge
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "token.h"
#include "scanner.h"

// enables all debugging output
#define DEBUG_ALL 0
// enables token debugging output
#define DEBUG_TOKENS 0
#define DEBUG_STATEMENTS 0

static const char *VERSION = "0.1.2";

void print_help() {
    printf("C-Slim compiler Help Page:\n");
    printf("\targs: <file1> [file2, file3, ...]\n");
    printf("\t-h --help ... print this page\n");
    printf("\t-v --version ... print version\n");
}

void print_version() {
    printf("C-Slim compiler version %s\n", VERSION);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        print_help();
        return 0;
    }
    
    // parse args and input files
    int input_files_count = 0;
    char **input_files = malloc(sizeof(char*) * (argc - 1));

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (!strncmp("-h", arg, 2) || !strncmp("--help", arg, 6)) {
            print_help();
            return 0;
        } else if (!strncmp("-v", arg, 2) || !strncmp("--version", arg, 9)) {
            print_version();
            return 0;
        }

        // file name
        input_files[input_files_count] = arg;
        input_files_count++;
    }
    
    // initialize compiler parts
    SymTable symtable;
    Scanner scanner;
    Parser parser;

    symtable_init(&symtable);
    scanner_init(&scanner);
    parser_init(&parser);

    // compile each input file
    for (int i = 0; i < input_files_count; i++) {
        FILE *file = fopen(input_files[i], "r");
        if (!file) {
            fprintf(stderr, "Failed to open file %s\n", input_files[i]);
            return 1;
        }

        int ln = 1; // line number
        Token *token;
        while ((token = scanner_scan_token(&scanner, file, &ln)) != NULL) {
            #if DEBUG_TOKENS || DEBUG_ALL
                printf("@%i [%i] %s\n", ln, token->id, token->string);
            #endif

            Statement *stmnt = parser_parse_statement(&parser, &symtable, token);
            // token_deinit(token); // don't do this. we save token.string char* 
            free(token);

            if (stmnt != NULL) {
                #if DEBUG_STATEMENTS || DEBUG_ALL
                    printf("@%i  |-> [%i]\n", ln, stmnt->id);
                #endif
            }
        }
        fclose(file);
    }
    return 0;
}
