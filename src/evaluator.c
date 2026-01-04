#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/SinglyLinkedList.h"
#include "../include/cell.h"
#include "../include/env.h"
#include "../include/operations.h"
#include "../include/pval.h"
#include "../include/token.h"

char specials[6][6] = {"def", "if", "cons", "head", "tail", "quote"};


pval *eval_if(linked_node *node) {
    linked_node *n0 = node;
    linked_node *n1 = n0->next;
    linked_node *n2 = n1 ? n1->next : NULL;

    pval *a;
    pval *b;
    pval *c;

    if (!n1) {
        return pval_error(NULL, "arity-error");
    }

    if (((a = _pval_evaluate(n0->data))->type) != PVAL_BOOL) {
        return pval_error(a, "invalid-type");
    }
    if (a->value.val_bool) {
        b = _pval_evaluate(n1->data);

        return b;
    } else {
        if (n2 == NULL)
            return pval_bool(false);

        c = _pval_evaluate(n2->data);

        return c;
    }
}
pval *eval_def(linked_node *node) {
    binding *b;
    if (node->data->type != PVAL_SYMBOL) {
        return pval_error(node->data, "type-error");
    }
    if (!node->next) {
        return pval_error(node->data, "arity-error");
    }
    b = hash_find(env_global->table, node->data->value.val_symbol);

    if (b && b->is_protected) {
        return pval_error(node->data, "protected-symbol");
    }

    pval *in = _pval_evaluate(node->next->data);
    if (in->type == PVAL_ERROR)
        return in;

    if (b) {
        cell_set(b->cell, in);
        return in;
    } else {
        cell *c = cell_new(in);
        hash_probe_insert(env_global->table,
                          binding_new(node->data->value.val_symbol, c));
        cell_release(&c);
        return in;
    }
}

bool is_special_symbol(pval *pv) {
    for (int i = 0; i < 6; i++) {
        if (strcmp(specials[i], pv->value.val_symbol) == 0) {
            return true;
        }
    }
    return false;
}

pval *eval_special(pval *s, linked_node *node) {

    if (strcmp(s->value.val_symbol, "def") == 0) {
        return eval_def(node);
    } else if (strcmp(s->value.val_symbol, "if") == 0) {
        return eval_if(node);
    } else if (strcmp(s->value.val_symbol, "quote") == 0) {
        if (!node) {
            return pval_error(s, "arity-error");
        }
        if (node->next) {
            return pval_error(s, "arity-error");
        }

        pval *out = node->data;
        return pval_retain(out);
    } else if (strcmp(s->value.val_symbol, "cons") == 0){
        pval *head = node->data;
        pval *tail = node->next ? _pval_evaluate(node->next->data) : NULL;

        if (!head){
            return pval_error(NULL, "value-error");
        }
        else if(!tail){
            return pval_error(head, "value-error");
        }
        else if(tail->type != PVAL_LIST){
            //TODO: figure out how to print both elements
            return pval_error(tail, "value-error");
        }

        pval *list = pval_empty_list();
        
        pval *pv = pval_retain(head);
        append_node(list->value.val_list, create_node(pv));
        pv = pval_release(pv);

        linked_node *curr = tail->value.val_list->head;
        pval *tmp;

        while (curr) {
            tmp = pval_retain(curr->data);
            append_node(list->value.val_list, create_node(tmp));
            tmp = pval_release(tmp);
            curr = curr->next;
        }
        return pval_retain(list);
    } else if (strcmp(s->value.val_symbol, "head") == 0) {
        pval *lst = _pval_evaluate(node->data);
        if(lst->type != PVAL_LIST){
            return pval_error(lst, "value-error");
        }
        return lst->value.val_list->head->data;
    } else if (strcmp(s->value.val_symbol, "tail") == 0){
        pval *lst = _pval_evaluate(node->data);
        if(lst->type != PVAL_LIST){
            return pval_error(lst, "value-error");
        }
        linked_node *curr = lst->value.val_list->head->next;
        if(!curr){
            return lst->value.val_list->head->data;
        }
        pval *out = pval_empty_list();
        
        while(curr){
            append_node(out->value.val_list, create_node(curr->data));
            curr = curr->next;
        }
        return out;
    }


    else {
        return pval_error(NULL, "invalid-special-symbol?");
    }
}

pval *process_func(pval *func, pval *args){
    op_func funcptr = func->value.val_function;
    return funcptr(args);
}


pval *eval_symbol(pval *sym){
    binding *b = hash_find(env_global->table, sym->value.val_symbol);
    if(!b) return pval_error(sym,"unbound-var");
    return pval_retain(b->cell->value);
}

pval *eval_list(pval *list) {
    linked_node *node = list->value.val_list->head;
    pval *out;

    if (!node) {
        return pval_retain(list);
    }

    pval *head = node->data;

    if (head->type == PVAL_SYMBOL && is_special_symbol(head)) {
        out = eval_special(head, node->next);  
        return out;
    }   

    pval *func = _pval_evaluate(head);
    if (func == NULL) {
        return NULL;
    }
    if (func->type == PVAL_ERROR) {
        return func;  
    }
    if (func->type == PVAL_LIST){
        return NULL;
    }
    if (func->type != PVAL_FUNCTION) {
        pval_release(func);
        return pval_error(NULL, "inapplicable-head");
    }

    pval *args_list = pval_empty_list();
    linked_node *curr = node->next;

    while (curr) {
        pval *arg_val = _pval_evaluate(curr->data);
        if (arg_val == NULL) {
            func = pval_release(func);
            args_list = pval_release(args_list);
            return NULL;
        }
        if (arg_val->type == PVAL_ERROR) {
            func = pval_release(func);
            args_list = pval_release(args_list);
            return arg_val; 
        }
        append_node(args_list->value.val_list, create_node(arg_val));
        arg_val = pval_release(arg_val);  
        curr = curr->next;
    }

    pval *result = process_func(func, args_list);
    func = pval_release(func);
    args_list = pval_release(args_list);

    return result;
}

pval *_pval_evaluate(pval *pv) {
    if (pv == NULL)
        return NULL;
    switch (pv->type) {
        case PVAL_NUMBER:
            return pval_retain(pv);
        case PVAL_BOOL:
            return pval_retain(pv);
        case PVAL_FUNCTION:
            return pval_retain(pv);
        case PVAL_ERROR:
            return pval_retain(pv);
        case PVAL_SYMBOL:
            return eval_symbol(pv);
        case PVAL_LIST:
            return eval_list(pv);
        default:
            break;
    }

    // Should never reach here
    return pval_error(pv, "unknown pval type");
}

