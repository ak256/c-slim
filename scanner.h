// scanner.h

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <stdio.h>
#include "token.h"

void scanner_init();
Token *scanner_scan_token(FILE *file, int *ln);

#endif
