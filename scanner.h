// scanner.h

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <stdio.h>
#include "token.h"

typedef struct Scanner {
    char *buf; // input character buffer
} Scanner;

void scanner_init();
Token *scanner_scan_token(Scanner *scanner, FILE *file, int *ln);

#endif
