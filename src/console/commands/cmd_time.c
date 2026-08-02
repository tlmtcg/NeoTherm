#include "cmd_time.h"

#include "clock.h"
#include "console_utils.h"

bool cmd_time(const char *args)
{
    clock_time_t now;

    clock_get_time(&now);

    console_print_header("Current Time");

    console_print_datetime(
        "Date/Time",
        &now);

    console_print_separator();

    return true;
}
