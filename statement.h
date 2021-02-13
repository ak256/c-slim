/* statement.h - a sequence of tokens that has a specific purpose
 * author: Andrew Klinge
*/

#ifndef _STATEMENT_H_
#define _STATEMENT_H_

enum statements { 
    STMNT_BREAK,
    STMNT_BREAK_LABEL,
    STMNT_VAR_DECL,
    STMNT_FUNC_DECL,
};

typedef struct Statement {
    int id; // enum statements
    int arg_count;
    char *args;
} Statement;

#endif
