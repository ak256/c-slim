/* token.h
 * author: Andrew Klinge
*/

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdbool.h>

typedef struct Token {
	int id; // token type (see enum tokens)
	int ln; // line number this token originated from
	char *string; // the text
} Token;

extern const Token INVALID_TOKEN;

// TOKSEC is a token section. Currently tokens fit into one of these sections,
// which the scanner uses to extract them differently
enum tokens { 
	TOKSEC_CHAR_TERMINATED_START,	// has a regex terminator character (see scanner)
		TOKEN_INT_LITERAL,
		TOKEN_FLOAT_LITERAL,
		TOKEN_OPERATOR_SLASH,	 	// for differentiating between /'s and comments
		TOKEN_IDENTIFIER,		  	// variable name, type, etc.
	TOKSEC_CHAR_TERMINATED_END,  	

	TOKEN_END,						// end of statement
	TOKEN_EOF,						// represents valid end of token stream
	TOKEN_STRING_LITERAL,
	TOKEN_LIST_SEPARATOR,			// a comma
	TOKEN_GROUP_OPEN,				// for grouping things, order of operations
	TOKEN_GROUP_CLOSE,
	TOKEN_BLOCK_OPEN,				// a scope block
	TOKEN_BLOCK_CLOSE,
	TOKEN_LIST_OPEN,				// arrays
	TOKEN_LIST_CLOSE,
	TOKEN_OPERATOR, 
	TOKEN_PREPROCESSOR_CMD
};

bool token_valid(Token *token);
bool token_char_terminated(int tokenID);

#endif
