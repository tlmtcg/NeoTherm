#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

/*
 *==========================================================
 * Initialisation
 *==========================================================
 */

void console_init(void);

/*
 *==========================================================
 * Boucle console
 *==========================================================
 */

void console_update(void);

/*
 *==========================================================
 * Exécution d'une commande
 *==========================================================
 */

bool console_execute(const char *command);

#endif /* CONSOLE_H */
