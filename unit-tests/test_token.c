#include <stdio.h>
#include <string.h>

#include "test_framework.h"
#include "token.h"

// int test_null_term_helper_basic(void)
// {
//     char buffer[6] = "abcde";                     // 'a','b','c','d','e','\0'
//     null_term_helper(buffer, sizeof(buffer) - 1); // len = 6

//     // Expected: buffer[5 - 1] = '\0', so buffer = "abcd"
//     if (strcmp(buffer, "abcd") != 0)
//     {
//         printf("Expected 'abcd', got '%s'\n", buffer);
//         return 1;
//     }

//     return 0;
// }

// REGISTER_TEST(test_null_term_helper_basic);