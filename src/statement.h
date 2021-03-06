/* statement.h
 * author: Andrew Klinge
*/

#ifndef __STATEMENT_H__
#define __STATEMENT_H__

struct Statement {
    int id; // enum statements
    int arg_count;
    char **args;
};

enum statements { 
    STATEMENT_BREAK,
    STATEMENT_BREAK_LABEL,
    STATEMENT_VAR_DECL,
    STATEMENT_FUNC_DECL
};

#endif
