#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "pval.h"
#include "SinglyLinkedList.h"
#include "operations.h"

// these functions ACTUALLY CREATE THE PVAL

pval *pval_number(int64_t n)
{
    pval *num = malloc(sizeof(*num));
    num->type = PVAL_NUMBER;
    num->value.val_number = n;
    return num;
}

pval *pval_bool(bool b)
{
    pval *boolean = malloc(sizeof(*boolean));
    if (!boolean)
        return NULL;
    boolean->type = PVAL_BOOL;
    boolean->value.val_bool = b;
    return boolean;
}


/*
I'm still unsure if it makes sense to check if a symbol is a valid operation here
*/
pval *pval_symbol(char *symbol_buf)
{
    pval *symbol = malloc(sizeof(*symbol));
    symbol->type = PVAL_SYMBOL;
    char *temp = strdup(symbol_buf);

    int symbol_idx = symbol_check(temp);
    if (symbol_idx != -1)
    {
        symbol->value.val_symbol = temp;
        return symbol;
    }
    else
    {
        symbol->value.val_symbol = temp;
        return pval_error(symbol, "invalid operation");
    }
}

/*returns a pval of type error that contains the offending pval.

1. creates a pval that is a (container). This is of type PVAL_ERROR
2. container WILL contain a struct of type 'error'
3. 'error' has pointers to erroring pval (type Symbol?) and error message to be print.

*/

pval *pval_error(pval *x, char* msg)
{
    pval *container = malloc(sizeof(*container));
    if (!container)
        return NULL;

    error *error = malloc(sizeof(*error));

    container->type = PVAL_ERROR;
    error->message = malloc(strlen(msg) + 1);
    error->pval_error = x;
    strcpy(error->message, msg);
    
    container->value.val_error = error;
    
    return container;
}


pval *pval_empty_list()
{
    pval *list = malloc(sizeof(*list));
    list->type = PVAL_LIST;
    list->value.val_list = create_list();
    return list;
}
void pval_add(pval *list, pval *elem){
    // TODO: do stuff
    (void)list;
    (void)elem;
}

/*
This function takes a CONTAINER of type PVAL_ERROR. the encapsulated pval within error struct 
will never be passed. 
*/
void pval_print_error(pval *pv){
    char *msg = pv->value.val_error->message;
    pval *offending_pval = pv->value.val_error->pval_error;

    switch (offending_pval->type){
        case PVAL_SYMBOL:
            char *sym = offending_pval->value.val_symbol;
            printf("$error{%s: %s}\n", msg, sym);
            break;
        case PVAL_NUMBER:
            printf("$error{%s: %ld}\n", msg, offending_pval->value.val_number);
            break;
        case PVAL_BOOL:
            if(pv->value.val_bool == true){
                printf("{%s: %s}\n", msg, "#t");
            }
            else{
                printf("{%s: %s}\n", msg, "#f");
            }
            break;
        case PVAL_FUNCTION:
            printf("$error{%s}\n", msg);
            break;
    }
}

void pval_print(pval *v)
{
    switch (v->type)
    {
    case PVAL_NUMBER:
        printf("%ld\n", v->value.val_number);
        break;
    case PVAL_BOOL:
        printf(v->value.val_bool ? "#t\n" : "#f\n");
        break;
    case PVAL_SYMBOL:
        printf("%s\n", v->value.val_symbol);
        break;
    case PVAL_LIST:
        // print list logic...
        break;
    case PVAL_ERROR:
        pval_print_error(v);
        break;
    }
}

pval *pval_copy(pval *pv){
    (void)pv;
}

void pval_delete(linked_node *node, pval *pv)
{
    if (pv->type == PVAL_SYMBOL)
    {
        free(pv->value.val_symbol);
    }
    else if (pv->type == PVAL_LIST)
    {
        free_list(pv->value.val_list);
    }
    node->data = NULL;
}



/*
1. take list of pvals
2. create backing list of just operable elements
3. if function is found, set that as current operation for current pval
4. 
*/


