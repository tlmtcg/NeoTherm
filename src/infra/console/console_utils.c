#include "console_utils.h"

#include <stdio.h>

/*
 *==========================================================
 * Header
 *==========================================================
 */

void console_print_header(
    const char *title)
{
    printf("\n");
    printf("==============================\n");
    printf("%s\n", title);
    printf("==============================\n");
}

/*
 *==========================================================
 * Separator
 *==========================================================
 */

void console_print_separator(void)
{
    printf("------------------------------\n");
}

/*
 *==========================================================
 * String
 *==========================================================
 */

void console_print_string(
    const char *label,
    const char *value)
{
    printf("%-20s %s\n",
           label,
           value);
}

/*
 *==========================================================
 * Float
 *==========================================================
 */

void console_print_float(
    const char *label,
    float value,
    const char *unit)
{
    printf("%-20s %.2f %s\n",
           label,
           value,
           unit);
}

/*
 *==========================================================
 * Unsigned integer
 *==========================================================
 */

void console_print_uint(
    const char *label,
    unsigned int value)
{
    printf("%-20s %u\n",
           label,
           value);
}

/*
 *==========================================================
 * Boolean
 *==========================================================
 */

void console_print_bool(
    const char *label,
    bool value)
{
    printf("%-20s %s\n",
           label,
           value ? "YES" : "NO");
}

void console_print_datetime(
    const char *label,
    const clock_time_t *time)
{
    printf("%-20s %04u-%02u-%02u %02u:%02u:%02u\n",
           label,
           time->year,
           time->month,
           time->day,
           time->hour,
           time->minute,
           time->second);
}

void console_print_alarm(
    const char *name,
    bool active,
    float value)
{
    printf("%-20s %-6s",
           name,
           active ? "ACTIVE" : "CLEAR");

    if (active)
    {
        printf(" %.2f", value);
    }

    printf("\n");
}
