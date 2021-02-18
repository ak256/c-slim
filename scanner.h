// scanner.h

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include "token.h"

typedef struct Scanner {
    char *buf; // input character buffer
} Scanner;

void scanner_init(Scanner *scanner);

Token scanner_scan(Scanner *scanner, FILE *file, int *ln);

#endif
