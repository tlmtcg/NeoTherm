#include "console_utils.h"
#include "relay.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bool cmd_relay(const char *args)
{
    if (strncmp(args, "delay ", 6) == 0)
    {
        uint32_t delay = (uint32_t)atoi(args + 6);

        relay_set_min_switch_delay(delay);

        console_print_uint("Min delay (s)",
                           relay_get_min_switch_delay());

        return true;
    }

    if (strcmp(args, "status") == 0)
    {
        console_print_header("Relay status");

        console_print_string(
            "State",
            relay_get() ? "ON" : "OFF");

        console_print_uint(
            "Switch count",
            relay_get_switch_count());

        console_print_uint(
            "Last switch (s)",
            relay_get_last_switch_time());

        console_print_uint(
            "Min delay (s)",
            relay_get_min_switch_delay());

        console_print_uint(
            "Elapsed (s)",
            relay_get_elapsed_delay());

        console_print_uint(
            "Remaining (s)",
            relay_get_remaining_delay());

        return true;
    }

    printf("Usage:\n");
    printf("  relay status\n");
    printf("  relay delay <seconds>\n");

    return false;
}
