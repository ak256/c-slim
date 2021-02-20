/* statement.h
 * author: Andrew Klinge
*/

#ifndef __STATEMENT_H__
#define __STATEMENT_H__

#include <stdbool.h>

// a sequence of tokens with a specific purpose
typedef struct Statement {
    int id; // enum statements
    int arg_count;
    char *args;
} Statement;

enum statements { 
    STATEMENT_BREAK,
    STATEMENT_BREAK_LABEL,
    STATEMENT_VAR_DECL,
    STATEMENT_FUNC_DECL
};

#endif