pval *evaluate_pvals(pval *pval_list, int depth)
{
    if(pval_list->type != PVAL_LIST){
        return pval_list;
    }
    linked_node *curr = pval_list->value.val_list->head;
    pval *result = malloc(sizeof(*result));
    pval *pv_error = malloc(sizeof(*pv_error));

    pval *data = NULL;
    op_func func = NULL;
    bool error = false;
    pval *pv_f_error = NULL;

    pval *out_pval = malloc(sizeof(*out_pval));
    out_pval->type = PVAL_LIST;
    out_pval->value.val_list = create_list();
    SinglyLinkedList *back_list = out_pval->value.val_list;
    

    while (curr != NULL)
    {
        data = curr->data;

        if(error){
            break;
        }
        if (data->type == PVAL_LIST)
        {
            depth++;
            data = evaluate_pvals(data, depth);
            
            if(data->type == PVAL_ERROR){
                return data;
            }
            append_node(back_list, create_node(data));
            depth--;
        }
        else if (data->type == PVAL_FUNCTION)
        {   
            func = data->value.val_function;

        }
        else if(data->type == PVAL_ERROR)
        {
            free(back_list);
            return data;
            // append_node(back_list, create_node(data));
        }
        
        else if (data->type == PVAL_NUMBER)
        {
            append_node(back_list, create_node(data));
        }
        else if(data->type == PVAL_BOOL)
        {
            append_node(back_list, create_node(data));
        }
        curr = curr->next;
    }

    if (error)
    {   
        free_list(back_list);
        if(pv_f_error != NULL){
            return pv_f_error;
        }
        return pv_error;
    }

    // if (depth != 0)
    // {
    //     result = func(out_pval);
    //     free_list(back_list);
    //     // if(result->type == PVAL_ERROR){
    //     //     free(out_pval);
    //     //     free_list(back_list);
    //     //     return result;
    //     // }
        
    // }
    
    result = func(out_pval);
    if(result->type == PVAL_ERROR){
        pval *error = malloc(sizeof(*error));
        memcpy(error, result, sizeof(*error));
        error->value.val_error = malloc(sizeof(*error->value.val_error));
        error->value.val_error->pval_error = malloc(sizeof(*result->value.val_error->pval_error));
        memcpy(error->value.val_error, result->value.val_error, sizeof(*result->value.val_error));
        memcpy(error->value.val_error->pval_error, result->value.val_error->pval_error, sizeof(*(result->value.val_error->pval_error)));
        return error;
    }
    free_list(back_list);
    free(out_pval);
    
    return result;
}

pval *bind_pvals(pval *pval_list)
{
    if(pval_list->type != PVAL_LIST){
        return pval_list;
    }
    linked_node *curr_pval = pval_list->value.val_list->head;
    while (curr_pval != NULL)
    {
        if (curr_pval->data->type == PVAL_LIST)
        {
            bind_pvals(curr_pval->data);
        }
        else if (curr_pval->data->type == PVAL_SYMBOL)
        {
            if (insert_op(curr_pval->data) != 0)
            {
                pval *error = (pval_error(curr_pval->data,"op-error: invalid operation"));
                curr_pval->data = error;
            }
        }
        // if(curr_pval == *(pval_list->tailp)){
        //     return pval_list;
        //     break;
        // }
        curr_pval = curr_pval->next;
    }
    return pval_list;
}

/*This feels pretty longwinded. I'm sure it'll become more bloated as I implement ALL the other
shit that the interpreter needs to do.

1. Create a list that will be passed along
2. Parse the incoming string
3. If number, just convert to pval
4. If char, copy until WS into a buffer
5. use pval_functions to create pvals for LL */

bool bound_check(char *input){
    char *index = input;
    int paren = 0;
    while(*index != '\0'){
        if (*index == '(' || *index == ')'){
            paren++;
        }
        index ++;
    }
    if (paren & 1){
        fprintf(stderr, "$error:{unpaired parenthesis}\n");
        return false;
    }

    return true;
}

