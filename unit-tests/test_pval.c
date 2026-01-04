#include <stdio.h>
#include <string.h>

#include "pval.h"
#include "test_framework.h"

// Example function under test
int test_correct_pval_type_to_string(void)
{
    pval test_pv;
    test_pv.type = PVAL_ERROR;

    char *converted_pval = pval_type_to_string(&test_pv);

    if (strcmp(converted_pval, "PVAL_ERROR") != 0)
    {
        printf("Expected 'PVAL_ERROR', got '%s'\n", converted_pval);
        return 1; // failure
    }
    return 0; // success
}
REGISTER_TEST(test_correct_pval_type_to_string);

int test_incorrect_pval_type_to_string(void)
{
    pval test_pv;
    test_pv.type = PVAL_NUMBER;

    char *converted_pval = pval_type_to_string(&test_pv);

    if (strcmp(converted_pval, "PVAL_NR") != 0)
    {
        printf("Expected 'PVAL_NUMBER', got '%s'\n", converted_pval);
        return 1; // failure
    }
    return 0; // success
}


REGISTER_TEST(test_incorrect_pval_type_to_string);
