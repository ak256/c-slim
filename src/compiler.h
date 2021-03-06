/* compiler.h
 * author: Andrew Klinge
*/

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"
#include "parser.h"

struct Compiler {
	struct SymTable symtable;
	struct Scanner scanner;
	struct Parser parser;
};

void compiler_init(struct Compiler *compiler);

bool compiler_compile(struct Compiler *compiler, char *file_name);

#endif
