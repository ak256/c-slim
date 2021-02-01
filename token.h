// token.h

#ifndef _TOKEN_H_
#define _TOKEN_H_

enum tokens { 
    T_INT_LITERAL,
    T_FLOAT_LITERAL,
    T_OPERATOR_SLASH,           // for differentiating between /'s and comments
    T_IDENTIFIER,               // variable name, type, etc.
    // anything above this line will have a regex terminator char (see scanner)
    T_END,                      // end of statement
    T_STRING_LITERAL,
    T_LIST_SEPARATOR,           // a comma
    T_GROUP_OPEN,               // for grouping things, order of operations
    T_GROUP_CLOSE,
    T_BLOCK_OPEN,               // a scope block
    T_BLOCK_CLOSE,
    T_LIST_OPEN,                // arrays
    T_LIST_CLOSE,
    T_OPERATOR, 
    // anything below this line is a preprocessor command token
    T_PP_INCLUDE,
    T_PP_DEFINE
};

typedef struct Token {
    int id; // token type (see enum tokens)
    char *string; // the text
} Token;

#endif
