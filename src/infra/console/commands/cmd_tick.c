#include "cmd_tick.h"

#include "clock.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

bool cmd_tick(const char *args)
{
    if ((args == NULL) || (*args == '\0'))
    {
        clock_add_second();

        printf("Time advanced by 1 second\n");

        return true;
    }

    uint32_t seconds = (uint32_t)atoi(args);

    clock_add_seconds(seconds);

    printf("Time advanced by %u second%s\n",
           seconds,
           (seconds > 1) ? "s" : "");

    return true;
}
