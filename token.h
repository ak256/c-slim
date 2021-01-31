// token.h

#ifndef _TOKEN_H_
#define _TOKEN_H_

enum tokens { 
    T_END,                      // end of statement
    T_COMMENT,
    T_STRING_LITERAL,
    T_MEGACOMMENT_OPEN,         // for commenting-out code with comments 
    T_MEGACOMMENT_CLOSE,
    T_IDENTIFIER,               // variable name, type, etc.
    T_LIST_SEPARATOR,           // a comma
    T_GROUP_OPEN,               // for grouping things, order of operations
    T_GROUP_CLOSE,
    T_BLOCK_OPEN,               // a scope block
    T_BLOCK_CLOSE,
    T_LIST_OPEN,                // arrays
    T_LIST_CLOSE
};

typedef struct Token {
    int id; // token type (see enum tokens)
    char *string; // the text
} Token;

#endif
