#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

/*==========================================================
 * Configuration de l'application
 *=========================================================*/

typedef struct
{
    /*
     * Logger
     */
    char logger_level[16];

    /*
     * Relay
     */
    uint32_t relay_gpio;

    /*
     * Scheduler
     */
    uint32_t climate_period;
    uint32_t thermostat_period;

} app_config_t;

/*==========================================================
 * API
 *=========================================================*/

/*
 * Charge la configuration de l'application depuis app.ini
 */
bool app_config_init(const char *filename);

/*
 * Retourne la configuration chargée.
 */
const app_config_t *app_config_get(void);

#endif /* APP_CONFIG_H */
