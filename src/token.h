/* token.h
 * author: Andrew Klinge
*/

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdbool.h>

struct Token {
	int id; // see enum tokens
	int ln; // line number this token originated from
	char *string; // the origin text 
};

enum tokens { 
	TOKSEC_END_MARKED_BY_NEXT_START,
		TOKEN_INT_LITERAL,
		TOKEN_FLOAT_LITERAL,
		TOKEN_IDENTIFIER,
		TOKEN_PREPROCESSOR_CMD,
		TOKEN_OPERATOR_DIVIDE,
	TOKSEC_END_MARKED_BY_NEXT_END,  	

	TOKEN_END_OF_STATEMENT,
	TOKEN_EOF,
	TOKEN_STRING_LITERAL,
	TOKEN_LIST_SEPARATOR,
	TOKEN_GROUP_OPEN,
	TOKEN_GROUP_CLOSE,
	TOKEN_BLOCK_OPEN,
	TOKEN_BLOCK_CLOSE,
	TOKEN_LIST_OPEN,
	TOKEN_LIST_CLOSE,
	TOKEN_OPERATOR
};

bool token_end_marked_by_next(int tokenID);

#endif
