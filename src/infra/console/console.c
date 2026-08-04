#include "console.h"

#include <stdio.h>
#include <string.h>

#include "alarm.h"
#include "clock.h"
#include "logger.h"
#include "console_commands.h"

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif

/*
 *==========================================================
 * Initialisation
 *==========================================================
 */

void console_init(void)
{
    LOG_INFO(
        "CONSOLE",
        "Console initialized");
}

/*
 *==========================================================
 * Exécution d'une commande
 *==========================================================
 */

bool console_execute(
    const char *command)
{
    return console_commands_execute(command);
}

/*
 *==========================================================
 * Lecture clavier
 *==========================================================
 */

void console_update(void)
{
#ifdef _WIN32

    static char command[128];
    static uint32_t index = 0;

    while (_kbhit())
    {
        int c = _getch();

        if ((c == '\r') || (c == '\n'))
        {
            command[index] = '\0';

            printf("\n");

            if (index > 0)
            {
                console_commands_execute(command);
            }

            index = 0;
        }
        else if (index < sizeof(command) - 1)
        {
            command[index++] = (char)c;

            putchar(c);
        }
    }

#endif
}
