#ifndef HISTORY_H
#define HISTORY_H

#include <stdbool.h>
#include <stdint.h>

#include "thermostat.h"
#include "clock.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================
 * Configuration
 *=========================================================*/

#define HISTORY_SIZE 1024

/*==========================================================
 * Types
 *=========================================================*/

typedef struct
{
    uint32_t tick;

    clock_time_t timestamp;

    float inside_temperature;

    float outside_temperature;

    float setpoint;

    thermostat_mode_t mode;

    bool relay;

    bool heating;

} history_record_t;

/*==========================================================
 * Initialisation
 *=========================================================*/

bool history_init(void);

/*==========================================================
 * Gestion de l'historique
 *=========================================================*/

bool history_add(
    float inside_temperature,
    float outside_temperature,
    float setpoint,
    thermostat_mode_t mode,
    bool relay,
    bool heating);

void history_clear(void);

uint32_t history_count(void);

bool history_get(
    uint32_t index,
    history_record_t *record);

bool history_push(
    const history_record_t *record);

bool history_get_latest(
    history_record_t *record);

bool history_save(
    const char *filename);

bool history_load(
    const char *filename);

void history_task_callback(void);

void history_csv_task_callback(void);

void history_dump_last(uint32_t count);

#ifdef __cplusplus
}
#endif

#endif
