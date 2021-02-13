// token.h

#ifndef _TOKEN_H_
#define _TOKEN_H_

enum tokens { 
    TOKSEC_CHAR_TERMINATED_START,
    TOK_INT_LITERAL,
    TOK_FLOAT_LITERAL,
    TOK_OPERATOR_SLASH,          // for differentiating between /'s and comments
    TOK_IDENTIFIER,              // variable name, type, etc.
    TOKSEC_CHAR_TERMINATED_END,  // token section
    // anything above this line will have a regex terminator char (see scanner)
    TOK_END,                     // end of statement
    TOK_STRING_LITERAL,
    TOK_LIST_SEPARATOR,        // a comma
    TOK_GROUP_OPEN,              // for grouping things, order of operations
    TOK_GROUP_CLOSE,
    TOK_BLOCK_OPEN,              // a scope block
    TOK_BLOCK_CLOSE,
    TOK_LIST_OPEN,             // arrays
    TOK_LIST_CLOSE,
    TOK_OPERATOR, 
    // anything below this line is a preprocessor command token
    TOKSEC_PP_START,
    TOK_PP_INCLUDE,
    TOK_PP_DEFINE,
    TOKSEC_PP_END
};

typedef struct Token {
    int id; // token type (see enum tokens)
    int ln; // line number this token originated from
    char *string; // the text
} Token;

#endif
