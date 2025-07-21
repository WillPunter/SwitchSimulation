/*  demo.c

    An example suite of unit tests for making sure the unit test frameworok is
    working. */

#include "test.h"

/*  Factorial function to test unit testing. */
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }

    return n * factorial(n - 1);
};

/*  Unit tests. */
DEFINE_TEST(test_factorial_0)
    ASSERT_EQ(1, factorial(0))
END_TEST

DEFINE_TEST(test_factorial_1)
    ASSERT_TRUE(factorial(1) == 1)
END_TEST

DEFINE_TEST(test_factorial_2)
    ASSERT_FALSE(factorial(1) != 1)
END_TEST

REGISTER_TESTS(
    test_factorial_0,
    test_factorial_1,
    test_factorial_2
)