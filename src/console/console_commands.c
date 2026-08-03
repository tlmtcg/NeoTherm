#include "console_commands.h"
#include "commands/cmd_alarm.h"
#include "commands/cmd_status.h"
#include "commands/cmd_temp.h"
#include "commands/cmd_mode.h"
#include "commands/cmd_relay.h"
#include "commands/cmd_logger.h"
#include "commands/cmd_outside.h"
#include "commands/cmd_setpoint.h"
#include "commands/cmd_hyst.h"
#include "commands/cmd_clock.h"

#include <stdio.h>
#include <string.h>

/*
 *==========================================================
 * Commandes
 *==========================================================
 */

static bool cmd_help(const char *args)
{
    (void)args;
    console_commands_help(args);
    return true;
}

/*
 *==========================================================
 * Table des commandes
 *==========================================================
 */

static const console_command_t commands[] =
    {
        {"help",
         cmd_help,
         "Display available commands"},

        {"alarms",
         cmd_alarms,
         "Display active alarms"},

        {"status",
         cmd_status,
         "Display thermostat status"},

        {"temp",
         cmd_temp,
         "Set simulated temperature"},

        {"mode",
         cmd_mode,
         "Set thermostat mode"},

        {"relay",
         cmd_relay,
         "Set relay or Get status"},

        {"logger",
         cmd_logger,
         "Logger configuration"},

        {"outside",
         cmd_outside,
         "Set outside temperature"},

        {"setpoint",
         cmd_setpoint,
         "Set setpoint temperature"},

        {"hyst",
         cmd_hyst,
         "Set Hystereris"},

        {"clock",
         cmd_clock,
         "Set clock"},

};

static const size_t command_count =
    sizeof(commands) /
    sizeof(commands[0]);

/*
 *==========================================================
 * Exécution
 *==========================================================
 */

bool console_commands_execute(
    const char *command)
{
    char name[32];

    sscanf(command,
           "%31s",
           name);

    const char *args =
        command + strlen(name);

    while (*args == ' ')
    {
        args++;
    }

    for (size_t i = 0;
         i < command_count;
         i++)
    {
        if (strcmp(name,
                   commands[i].name) == 0)
        {
            return commands[i].handler(args);
        }
    }

    printf("Unknown command : %s\n",
           command);

    return false;
}

/*
 *==========================================================
 * Help
 *==========================================================
 */

void console_commands_help(const char *args)
{
    (void)args;
    printf("\n");

    printf("Available commands\n");
    printf("------------------------------\n");

    for (size_t i = 0;
         i < command_count;
         i++)
    {
        printf("%-12s %s\n",
               commands[i].name,
               commands[i].help);
    }

    printf("\n");
}
