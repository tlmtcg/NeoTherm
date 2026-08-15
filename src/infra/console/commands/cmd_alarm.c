#include "cmd_alarm.h"

#include "alarm.h"
#include "alarm_history.h"
#include "console_utils.h"

#include <stdio.h>
#include <string.h>

/*
 * Conversion texte -> type alarme
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
 * Liste des alarmes disponibles
 */
static void alarm_list(void)
{
    console_print_header(
        "Available alarms");

    for (alarm_type_t i = ALARM_TEMP_HIGH;
         i < ALARM_COUNT;
         i++)
    {
        printf("%-25s %s\n",
               alarm_get_command_name(i),
               alarm_get_name(i));
    }

    console_print_separator();
}

/*
 * Effacement complet des alarmes actives
 */
static void alarm_clear_all(void)
{
    for (alarm_type_t i = ALARM_TEMP_HIGH;
         i < ALARM_COUNT;
         i++)
    {
        alarm_clear(i);
    }
}

/*
 * Aide commande
 */
static void alarm_usage(void)
{
    printf("\nUsage:\n");

    printf("  alarm status\n");
    printf("  alarm list\n");

    printf("  alarm history\n");
    printf("  alarm history clear\n");
    printf("  alarm history save\n");
    printf("  alarm history load\n");

    printf("  alarm storage\n");

    printf("  alarm set <type> <value>\n");
    printf("  alarm ack <type>\n");
    printf("  alarm clear <type>\n");
    printf("  alarm clear all\n");
}

    // alarm set TEMP_HIGH 30.5


/*
 * Commande principale
 */
bool cmd_alarms(
    const char *args)
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
     * alarm storage
     */
    if (strcmp(command, "storage") == 0)
    {
        printf("Alarm history entries : %u\n",
               alarm_history_count());

        printf("Alamr history dirty  : %s\n",
               alarm_history_is_dirty()
                   ? "YES"
                   : "NO");

        return true;
    }

    /*
     * alarm history
     *
     * alarm history
     * alarm history clear
     * alarm history save
     * alarm history load
     */
    if (strcmp(command, "history") == 0)
    {

        /*
         * clear
         */
        if (strcmp(name, "clear") == 0)
        {
            if (alarm_history_clear())
            {
                printf(
                    "Alarm history cleared.\n");

                return true;
            }

            return false;
        }

        /*
         * save
         */
        if (strcmp(name, "save") == 0)
        {
            if (alarm_history_save())
            {
                printf(
                    "Alarm history saved.\n");

                return true;
            }

            printf(
                "Alarm history save failed.\n");

            return false;
        }

        /*
         * load
         */
        if (strcmp(name, "load") == 0)
        {
            if (alarm_history_load())
            {
                printf(
                    "Alarm history loaded.\n");

                return true;
            }

            printf(
                "Alarm history load failed.\n");

            return false;
        }

        /*
         * affichage par défaut
         */
        if (name[0] == '\0')
        {
            alarm_history_dump();
            return true;
        }

        printf(
            "Unknown history command : %s\n",
            name);

        return false;
    }

    /*
     * alarm list
     */
    if (strcmp(command, "list") == 0)
    {
        alarm_list();
        return true;
    }

    /*
     * alarm set TYPE VALUE
     */
    if (strcmp(command, "set") == 0)
    {
        if (count < 3)
        {
            printf(
                "Usage: alarm set <type> <value>\n");

            return false;
        }

        alarm_type_t type =
            alarm_from_string(name);

        if (type == ALARM_NONE)
        {
            printf(
                "Unknown alarm : %s\n",
                name);

            return false;
        }

        if (alarm_set(type, value))
        {
            printf(
                "Alarm activated : %s (%.2f)\n",
                alarm_get_name(type),
                value);

            return true;
        }

        return false;
    }

    /*
     * alarm ack TYPE
     */
    if (strcmp(command, "ack") == 0)
    {
        if (count < 2)
        {
            printf(
                "Usage: alarm ack <type>\n");

            return false;
        }

        alarm_type_t type =
            alarm_from_string(name);

        if (type == ALARM_NONE)
        {
            printf(
                "Unknown alarm : %s\n",
                name);

            return false;
        }

        if (alarm_ack(type))
        {
            printf(
                "Alarm acknowledged : %s\n",
                alarm_get_name(type));

            return true;
        }

        return false;
    }

    /*
     * alarm clear all
     */
    if (strcmp(command, "clear") == 0 &&
        strcmp(name, "all") == 0)
    {
        alarm_clear_all();

        printf(
            "All alarms cleared.\n");

        return true;
    }

    /*
     * alarm clear TYPE
     */
    if (strcmp(command, "clear") == 0)
    {
        if (count < 2)
        {
            printf(
                "Usage: alarm clear <type>\n");

            return false;
        }

        alarm_type_t type =
            alarm_from_string(name);

        if (type == ALARM_NONE)
        {
            printf(
                "Unknown alarm : %s\n",
                name);

            return false;
        }

        if (alarm_clear(type))
        {
            printf(
                "Alarm cleared : %s\n",
                alarm_get_name(type));

            return true;
        }

        return false;
    }

    /*
     * Commande inconnue
     */
    printf(
        "Unknown alarm command : %s\n",
        command);

    alarm_usage();

    return false;
}
