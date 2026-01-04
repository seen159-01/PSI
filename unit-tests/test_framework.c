#include "test_framework.h"
#include <stdlib.h>

#define MAX_TESTS 256

static TestCase tests[MAX_TESTS];
static int test_count = 0;

void add_test(const char *name, TestFunc func)
{
    if (test_count < MAX_TESTS)
    {
        tests[test_count].name = name;
        tests[test_count].func = func;
        test_count++;
    }
    else
    {
        fprintf(stderr, "Too many tests registered!\n");
        exit(1);
    }
}

int run_all_tests(void)
{
    int passed = 0;

    printf("==== Running %d test(s) ====\n", test_count);

    for (int i = 0; i < test_count; i++)
    {
        int result = tests[i].func();
        if (result == 0)
        {
            printf("✅ %s passed\n\n", tests[i].name);
            passed++;
        }
        else
        {
            printf("❌ %s failed (code=%d)\n\n", tests[i].name, result);
        }
    }

    printf("==== %d/%d tests passed ====\n", passed, test_count);

    return passed == test_count ? 0 : 1;
}
