#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdbool.h>

/*
 * Initialise le serveur web.
 */
bool webserver_init(void);

/*
 * Démarre le serveur web.
 */
bool webserver_start(void);

/*
 * Arrête le serveur web.
 */
void webserver_stop(void);

/*
 * Indique si le serveur est actif.
 */
bool webserver_is_running(void);


void webserver_update(void);

#endif /* WEBSERVER_H */
