/* scanner.h
 * author: Andrew Klinge
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include <regex.h>

#include "token.h"

enum scan_code {
	SCAN_NULL,
	SCAN_ERROR,
	SCAN_VALID
};

struct TokenRegex {
	int tokenID;
	regex_t regex;
}

struct Scanner {
	struct TokenRegex *token_regexes;
	int token_regexes_count;
    char *buf; // input character buffer
};

void scanner_init(struct Scanner *scanner);

int scanner_scan(struct Scanner *scanner, FILE *file, int *ln, struct Token *output);

#endif