pval *parse_pvals(char **input, pval *pval_list)
{   SinglyLinkedList *list = pval_list->value.val_list;
    bool paren = true;
    while (**input != '\0')
    {
        char input_peek = *(*input + 1);

        if (isspace(**input))
        {
            (*input)++;
            continue;
        }


        else if (**input == '(')
        {
            (*input)++;
            if(list == NULL){
                pval_list->value.val_list = create_list();
                list = pval_list->value.val_list;
                pval_list = parse_pvals(input, pval_list);
            }
            else{
                pval *sub_list = pval_empty_list();
                sub_list = parse_pvals(input, sub_list);
                append_node(list, create_node(sub_list));
            }

            (*input)++;

        }

        else if (**input == ')')
        {

            return pval_list;
        }
        

        //NUMBER PARSE
        else if (isdigit(**input) || (**input == '-' && isdigit(input_peek)))
        {
            
            char *out;
            int64_t num = strtol(*input, &out, 10);
            pval *t_num = pval_number(num);
            if(paren){
                if(list == NULL){
                    return t_num;
                }
                pval *error = pval_error(t_num, "op-error: invalid operator first");
                append_node(list, create_node(error));
            }else{
                append_node(list, create_node(t_num));
            }
            *input = out;
        }


        else
        {
            char symbol_buf[256];
            int i = 0;
            while (**input != '\0' && **input != ')' && **input != '(' && !isdigit(**input) && !isspace(**input))
            {   
                symbol_buf[i] = **input;
                i++;
                (*input)++;
            }
            symbol_buf[i] = '\0';

            if (strcmp(symbol_buf, "#t") == 0)
            {
                pval *t_bool = pval_bool(true);
                if (list == NULL)
                    return pval_bool(true);
                append_node(list, create_node(t_bool));
            }
            else if (strcmp(symbol_buf, "#f") == 0)
            {
                pval *t_bool = pval_bool(false);
                if (list == NULL)
                    return pval_bool(false);

                append_node(list, create_node(t_bool));
            }
            else
            {
                pval *t_symbol;
                if ((t_symbol = pval_symbol(symbol_buf)))
                {
                    if(list == NULL){
                        return t_symbol;
                    }
                    append_node(list, create_node(t_symbol));
                    paren = false;
                }
            }
        }
    }
   
    return pval_list;
}

int main()
{


    // executes main while loop. While 1 continue. When quit (0), break the loop



    // GETTING USER INPUT

    char buffer[4096];

    while (1)
    {
        printf("psi> ");
        // buffer = "(+ 3 (* 4 5))";
        fgets(buffer, sizeof buffer, stdin);
        char *input_ptr = buffer;
        if(!bound_check(input_ptr)){
            continue;
        }
        pval *pval_list = malloc(sizeof(*pval_list));
        pval_list->type = PVAL_LIST;
        pval_list->value.val_list = NULL;

        SinglyLinkedList *ptr = parse_pvals(&input_ptr, pval_list);
        ptr = bind_pvals(ptr);

        pval *out = evaluate_pvals(ptr, 0);

        pval_print(out);
    }


    // // HARD CODED BUFFER
    // char *buffer = malloc(1000);

    // printf("psi> ");
    // // buffer = "(/ 5 0)";
    // // buffer = "(+ 3 (+ 2 3))";
    // buffer = "(= #f #t)";
    // // buffer = "(= #f #f)";
    // // buffer = "(+ 3 (+ 2 1(4*)))";
    // char *input_ptr = buffer;
    
    // pval *pval_list = malloc(sizeof(*pval_list));
    // pval_list->type = PVAL_LIST;
    // pval_list->value.val_list = NULL;

    // pval *ptr = parse_pvals(&input_ptr, pval_list);
    // // free(buffer);
    // ptr = bind_pvals(ptr);
    // pval *pout = evaluate_pvals(ptr, 0);
    // pval_print(pout);

}