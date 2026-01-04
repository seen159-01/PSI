#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

typedef int (*TestFunc)(void);

typedef struct
{
    const char *name;
    TestFunc func;
} TestCase;

#define REGISTER_TEST(fn)                                                                                              \
    static void __attribute__((constructor)) register_##fn(void) { add_test(#fn, fn); }

void add_test(const char *name, TestFunc func);
int run_all_tests(void);

#endif
