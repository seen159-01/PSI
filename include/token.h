#include <stdlib.h>
#include <stdbool.h>
#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOK_WS,
    TOK_COMMENT,
    TOK_BOOL,
    TOK_NUM,
    TOK_LIT,
    TOK_SYM,
    TOK_LIST_L,
    TOK_LIST_R,
    TOK_INVALID,
}token_type;

typedef struct token{
    token_type type;
    char *lexeme;
}token;

typedef struct token_stream{
    token *data;
    size_t index;
    size_t count;
    size_t capacity;
    bool error;
    
}token_stream;

token_stream *tokenize(FILE *fp);
char buff_peek_next(char *buff, size_t idx);
int peek_next_token_idx(size_t idx, token_stream *tk_stream);
void print_tokens(token_stream *tk_stream);
void free_token_stream(token_stream *tk_stream);

#endif