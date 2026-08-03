#include "cmd_clock.h"
#include "console_utils.h"
#include "clock.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

bool cmd_clock(const char *args)
{
    if ((args == NULL) || (*args == '\0'))
    {
        printf("Usage : clock HH:MM:SS\n");
        return false;
    }

    if (strcmp(args, "status") == 0)
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

    clock_time_t t = {0};

    if (sscanf(args,
               "%u:%u:%u",
               &t.hour,
               &t.minute,
               &t.second) != 3)
    {
        printf("Invalid time format\n");
        printf("Usage : clock HH:MM:SS\n");
        return false;
    }

    clock_set_time(&t);

    printf("Time set to %02u:%02u:%02u\n",
           t.hour,
           t.minute,
           t.second);

    return true;
}
