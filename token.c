// token.c

#include <stdlib.h>

#include "token.h"

/* Frees the token's resources. Does NOT free the token. */
void token_deinit(Token *token) {
    free(token->string);
}
