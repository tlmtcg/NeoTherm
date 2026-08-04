#include "cmd_version.h"

#include <stdio.h>

#include "console_utils.h"

bool cmd_version(const char *args)
{
    (void)args;

    console_print_header("NeoTherm Simulator");

    console_print_string(
        "Version",
        APP_VERSION);

    console_print_string(
        "Build date",
        __DATE__);

    console_print_string(
        "Build time",
        __TIME__);

#if defined(__GNUC__)
    printf("%-20s GCC %d.%d.%d\n",
           "Compiler",
           __GNUC__,
           __GNUC_MINOR__,
           __GNUC_PATCHLEVEL__);
#else
    console_print_string(
        "Compiler",
        "Unknown");
#endif

#if __STDC_VERSION__ >= 201710L
    console_print_string(
        "C Standard",
        "C17");
#elif __STDC_VERSION__ >= 201112L
    console_print_string(
        "C Standard",
        "C11");
#elif __STDC_VERSION__ >= 199901L
    console_print_string(
        "C Standard",
        "C99");
#else
    console_print_string(
        "C Standard",
        "C90");
#endif

#ifdef _WIN32
    console_print_string(
        "Platform",
        "Windows");
#else
    console_print_string(
        "Platform",
        "Linux");
#endif

    console_print_separator();

    return true;
}
