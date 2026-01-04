// #include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/SinglyLinkedList.h"
#include "../include/operations.h"
#include "../include/pval.h"
#include "../include/token.h"
#include "../include/env.h"
#include "../include/cell.h"

pval *evaluate(pval *pval_list) {
    if (pval_list == NULL) {
        return NULL;
    }
    pval *res = _pval_evaluate(pval_list);
    pval_release(pval_list);
    return res;
}

pval *parse(token_stream *tk_stream) {
    if (tk_stream->count == 0) {
        return pval_bool(false);
    }
    pval *out;
    out =_parse_exp(0, tk_stream);
    
    // if((tk_stream->index != tk_stream->count - 1) && tk_stream->error == false){
    //     fprintf(stderr, "incomplete-parse\n");
    //     pval_release(out);
    //     return NULL;
    // }
    return out;

}

pval *_parse_exp(int idx, token_stream *tk_stream) {
    tk_stream->index = peek_next_token_idx(idx, tk_stream);
    token cur_tk = tk_stream->data[tk_stream->index];

    switch (cur_tk.type) {
        case TOK_INVALID:
            return NULL;
        case TOK_NUM:
            return pval_number(atoi(cur_tk.lexeme));
        case TOK_BOOL:
            if (strcmp("Bool(true)", cur_tk.lexeme) == 0) {
                return pval_bool(true);
            }
            return pval_bool(false);
        case TOK_COMMENT:
            return pval_str(cur_tk.lexeme);
        case TOK_SYM:
            return pval_symbol(cur_tk.lexeme);
        case TOK_LIST_L:
            // ++tk_stream->index;
            int peek_idx = peek_next_token_idx(++tk_stream->index, tk_stream);
            if(tk_stream->count == 1){
                return NULL;
            }
            pval *result = _parse_list(peek_idx, tk_stream);
             
            if (result != NULL) {                                        
                return result;          //<============ func exit!
            }
            
            return NULL;
        case TOK_LIST_R:
            fprintf(stderr, "invalid-token: )\n");
            tk_stream->error = true;
            return NULL;
        default:
            if (cur_tk.type == TOK_WS) {
                return pval_bool(false);
            }
            return NULL;
    }
}

pval *_parse_list(int idx, token_stream *tk_stream) {
    pval *new_list = pval_empty_list();
    tk_stream->index = idx;

    while (tk_stream->index < tk_stream->count) {
        if (tk_stream->data[tk_stream->index].type == TOK_LIST_R) {
            return new_list;
        }
        pval *result = _parse_exp(tk_stream->index, tk_stream);

        if (result == NULL) {
            pval_release(new_list);
            return NULL;
            tk_stream->error = true;

        } else {
            append_node(new_list->value.val_list, create_node(result));
            result = pval_release(result);
            tk_stream->index = peek_next_token_idx(++tk_stream->index, tk_stream);
        }
    }
    pval_release(new_list);
    return NULL;
    tk_stream->error = true;
}

void interactive_input(void) {
    int cnt = 0;
    while (1)
      {
        printf("%d psi> ", cnt);
        token_stream *tk_stream = tokenize(stdin);

        print_tokens(tk_stream);

        printf("\n\n");

        pval *out = parse(tk_stream);
        // /* Ensure token stream is always freed */
        free_token_stream(tk_stream);

        out = evaluate(out);

        printf("answer: ");

        out = pval_print(out);
        printf("\n");

        pval_release(out);
        cnt ++;
    }
}

void debug_input(char *buf)
{
    printf("psi> ");

    token_stream *tk_stream = tokenize(fmemopen(buf, strlen(buf), "r"));
    print_tokens(tk_stream);

    printf("\n");

    pval *out = parse(tk_stream);
    // /* Ensure token stream is always freed */
    free_token_stream(tk_stream);



    out = evaluate(out);

    printf("answer: ");
    out = pval_print(out);

    out = pval_release(out);

    // if (out) {
    //     out = evaluate(out);
    //     if (out) {
    //         pval_print(out);
    //         out = pval_free(out);
    //     }
    //     // pval_free(out);
    // } else {
    //     printf("(no output)\n");
    // }

    // printf("\n\n");
}

int main() {
    init_global();
    char input[] = "";

    if (strcmp(input, "") != 0){
        debug_input(input);
    }
    else
    {
        interactive_input();
    }
    return 0;
}
