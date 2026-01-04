#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/SinglyLinkedList.h"
#include "../include/operations.h"
#include "../include/pval.h"
#include "../include/token.h"

pval *pval_retain(pval *pv){
    if(!pv) return NULL;
    pv->refcnt ++;
    return pv;
}

pval *pval_release(pval *pv){
    if(!pv) return NULL;
    if(pv->refcnt >= 1){
        pv->refcnt --;
    }
    if(pv->refcnt == 0){
        pv = pval_free(pv);
        return NULL;
    }
    return pv;
}

char *pval_type_to_string(pval *pv) {
    switch (pv->type) {
        case PVAL_NUMBER:
            return "PVAL_NUMBER";
        case PVAL_BOOL:
            return "PVAL_BOOL";
        case PVAL_SYMBOL:
            return "PVAL_SYMBOL";
        case PVAL_FUNCTION:
            return "PVAL_FUNCTION";
        case PVAL_LIST:
            return "PVAL_LIST";
        case PVAL_ERROR:
            return "PVAL_ERROR";
        default:
            return "UNKNOWN";
    }
}

pval *pval_number(int64_t n) {
    pval *num = malloc(sizeof(*num));
    num->type = PVAL_NUMBER;
    num->value.val_number = n;
    num->refcnt = 1;
    return num;
}

pval *pval_bool(bool b) {
    pval *boolean = malloc(sizeof(*boolean));
    if (!boolean)
        return NULL;
    boolean->type = PVAL_BOOL;
    boolean->value.val_bool = b;
    boolean->refcnt = 1;

    return boolean;
}

pval *pval_symbol(char *symbol_buf) {
    pval *symbol = malloc(sizeof(*symbol));
    symbol->type = PVAL_SYMBOL;
    char *temp = strdup(symbol_buf);
    symbol->value.val_symbol = temp;
    symbol->refcnt = 1;

    return symbol;
}

pval *pval_error(pval *pv, char *msg) {
    pval *container = malloc(sizeof(*container));
    if (!container)
        return NULL;

    error *error = malloc(sizeof(*error));

    container->type = PVAL_ERROR;
    error->message = malloc(strlen(msg) + 1);
    if (pv) {
        error->pval_error = pv;
        pval_retain(pv);
    } else {
        error->pval_error = NULL;
    }
    strcpy(error->message, msg);
    container->value.val_error = error;
    container->refcnt = 1;
    return container;
}

pval *pval_empty_list() {
    pval *list = malloc(sizeof(*list));
    list->type = PVAL_LIST;
    list->value.val_list = create_list();
    list->refcnt = 1;
    return list;
}

pval *pval_str(char *str) {
    pval *str_pval = malloc(sizeof(*str_pval));
    str_pval->type = PVAL_STRING;
    str_pval->value.val_string = malloc(strlen(str) + 1);
    strcpy(str_pval->value.val_string, str);
    str_pval->refcnt = 1;
    return str_pval;
}

pval *pval_copy(pval *src) {
    pval *dst = malloc(sizeof(*dst));
    dst->type = src->type;
    dst->refcnt = 1;

    switch (src->type) {
        case PVAL_SYMBOL:
            dst->value.val_symbol = strdup(src->value.val_symbol);
            break;
        case PVAL_STRING:
            dst->value.val_string = strdup(src->value.val_string);
            break;
        case PVAL_LIST:
            pval *tmp = pval_list_handler(src, pval_copy, true);
            dst->value.val_list = tmp->value.val_list;
            free(tmp);
            break;
        case PVAL_ERROR:
            error *dst_error = malloc(sizeof(*dst_error));
            dst_error->message = strdup(src->value.val_error->message);

            if (src->value.val_error->pval_error != NULL) {
                dst_error->pval_error = pval_copy(src->value.val_error->pval_error);
            } else {
                dst_error->pval_error = NULL;
            }
            dst->value.val_error = dst_error;
            break;

        default:
            dst->value = src->value;  // primitives (number, bool, function pointer)
            break;
    }

    return dst;
}

void pval_add(pval *list, pval *pv) {
    // TODO: do stuff
    (void)list;
    (void)pv;
}

pval *pval_free(pval *pv) {
    if (pv == NULL) {
        return NULL;
    }
    switch (pv->type) {
        case PVAL_SYMBOL:
            free(pv->value.val_symbol);
            pv->value.val_symbol = NULL;

            break;
        case PVAL_STRING:
            free(pv->value.val_string);
            pv->value.val_string = NULL;
            break;

        case PVAL_LIST:
            // pval *emp_list = pval_list_handler(pv, pval_free, false);
            // _free_list(emp_list->value.val_list);

            free_list(&(pv->value.val_list));

            // linked_node *curr_node = pv->value.val_list->head;
            
            // while (curr_node) {
            //     curr_node->data = pval_free(curr_node->data);
            //     linked_node *next = curr_node->next;
            //     free(curr_node);
            //     curr_node = next;
            // }
            
            break;

        case PVAL_ERROR:
            error *pv_err = pv->value.val_error;
            free(pv_err->message);
            if (pv_err->pval_error != NULL) {
                if (pv_err->pval_error->type == PVAL_SYMBOL) {
                    free(pv_err->pval_error->value.val_symbol);
                    free(pv_err->pval_error);
                } else if (pv_err->pval_error->type == PVAL_LIST) {
                    free_list(&pv_err->pval_error->value.val_list);
                }
                pv_err->message = NULL;
                pv_err->pval_error = NULL;
            }
            free(pv_err);
            pv_err = NULL;
            break;
        default:
            break;
    }
    free(pv);
    pv = NULL;
    return NULL;
}

