#include "console_commands.h"
#include "commands/cmd_alarm.h"
#include "commands/cmd_thermostat.h"
#include "commands/cmd_temp.h"
#include "commands/cmd_mode.h"
#include "commands/cmd_relay.h"
#include "commands/cmd_status.h"
#include "commands/cmd_logger.h"
#include "commands/cmd_outside.h"
#include "commands/cmd_setpoint.h"
#include "commands/cmd_hyst.h"
#include "commands/cmd_clock.h"
#include "commands/cmd_tick.h"
#include "commands/cmd_program.h"
#include "commands/cmd_history.h"
#include "commands/cmd_thermal.h"
#include "commands/cmd_runtime.h"
#include "commands/cmd_task.h"
#include "commands/cmd_event.h"
#include "commands/cmd_storage.h"
#include "commands/cmd_version.h"
#include "commands/cmd_config.h"
#include "commands/cmd_clear.h"
#include "commands/cmd_quit.h"

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
        {"alarm",
         cmd_alarms,
         "Display active alarms"},

        {"clock",
         cmd_clock,
         "Set / Display clock"},

        {"cls",
         cmd_clear,
         "Clear screen"},

        {"config",
         cmd_config,
         "Display configuration"},

        {"event",
         cmd_event,
         "Display event counters"},

        {"help",
         cmd_help,
         "Display available commands"},

        {"history",
         cmd_history,
         "Display / Clear history"},

        {"hyst",
         cmd_hyst,
         "Set thermostat hysteresis"},

        {"logger",
         cmd_logger,
         "Display logger configuration"},

        {"mode",
         cmd_mode,
         "Set thermostat mode"},

        {"outside",
         cmd_outside,
         "Set outside temperature"},

        {"program",
         cmd_program,
         "Display schedule / Set program"},

        "quit",
        cmd_quit,
        "Exit simulator",

        {"relay",
         cmd_relay,
         "Control relay / Display status"},

        {"runtime",
         cmd_runtime,
         "Display runtime configuration"},

        {"setpoint",
         cmd_setpoint,
         "Set temperature setpoint"},

        {"status",
         cmd_status,
         "Display complete status"},

        {"storage",
         cmd_storage,
         "Load / Save runtime configuration"},

        {"task",
         cmd_task,
         "Display scheduler tasks"},

        {"temp",
         cmd_temp,
         "Set simulated temperature"},

        {"thermal",
         cmd_thermal,
         "Display / Set thermal model"},

        {"thermostat",
         cmd_thermostat,
         "Display thermostat status"},

        {"tick",
         cmd_tick,
         "Advance simulated time"},

        {"version",
         cmd_version,
         "Display version"},
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

static void console_help_print(const char *name)
{
    for (size_t i = 0; i < command_count; i++)
    {
        if (strcmp(commands[i].name, name) == 0)
        {
            printf("%-12s %s\n",
                   commands[i].name,
                   commands[i].help);
            return;
        }
    }
}

void console_commands_help(const char *args)
{
    (void)args;

    printf("\n");
    printf("System\n");
    printf("------------------------------\n");

    console_help_print("config");
    console_help_print("cls");
    console_help_print("help");
    console_help_print("logger");
    console_help_print("runtime");
    console_help_print("quit");
    console_help_print("status");
    console_help_print("storage");
    console_help_print("version");

    printf("\n");
    printf("Time\n");
    printf("------------------------------\n");

    console_help_print("clock");
    console_help_print("tick");

    printf("\n");
    printf("Thermostat\n");
    printf("------------------------------\n");

    console_help_print("mode");
    console_help_print("setpoint");
    console_help_print("hyst");
    console_help_print("program");
    console_help_print("temp");
    console_help_print("outside");
    console_help_print("relay");
    console_help_print("thermal");
    console_help_print("thermostat");
    console_help_print("alarms");

    printf("\n");
    printf("Diagnostics\n");
    printf("------------------------------\n");

    console_help_print("history");
    console_help_print("event");
    console_help_print("task");

    printf("\n");
}
