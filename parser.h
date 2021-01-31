// parser.h

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include "token.h"

void parser_init();
Token *parse_token(FILE *file, int *ln);

#endif
