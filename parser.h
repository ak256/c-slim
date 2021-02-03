/* parser.h
 * author: Andrew Klinge
*/

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>

#include "token.h"
#include "statement.h"

Statement *parser_parse_statement();

#endif
