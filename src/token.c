/* token.c
 * author: Andrew Klinge
*/

#include "token.h"

/* Returns whether the tokenID corresponds with a regex string
 * that is ended by the first character of the next token, which will need
 * to be rescanned. See scanner for implementation detail.
 */
inline bool token_end_marked_by_next(int tokenID) {
	return tokenID > TOKSEC_END_MARKED_BY_NEXT_START
		&& tokenID < TOKSEC_END_MARKED_BY_NEXT_END;
}
