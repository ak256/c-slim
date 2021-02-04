// token.h

#ifndef _TOKEN_H_
#define _TOKEN_H_

enum tokens { 
    TOK_INTOK_LITERAL,
    TOK_FLOATOK_LITERAL,
    TOK_OPERATOR_SLASH,          // for differentiating between /'s and comments
    TOK_IDENTIFIER,              // variable name, type, etc.
    TOKSEC_CHAR_TERMINATED,      // token section
    // anything above this line will have a regex terminator char (see scanner)
    TOK_END,                     // end of statement
    TOK_STRING_LITERAL,
    TOK_LISTOK_SEPARATOR,        // a comma
    TOK_GROUP_OPEN,              // for grouping things, order of operations
    TOK_GROUP_CLOSE,
    TOK_BLOCK_OPEN,              // a scope block
    TOK_BLOCK_CLOSE,
    TOK_LISTOK_OPEN,             // arrays
    TOK_LISTOK_CLOSE,
    TOK_OPERATOR, 
    // anything below this line is a preprocessor command token
    TOKSEC_PP,
    TOK_PP_INCLUDE,
    TOK_PP_DEFINE
};

typedef struct Token {
    int id; // token type (see enum tokens)
    int ln; // line number this token originated from
    char *string; // the text
} Token;

#endif
