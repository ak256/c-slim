/* compiler.h
 * author: Andrew Klinge
*/

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"
#include "parser.h"

typedef struct Compiler {
	SymTable symtable;
	Scanner scanner;
	Parser parser;
} Compiler;

void compiler_init(Compiler *compiler);

bool compiler_compile(Compiler *compiler, char *file_name);

#endif
