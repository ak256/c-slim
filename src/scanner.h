// scanner.h

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include "token.h"

enum scan_code {
	SCAN_NULL,
	SCAN_ERROR,
	SCAN_VALID
};

typedef struct Scanner {
    char *buf; // input character buffer
} Scanner;

void scanner_init(Scanner *scanner);

int scanner_scan(Scanner *scanner, FILE *file, int *ln, Token *output);

#endif
