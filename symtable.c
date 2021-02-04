/* symtable.c
 * author: Andrew Klinge
*/

#include <stdlib.h>

#include "symtable.h"
#include "utils/hashtable.h"

// max number of nested scopes
const int SYMTABLE_MAX_SCOPES = 127;

/* Initializes a SymTable's resources. */
void symtable_init(SymTable *tbl) {
    array_init(&tbl->scopes, 8);
}

/* Deinitializes a SymTable's resources. Does NOT free the table. */
void symtable_deinit(SymTable *tbl) {
    array_deinit(&tbl->scopes);
}

/* Returns 1 if error (max # scopes exceeded) else 0. */
int symtable_push_scope(SymTable *tbl) {
    if (tbl->scopes.count >= SYMTABLE_MAX_SCOPES) return 1;
    HashTable *hashtbl = malloc(sizeof(HashTable));
    array_add(&tbl->scopes, hashtbl);
    hashtable_init(hashtbl, 8, 0);
    return 0;
}

/* Removes the current scope. Returns 1 if error (no scopes to remove) else 0.
*/
int symtable_pop_scope(SymTable *tbl) {
    if (tbl->scopes.count <= 0) return 1;
    HashTable *scope = (HashTable*) tbl->scopes.items[tbl->scopes.count - 1];
    hashtable_deinit(scope);
    free(scope);
    tbl->scopes.count--;
    return 0;
}

/* Adds a symbol to the current scope. Does nothing if no scopes. */
void symtable_add(SymTable *tbl, Sym *sym) {
    if (tbl->scopes.count <= 0) return;
    HashTable *scope = (HashTable*) tbl->scopes.items[tbl->scopes.count - 1];
    hashtable_add(scope, hash(sym->name), sym);
}

/* Gets the symbol by name, searching first in local scope and continuing to
 * up to global scope. Returns null if nothing found.
 */
Sym *symtable_get(SymTable *tbl, char *sym_name) {
    if (tbl->scopes.count <= 0) return NULL;
    Sym *sym = NULL;
    int hashcode = hash(sym_name);
    int at = tbl->scopes.count - 1;
    while ((sym = hashtable_get((HashTable*) tbl->scopes.items[at], hashcode))
        == NULL) {
        at--;
        if (at < 0) break; // searched all scopes 
    }
    return sym;
}
