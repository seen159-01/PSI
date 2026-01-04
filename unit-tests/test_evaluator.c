#include <stdio.h>
#include <string.h>

#include "pval.h"
#include "SinglyLinkedList.h"
#include "test_framework.h"

// Example function under test
int test_evaluator_list(void) {
    pval *list = pval_empty_list();
    pval *inner_list = pval_empty_list();
    
    append_node(inner_list, create_node(pval_symbol("cons")));
    append_node(inner_list, create_node(pval_number(9)));
    append_node(inner_list, create_node(pval_empty_list()));

    append_node(list, create_node(pval_) )
    
    return 1;
     return 0;
}
REGISTER_TEST(test_correct_pval_type_to_string);

int test_incorrect_pval_type_to_string(void) {
    pval test_pv;
    test_pv.type = PVAL_NUMBER;

    char *converted_pval = pval_type_to_string(&test_pv);

    if (strcmp(converted_pval, "PVAL_NR") != 0) {
        printf("Expected 'PVAL_NUMBER', got '%s'\n", converted_pval);
        return 1;  // failure
    }
    return 0;  // success
}

REGISTER_TEST(test_incorrect_pval_type_to_string);