/*
This function takes a CONTAINER of type PVAL_ERROR. the encapsulated pval within
error struct will never be passed.
*/

void pval_delete(pval *pv) {
    pval_free(pv);
}

pval *pval_print(pval *pv) {
    if (pv == NULL) {
        return NULL;
    }
    switch (pv->type) {
        case PVAL_NUMBER:
            printf("pval_num: %ld\n", pv->value.val_number);
            break;
        case PVAL_BOOL:
            printf("pval_bool: %s\n", pv->value.val_bool ? "#t" : "#f");
            break;
        case PVAL_STRING:
            printf("pval_string: %s\n", pv->value.val_string);
            break;
        case PVAL_SYMBOL:
            printf("pval_symbol: %s\n", pv->value.val_symbol);
            break;
        case PVAL_ERROR:
            pval_print_error(pv);
            break;
        case PVAL_LIST:
            pval_list_handler(pv, pval_print, false);
            break;
        default:
            printf("unknown_type: %s\n", pval_type_to_string(pv));
    }
    return pv;
}

pval *pval_list_handler(pval *pvals, pval *(*func)(pval *args), bool copy) {
    /*
    apply supplied function on whatever pvals in list;
    print pval*/
    if (pvals == NULL || pvals->type != PVAL_LIST) {
        return NULL;
    }
    pval *pval_list = copy ? pval_empty_list() : pvals;

    linked_node *curr_node = pvals->value.val_list->head;

    size_t idx = 0;
    while (idx < pvals->value.val_list->size) {
        if (curr_node->data->type == PVAL_LIST) {
            if (copy) {
                pval *nested_copy = pval_list_handler(curr_node->data, func, true);
                append_node(pval_list->value.val_list, create_node(nested_copy));
                nested_copy = NULL;
            } else {
                pval_list_handler(curr_node->data, func, false);
            }
        } else if (copy) {
            append_node(pval_list->value.val_list,
                        create_node(func(curr_node->data)));
        } else {
            func(curr_node->data);
        }
        curr_node = curr_node->next;
        idx++;
    }
    if (copy) {
        return pval_list;
    }
    return pvals;
}

// void pval_print_list_handler(pval *pvals) {
//     /*
//     if pvals is not list type:
//     print pval*/
//     if (pvals == NULL) {
//         return;
//     }
//     if (pvals->type != PVAL_LIST) {
//         return pval_print(pvals);
//     }
//     /*setup vars*/
//     linked_node *curr_node = pvals->value.val_list->head;

//     if (curr_node == NULL) {
//         printf("()");
//     } else {
//         while (curr_node) {
//             pval_print(curr_node->data);
//             curr_node = curr_node->next;
//         }
//     }
// }
void pval_print_error(pval *pv) {
    char *msg = pv->value.val_error->message;
    pval *offending_pval = pv->value.val_error->pval_error;
    if(pv->value.val_error->pval_error == NULL){
        printf("$error{%s}\n", msg);
        return;
    }
    switch (offending_pval->type) {
        case PVAL_SYMBOL:
            if (offending_pval->value.val_error->pval_error == NULL) {
                printf("{%s}", msg);
                break;
            }
            char *sym = offending_pval->value.val_symbol;
            printf("$error{%s: %s}\n", msg, sym);
            break;
        case PVAL_NUMBER:
            printf("$error{%s: %ld}\n", msg, offending_pval->value.val_number);
            break;
        case PVAL_BOOL:
            if (pv->value.val_bool == true) {
                printf("{%s: %s}\n", msg, "#t");
            } else {
                printf("{%s: %s}\n", msg, "#f");
            }
            break;
        case PVAL_FUNCTION:
            printf("$error{%s}\n", msg);
            break;
        case PVAL_LIST:
            printf("$error{%s}\n", msg);
            break;
        default:
            printf("$error{%s}\n", "msg");
    }
    msg = NULL;
    offending_pval = NULL;
}

// pval_eval *new_pev(pval *pv){
//     pval_eval *pev = malloc(sizeof(*pev));
//     pev->pv = pv;
//     if(pv->type == PVAL_LIST){
//         pev->curr_node = pv->value.val_list->head;
//     }
//     else{
//         pev->curr_node = NULL;
//     }
//     pev->error = NULL;
// }
