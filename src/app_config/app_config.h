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
    uint32_t relay_default_switch_delay;

    /*
     * Scheduler
     */
    uint32_t climate_period;
    uint32_t thermostat_period;
    uint32_t history_save_period;
    uint32_t history_csv_period;

    uint32_t history_max_records;

    uint32_t schedule_max_points;
    float schedule_default_setpoint;

    float climate_initial_temperature;

    char runtime_file[64];
    char schedule_file[64];
    char history_file[64];
    char history_csv_file[64];

    bool debug_enabled;
    bool debug_dump_config;

    float thermal_outside_temperature;
    float thermal_heat_power;
    float thermal_loss_factor;
    float thermal_mass;

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

bool app_config_set_float(
    const char *section,
    const char *key,
    float value);

#endif /* APP_CONFIG_H */
