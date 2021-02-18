/* statement.c
 * author: Andrew Klinge
*/

#include "statement.h"

#define INVALID_STATEMENT_ID -1
const Statement INVALID_STATEMENT = {.id = INVALID_STATEMENT_ID};

/* Returns whether the statement's data is valid. */
inline bool statement_valid(Statement *stmnt) {
	return stmnt->id != INVALID_STATEMENT_ID;
}
