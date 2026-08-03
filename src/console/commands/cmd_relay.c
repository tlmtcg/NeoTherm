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

    if (strncmp(args, "on ", 2) == 0)
    {
        relay_set(true);

        console_print_string("Relay", "ON");

        return true;
    }

    if (strncmp(args, "off ", 3) == 0)
    {
        relay_set(false);

        console_print_string("Relay", "OFF");

        return true;
    }

    if (strncmp(args, "reset ", 8) == 0)
    {
        relay_test_reset();

        console_print_string("Relay", "resetted");

        return true;
    }

    if (strcmp(args, "status") == 0)
    {
        relay_status_t status;

        relay_get_status(&status);

        console_print_header("Relay status");

        console_print_string(
            "State",
            status.state ? "ON" : "OFF");

        console_print_string(
            "Can switch",
            status.can_switch ? "YES" : "NO");

        console_print_uint(
            "Switch count",
            status.switch_count);

        console_print_uint(
            "Last switch (s)",
            status.last_switch_time);

        console_print_uint(
            "Min delay (s)",
            status.min_switch_delay);

        console_print_uint(
            "Elapsed (s)",
            status.elapsed_delay);

        console_print_uint(
            "Remaining (s)",
            status.remaining_delay);

        return true;
    }

    printf("Usage:\n");
    printf("  relay status\n");
    printf("  relay delay <seconds>\n");
    printf("  relay on\n");
    printf("  relay off\n");
    printf("  relay reset\n");

    return false;
}
