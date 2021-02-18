/* symbol_table.h - data structure for managing code symbols
 * author: Andrew Klinge
*/

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include "utils/array.h"

extern const int SYMTABLE_MAX_SCOPES;

enum symbols {
	SYM_VAR,
	SYM_FUNC,
	SYM_STRUCT
};

/* group of code symbols within scopes. */
typedef struct SymTable {
	// Array scopes -> HashTable decl -> Sym decl
	Array scopes; // lists of symbols, one per scope. highest scope is first
} SymTable;

/* an entry in the symbol table. */
typedef struct Sym {
	char id; // enum symbols
	char *name;
} Sym;

void symtable_init(SymTable *tbl);
void symtable_deinit(SymTable *tbl);
void symtable_add(SymTable *tbl, Sym *sym);

int symtable_push_scope(SymTable *tbl);
int symtable_pop_scope(SymTable *tbl);

Sym *symtable_get(SymTable *tbl, char *sym_name);

#endif
