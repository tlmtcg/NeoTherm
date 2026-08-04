#include "cmd_alarm.h"

#include "alarm.h"
#include "console_utils.h"

#include <stdio.h>
#include <string.h>

/*
 * Conversion texte -> type d'alarme
 *
 * Exemple :
 * TEMP_HIGH    -> ALARM_TEMP_HIGH
 * SHT31_ERROR  -> ALARM_SENSOR_SHT31_ERROR
 */
static alarm_type_t alarm_from_string(
    const char *name)
{
    if (name == NULL)
    {
        return ALARM_NONE;
    }

    for (alarm_type_t i = ALARM_TEMP_HIGH;
         i < ALARM_COUNT;
         i++)
    {
        if (strcmp(name,
                   alarm_get_command_name(i)) == 0)
        {
            return i;
        }
    }

    return ALARM_NONE;
}

/*
 * Commande :
 *
 * alarm status
 * alarm set TEMP_HIGH 25
 * alarm clear TEMP_HIGH
 *
 */
bool cmd_alarms(const char *args)
{
    if (args == NULL ||
        *args == '\0')
    {
        alarm_dump();
        return true;
    }

    char command[32] = {0};
    char name[64] = {0};
    float value = 0.0f;

    int count =
        sscanf(args,
               "%31s %63s %f",
               command,
               name,
               &value);

    /*
     * alarm status
     */
    if (strcmp(command, "status") == 0)
    {
        alarm_dump();
        return true;
    }

    /*
     * alarm set
     */
    if (strcmp(command, "set") == 0)
    {
        if (count < 3)
        {
            printf("Usage: alarm set <type> <value>\n");
            return false;
        }

        alarm_type_t type =
            alarm_from_string(name);

        if (type == ALARM_NONE)
        {
            printf("Unknown alarm : %s\n",
                   name);

            return false;
        }

        if (alarm_set(type, value))
        {
            printf("Alarm activated : %s (%.2f)\n",
                   alarm_get_name(type),
                   value);

            return true;
        }

        return false;
    }

    if ((strcmp(command, "clear") == 0) &&
        (strcmp(name, "all") == 0))
    {
        for (alarm_type_t type = ALARM_TEMP_HIGH;
             type < ALARM_COUNT;
             type++)
        {
            alarm_clear(type);
        }

        printf("All alarms cleared.\n");

        return true;
    }

    /*
     * alarm clear
     */
    if (strcmp(command, "clear") == 0)
    {
        if (count < 2)
        {
            printf("Usage: alarm clear <type>\n");
            return false;
        }

        alarm_type_t type =
            alarm_from_string(name);

        if (type == ALARM_NONE)
        {
            printf("Unknown alarm : %s\n",
                   name);

            return false;
        }

        if (alarm_clear(type))
        {
            printf("Alarm cleared : %s\n",
                   alarm_get_name(type));

            return true;
        }

        return false;
    }

    printf("Unknown alarm command : %s\n",
           command);

    printf("\nUsage:\n");
    printf("  alarm status\n");
    printf("  alarm set <type> <value>\n");
    printf("  alarm clear <type>\n");

    return false;
}
