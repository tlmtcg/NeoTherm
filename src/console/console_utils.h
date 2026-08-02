#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <stdbool.h>
#include "clock.h"

void console_print_header(
    const char *title);

void console_print_separator(void);

void console_print_string(
    const char *label,
    const char *value);

void console_print_float(
    const char *label,
    float value,
    const char *unit);

void console_print_uint(
    const char *label,
    unsigned int value);

void console_print_bool(
    const char *label,
    bool value);

void console_print_datetime(
    const char *label,
    const clock_time_t *time);

void console_print_alarm(
    const char *name,
    bool active,
    float value);

#endif /* CONSOLE_UTILS_H */
