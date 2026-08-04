#include "cmd_program.h"
#include "schedule.h"
#include "console_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool cmd_program(const char *args)
{
    if (args == NULL || *args == '\0')
    {
        printf("Usage:\n");
        printf("  program status\n");
        printf("  program dump\n");
        printf("  program set <day> <HH:MM> <temp>\n");
        return false;
    }

    if (strcmp(args, "status") == 0)
    {
        schedule_dump();
        return true;
    }

    if (strcmp(args, "dump") == 0)
    {
        schedule_dump();
        return true;
    }

    if (strncmp(args, "set ", 4) == 0)
    {
        char day_name[16];
        unsigned hour;
        unsigned minute;
        float setpoint;

        if (sscanf(args + 4,
                   "%15s %u:%u %f",
                   day_name,
                   &hour,
                   &minute,
                   &setpoint) != 4)
        {
            printf("Usage: program set <day> <HH:MM> <temp>\n");
            return false;
        }

        int day = schedule_day_from_name(day_name);

        if (day < 0)
        {
            printf("Unknown day : %s\n", day_name);
            return false;
        }

        if (!schedule_set_point((uint8_t)day,
                                (uint8_t)hour,
                                (uint8_t)minute,
                                setpoint))
        {
            printf("Unable to update schedule\n");
            return false;
        }
        else
        {
            printf("Schedule updated: %s %02u:%02u -> %.1f C\n",
                   day_name,
                   hour,
                   minute,
                   setpoint);
        }

        printf("Unknown program command\n");

        return false;
    }

    if (strncmp(args, "rem ", 4) == 0)
    {
        char day_name[16];
        unsigned hour;
        unsigned minute;

        if (sscanf(args + 3,
                   "%15s %u:%u",
                   day_name,
                   &hour,
                   &minute
                   ) != 3)
        {
            printf("Usage: program rem <day> <HH:MM>\n");
            return false;
        }

        int day = schedule_day_from_name(day_name);

        if (day < 0)
        {
            printf("Unknown day : %s\n", day_name);
            return false;
        }

        if (!schedule_remove_point((uint8_t)day,
                                   (uint8_t)hour,
                                   (uint8_t)minute))
        {
            printf("Unable to remove schedule\n");
            return false;
        }
        else
        {
            printf("Schedule remove: %s %02u:%02u\n",
                   day_name,
                   hour,
                   minute);
        }

        printf("Unknown program command\n");

        return false;
    }
    
    return false;
}