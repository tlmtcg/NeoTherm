#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern unsigned g_test_count;
extern unsigned g_test_passed;

#define TEST_BEGIN(name) \
    printf("%-20s", name)

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
        printf("FAIL : %s\n", msg); \
        g_test_count++;             \
        return false;               \
    } while (0)

/*----------------------------------------------------------
 * Bool
 *---------------------------------------------------------*/

#define ASSERT_TRUE(expr)                 \
    do                                    \
    {                                     \
        if (!(expr))                      \
        {                                 \
            printf("FAIL : %s\n", #expr); \
            return false;                 \
        }                                 \
    } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

/*----------------------------------------------------------
 * Integer
 *---------------------------------------------------------*/

#define ASSERT_EQ_INT(expected, actual)           \
    do                                            \
    {                                             \
        if ((expected) != (actual))               \
        {                                         \
            printf("FAIL : expected %d got %d\n", \
                   (int)(expected),               \
                   (int)(actual));                \
            return false;                         \
        }                                         \
    } while (0)

#define ASSERT_EQ_BOOL(expected, actual)          \
    do                                            \
    {                                             \
        if ((bool)(expected) != (bool)(actual))   \
        {                                         \
            printf("FAIL : expected %s got %s\n", \
                   (expected) ? "true" : "false", \
                   (actual) ? "true" : "false");  \
            return false;                         \
        }                                         \
    } while (0)

/*----------------------------------------------------------
 * uint32_t
 *---------------------------------------------------------*/

#define ASSERT_EQ_UINT32(expected, actual)        \
    do                                            \
    {                                             \
        if ((expected) != (actual))               \
        {                                         \
            printf("FAIL : expected %u got %u\n", \
                   (unsigned)(expected),          \
                   (unsigned)(actual));           \
            return false;                         \
        }                                         \
    } while (0)

/*----------------------------------------------------------
 * Float
 *---------------------------------------------------------*/

#define ASSERT_EQ_FLOAT(expected, actual)             \
    do                                                \
    {                                                 \
        if (fabsf((expected) - (actual)) > 0.01f)     \
        {                                             \
            printf("FAIL : expected %.2f got %.2f\n", \
                   (float)(expected),                 \
                   (float)(actual));                  \
            return false;                             \
        }                                             \
    } while (0)

/*----------------------------------------------------------
 * Float comparisons
 *---------------------------------------------------------*/

#define ASSERT_GT_FLOAT(expected, actual)           \
    do                                              \
    {                                               \
        if (!((actual) > (expected)))               \
        {                                           \
            printf("FAIL : expected %.2f > %.2f\n", \
                   (float)(actual),                 \
                   (float)(expected));              \
            return false;                           \
        }                                           \
    } while (0)

#define ASSERT_LT_FLOAT(expected, actual)           \
    do                                              \
    {                                               \
        if (!((actual) < (expected)))               \
        {                                           \
            printf("FAIL : expected %.2f < %.2f\n", \
                   (float)(actual),                 \
                   (float)(expected));              \
            return false;                           \
        }                                           \
    } while (0)

#define ASSERT_GE_FLOAT(expected, actual)            \
    do                                               \
    {                                                \
        if (!((actual) >= (expected)))               \
        {                                            \
            printf("FAIL : expected %.2f >= %.2f\n", \
                   (float)(actual),                  \
                   (float)(expected));               \
            return false;                            \
        }                                            \
    } while (0)

#define ASSERT_LE_FLOAT(expected, actual)            \
    do                                               \
    {                                                \
        if (!((actual) <= (expected)))               \
        {                                            \
            printf("FAIL : expected %.2f <= %.2f\n", \
                   (float)(actual),                  \
                   (float)(expected));               \
            return false;                            \
        }                                            \
    } while (0)

/*----------------------------------------------------------
 * Pointer
 *---------------------------------------------------------*/

#define ASSERT_NULL(ptr) ASSERT_TRUE((ptr) == NULL)

#define ASSERT_NOT_NULL(ptr) ASSERT_TRUE((ptr) != NULL)

/*----------------------------------------------------------
 * Return value
 *---------------------------------------------------------*/

#define ASSERT_SUCCESS(expr) ASSERT_TRUE(expr)

#define ASSERT_FAILURE(expr) ASSERT_FALSE(expr)

/*----------------------------------------------------------
 * string
 *---------------------------------------------------------*/

#define ASSERT_EQ_STRING(expected, actual)                \
    do                                                    \
    {                                                     \
        if (strcmp((expected), (actual)) != 0)            \
        {                                                 \
            printf("FAIL : expected \"%s\" got \"%s\"\n", \
                   (expected),                            \
                   (actual));                             \
            return false;                                 \
        }                                                 \
    } while (0)

#endif
