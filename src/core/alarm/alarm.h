#ifndef ALARM_H
#define ALARM_H

#include <stdbool.h>
#include <stdint.h>


typedef enum
{
    ALARM_NONE = 0,

    /* Température */
    ALARM_TEMP_HIGH,
    ALARM_TEMP_LOW,

    /* Capteurs */
    ALARM_SENSOR_DHT_ERROR,
    ALARM_SENSOR_SHT31_ERROR,

    ALARM_SENSOR_DHT,
    ALARM_SENSOR_SHT31,
    ALARM_SENSOR_INVALID,

    /* Chauffage */
    ALARM_RELAY_ERROR,
    ALARM_HEATING_TIMEOUT,

    /* Configuration */
    ALARM_CONFIG_ERROR,

    /* Stockage */
    ALARM_STORAGE,
    ALARM_HISTORY,

    /* Configuration */
    ALARM_CONFIG,

    /* Bus */
    ALARM_I2C,

    ALARM_COUNT

} alarm_type_t;


typedef enum
{
    ALARM_STATE_CLEAR = 0,
    ALARM_STATE_ACTIVE,
    ALARM_STATE_ACK

} alarm_state_t;



typedef struct
{
    alarm_type_t type;

    alarm_state_t state;

    float value;

    uint32_t timestamp;

} alarm_t;



void alarm_init(void);


bool alarm_set(
    alarm_type_t type,
    float value
);


bool alarm_clear(
    alarm_type_t type
);


const alarm_t *alarm_get(
    alarm_type_t type
);


bool alarm_is_active(
    alarm_type_t type
);

void alarm_dump(void);

const char *alarm_get_name(alarm_type_t type);

const char *alarm_get_command_name(alarm_type_t type);

#endif