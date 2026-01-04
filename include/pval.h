#include <stdbool.h>
#include <stdint.h>

#ifndef PVAL_H
#define PVAL_H

// Forward-declare structs that are linked later on (prevents cyclical includes)
typedef struct SinglyLinkedList SinglyLinkedList;
typedef struct linked_node linked_node;
typedef struct pval pval;
typedef struct token_stream token_stream;

typedef pval *(*op_func)(pval *args); // this should take a pval_list actually. The arguments will be within it.

typedef enum
{
    PVAL_BOOL,
    PVAL_NUMBER,
    PVAL_SYMBOL,
    PVAL_FUNCTION,
    PVAL_LIST,
    PVAL_ERROR,
    PVAL_STRING,
} pval_type;


typedef struct error
{
    pval *pval_error;
    char *message;
} error;

typedef struct pval
{
    pval_type type;
    int refcnt;
    union
    {
        bool val_bool;
        int64_t val_number;
        char *val_symbol;
        op_func val_function;
        SinglyLinkedList *val_list;
        error *val_error;
        char *val_string;

    } value;
} pval;

/* pval helper function prototypes */
pval *pval_number(int64_t n);
pval *pval_bool(bool b);
pval *pval_symbol(char *symbol_buf);
pval *pval_error(pval *pv, char *msg);
pval *pval_empty_list(void);
pval *pval_str(char *str);
pval *pval_retain(pval *pv);
pval *pval_release(pval *pv);

    /* more helper declarations pulled from the pval.c file */

    char *pval_type_to_string(pval *pv);
void pval_add(pval *list, pval *pv);
void pval_print_error(pval *pv);
pval *pval_print(pval *pv);
pval *pval_free(pval* pv);
pval *pval_list_handler(pval *pvals, pval *(*func)(pval *args), bool copy);
pval *pval_copy(pval *src);
void pval_delete(pval *pv);
pval *evaluate(pval *pv);
pval *_pval_evaluate(pval *pv);

pval *parse(token_stream *tk_stream);
pval *_parse_exp(int idx, token_stream *tk_stream);
pval *_parse_list(int idx, token_stream *tk_stream);
#endif