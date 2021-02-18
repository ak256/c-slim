/* token.c
 * author: Andrew Klinge
*/

#include "token.h"

#define INVALID_TOKEN_ID -1

const Token INVALID_TOKEN = {.id = INVALID_TOKEN_ID};

/* Returns whether the token contains valid data. */
inline bool token_valid(Token *token) {
	return token->id != INVALID_TOKEN_ID;
}

/* Returns whether the tokenID corresponds with a regex string
 * that ends with some extra terminator character, which will need
 * to be rescanned. See scanner for implementation detail.
 */
inline bool token_char_terminated(int tokenID) {
	return tokenID > TOKSEC_CHAR_TERMINATED_START && tokenID < TOKSEC_CHAR_TERMINATED_END;
}
