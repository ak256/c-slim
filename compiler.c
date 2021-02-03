/* compiler.c
 * author: Andrew Klinge
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

static const char *VERSION = "0.1.1";

void print_help() {
    printf("C-Slim compiler Help Page:\n\t-h --help ... print this page\n\t-v --version ... print version\n");
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
    
    // compile each input file
    scanner_init();
    for (int i = 0; i < input_files_count; i++) {
        FILE *file = fopen(input_files[i], "r");
        if (!file) {
            fprintf(stderr, "Failed to open file %s\n", input_files[i]);
            return 1;
        }

        int ln = 1; // line number
        Token *token;
        while ((token = scanner_scan_token(file, &ln)) != NULL) {
            printf("  [%i] %s\n", token->id, token->string);
        }
        fclose(file);
    }
    return 0;
}
