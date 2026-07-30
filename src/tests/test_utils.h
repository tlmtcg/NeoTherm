#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

extern unsigned g_test_count;
extern unsigned g_test_passed;

#define TEST_BEGIN(name) \
    printf("%-20s", name);

#define TEST_PASS()       \
    do                    \
    {                     \
        printf("PASS\n"); \
        g_test_count++;   \
        g_test_passed++;  \
        return true;      \
    } while (0)

#define TEST_FAIL(msg)              \
    do                              \
    {                               \
        printf("FAIL (%s)\n", msg); \
        g_test_count++;             \
        return false;               \
    } while (0)

#define ASSERT_TRUE(expr)                 \
    do                                    \
    {                                     \
        if (!(expr))                      \
        {                                 \
            printf("FAIL : %s\n", #expr); \
            return false;                 \
        }                                 \
    } while (0)

#define ASSERT_FALSE(expr)                \
    do                                    \
    {                                     \
        if (expr)                         \
        {                                 \
            printf("FAIL : %s\n", #expr); \
            return false;                 \
        }                                 \
    } while (0)

#define ASSERT_INT_EQ(expected, actual) \
    if ((expected) != (actual))         \
    TEST_FAIL(#actual)

#define ASSERT_FLOAT_EQ(expected, actual)     \
    if (fabsf((expected) - (actual)) > 0.01f) \
    TEST_FAIL(#actual)

#define ASSERT_EQ_UINT32(expected, actual)              \
do                                                      \
{                                                       \
    if ((expected) != (actual))                         \
    {                                                   \
        printf("FAIL : expected %u got %u\n",           \
               (unsigned)(expected),                    \
               (unsigned)(actual));                     \
        return false;                                   \
    }                                                   \
} while (0)

#endif
