/*  test.h

    A minimal C unit testing framework. */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define HEADER_STR "-----------------------------"

enum test_result {
    FAIL = 0,
    PASS
};

typedef enum test_result test_result_t;

/*  Define and end test macro. */
#define DEFINE_TEST(name) test_result_t name() { \
    printf("\nRunning test %s...\n", #name);

#define END_TEST printf("passed..."); \
        return PASS; \
    };

/*  Assertions. */
#define ASSERT_TRUE(expr) if (!expr) { \
    fprintf(stderr, "Test failed: ASSERT_TRUE failed on: \n\t expression %s \
        \n\t file: %s \n\t line: %d\n\n", #expr, __FILE__, __LINE__); \
    return FAIL; \
};

#define ASSERT_FALSE(expr) if (expr) { \
    fprintf(stderr, "Test failed: ASSERT_FALSE failed on: \n\t expression %s" \
        "\n\t file: %s \n\t line: %d\n\n", #expr, __FILE__, __LINE__); \
    return FAIL; \
};

#define ASSERT_EQ(expected, val) if (expected != val) { \
    fprintf(stderr, "Test failed: ASSERT_EQ failed: \n\t expected: %s \n\t" \
        "got: %s \n\t file: %s \n\t line: %d \n\n", #expected, #val, \
        __FILE__, __LINE__); \
    return FAIL; \
};

/*  Register tests macro. */
#define REGISTER_TESTS(args...) int main() { \
    printf(HEADER_STR "\n"); \
    printf("Starting unit tests: %s \n", __FILE__); \
    printf(HEADER_STR "\n\n"); \
    test_result_t (*tests[])(void) = {args, NULL}; \
    int index = 0; \
    int passed = 0; \
    int total = 0; \
    while (tests[index] != NULL) { \
        passed += tests[index](); \
        total ++; \
        index ++; \
    } \
    printf("\n\n" HEADER_STR "\n"); \
    printf("Tests finished for %s.\n", __FILE__); \
    printf("Passed %d of %d tests. \n", passed, total); \
    printf(HEADER_STR "\n\n"); \
};

#endif