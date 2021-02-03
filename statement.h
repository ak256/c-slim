/* statement.h - a sequence of tokens that has a specific purpose
 * author: Andrew Klinge
*/

#ifndef _STATEMENT_H_
#define _STATEMENT_H_

enum statements { 
    STA_VAR_DECL,
    STA_FUNC_DECL,
};

typedef struct Statement {
    int id; // enum statements
} Statement;

#endif
