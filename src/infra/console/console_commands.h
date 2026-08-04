#ifndef CONSOLE_COMMANDS_H
#define CONSOLE_COMMANDS_H

#include <stdbool.h>

/*
 *==========================================================
 * Type d'une commande console
 *==========================================================
 */

typedef bool (*console_command_handler_t)(const char *args);

typedef struct
{
    const char *name;
    console_command_handler_t handler;
    const char *help;

} console_command_t;

/*
 *==========================================================
 * Exécution
 *==========================================================
 */

bool console_commands_execute(
    const char *command);

/*
 *==========================================================
 * Affichage de l'aide
 *==========================================================
 */

void console_commands_help(const char *args);

#endif /* CONSOLE_COMMANDS_H */
