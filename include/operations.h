#ifndef BUILTINS_H
#define BUILTINS_H

#include "pval.h"

pval *op_add(pval *args);
pval *op_sub(pval *args);
pval *op_mul(pval *args);
pval *op_div(pval *args);
pval *op_eq(pval *args);
pval *op_quit(pval *args);
pval *op_less(pval *args);
pval *op_greater(pval *args);
pval *op_not(pval *args);
pval *op_less_equal(pval *args);
pval *op_greater_equal(pval *args);
pval *func_ptr_helper(op_func op);
int symbol_check(char *in);

typedef struct {
    char *name;
    op_func fn;
} operation;


pval *insert_op(pval *args);


extern operation operations[];
extern int op_len;

#endif