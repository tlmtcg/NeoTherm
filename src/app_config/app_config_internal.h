#ifndef APP_CONFIG_INTERNAL_H
#define APP_CONFIG_INTERNAL_H

#include "app_config.h"

/*
 * Configuration globale
 */

extern app_config_t s_config;

/*
 * Loaders internes
 */

void app_config_load_logger(void);

void app_config_load_relay(void);

void app_config_load_scheduler(void);

void app_config_load_history(void);

void app_config_load_files(void);

void app_config_load_schedule(void);

void app_config_load_climate(void);

void app_config_load_thermal_model(void);

void app_config_load_debug(void);


#endif