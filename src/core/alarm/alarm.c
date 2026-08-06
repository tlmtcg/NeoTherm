#include "alarm.h"
#include "console_utils.h"

#include <string.h>
#include <stdio.h>

#include "event.h"
#include "clock.h"
#include "alarm_history.h"

static alarm_t alarm_table[ALARM_COUNT];

void alarm_init(void)
{
    memset(
        alarm_table,
        0,
        sizeof(alarm_table));

    for (int i = 0; i < ALARM_COUNT; i++)
    {
        alarm_table[i].type = i;
        alarm_table[i].state = ALARM_STATE_CLEAR;
    }
}

bool alarm_set(
    alarm_type_t type,
    float value)
{
    if ((type <= ALARM_NONE) ||
        (type >= ALARM_COUNT))
    {
        return false;
    }

    alarm_t *alarm =
        &alarm_table[type];

    /*
     * Déjà active :
     * on met seulement à jour la valeur
     */
    if (alarm->state ==
        ALARM_STATE_ACTIVE)
    {
        alarm->value = value;

        return true;
    }

    alarm->type =
        type;

    alarm->state =
        ALARM_STATE_ACTIVE;

    alarm->value =
        value;

    alarm->timestamp =
        clock_seconds_today();

    /*
     * Historique
     */

    alarm_history_add(
        type,
        ALARM_STATE_ACTIVE,
        value);

    event_t event =
        {
            .type =
                EVENT_ALARM_ACTIVE,

            .data.value =
                (int32_t)type};

    event_post(&event);

    return true;
}

bool alarm_clear(
    alarm_type_t type)
{
    if ((type <= ALARM_NONE) ||
        (type >= ALARM_COUNT))
    {
        return false;
    }

    alarm_t *alarm =
        &alarm_table[type];

    if (alarm->state ==
        ALARM_STATE_CLEAR)
    {
        return true;
    }

    alarm->state =
        ALARM_STATE_CLEAR;

    alarm_history_add(
        type,
        ALARM_STATE_ACK,
        alarm_get_value(type));

    event_t event =
        {
            .type =
                EVENT_ALARM_CLEAR,

            .data.value =
                (int32_t)type};

    event_post(&event);

    return true;
}

bool alarm_is_active(
    alarm_type_t type)
{
    if ((type <= ALARM_NONE) ||
        (type >= ALARM_COUNT))
    {
        return false;
    }

    return alarm_table[type].state !=
           ALARM_STATE_CLEAR;
}

const alarm_t *alarm_get(
    alarm_type_t type)
{
    if (type <= ALARM_NONE ||
        type >= ALARM_COUNT)
    {
        return NULL;
    }

    return &alarm_table[type];
}

void alarm_dump(void)
{
    console_print_header("Active alarms");

    for (alarm_type_t type = ALARM_TEMP_HIGH;
         type < ALARM_COUNT;
         type++)
    {
        const alarm_t *alarm = alarm_get(type);

        console_print_alarm(
            alarm_get_name(type),
            alarm->state == ALARM_STATE_ACTIVE,
            alarm->value);
    }

    console_print_separator();
}

static const char *const alarm_names[ALARM_COUNT] =
    {
        [ALARM_NONE] = "None",

        [ALARM_TEMP_HIGH] = "Temperature High",
        [ALARM_TEMP_LOW] = "Temperature Low",

        [ALARM_SENSOR_DHT_ERROR] = "DHT Error",
        [ALARM_SENSOR_SHT31_ERROR] = "SHT31 Error",

        [ALARM_SENSOR_DHT] = "DHT Sensor",
        [ALARM_SENSOR_SHT31] = "SHT31 Sensor",
        [ALARM_SENSOR_INVALID] = "Invalid Sensor",

        [ALARM_RELAY_ERROR] = "Relay Error",
        [ALARM_HEATING_TIMEOUT] = "Heating Timeout",

        [ALARM_CONFIG_ERROR] = "Configuration Error",

        [ALARM_STORAGE] = "Storage Error",
        [ALARM_HISTORY] = "History Error",

        [ALARM_CONFIG] = "Configuration",

        [ALARM_I2C] = "I2C Bus",

        [ALARM_WEATHER_ERROR] = "WEATHER_ERROR",
};

const char *alarm_get_name(alarm_type_t type)
{
    if (type >= ALARM_COUNT)
    {
        return "Unknown";
    }

    return alarm_names[type];
}

static const char *const alarm_command_names[ALARM_COUNT] =
    {
        [ALARM_NONE] = "NONE",

        [ALARM_TEMP_HIGH] = "TEMP_HIGH",
        [ALARM_TEMP_LOW] = "TEMP_LOW",

        [ALARM_SENSOR_DHT_ERROR] = "DHT_ERROR",
        [ALARM_SENSOR_SHT31_ERROR] = "SHT31_ERROR",

        [ALARM_SENSOR_DHT] = "DHT_SENSOR",
        [ALARM_SENSOR_SHT31] = "SHT31_SENSOR",
        [ALARM_SENSOR_INVALID] = "SENSOR_INVALID",

        [ALARM_RELAY_ERROR] = "RELAY_ERROR",
        [ALARM_HEATING_TIMEOUT] = "HEATING_TIMEOUT",

        [ALARM_CONFIG_ERROR] = "CONFIG_ERROR",

        [ALARM_STORAGE] = "STORAGE_ERROR",
        [ALARM_HISTORY] = "HISTORY_ERROR",

        [ALARM_CONFIG] = "CONFIG",

        [ALARM_I2C] = "I2C_ERROR",

        [ALARM_WEATHER_ERROR] = "WEATHER_ERROR",
};

const char *alarm_get_command_name(alarm_type_t type)
{
    if (type >= ALARM_COUNT)
    {
        return "UNKNOWN";
    }

    return alarm_command_names[type];
}

bool alarm_ack(
    alarm_type_t type)
{
    if ((type <= ALARM_NONE) ||
        (type >= ALARM_COUNT))
    {
        return false;
    }

    alarm_t *alarm =
        &alarm_table[type];

    if (alarm->state !=
        ALARM_STATE_ACTIVE)
    {
        return false;
    }

    alarm->state =
        ALARM_STATE_ACK;

    alarm_history_add(
        type,
        ALARM_STATE_CLEAR,
        alarm_get_value(type));

    return true;
}

uint32_t alarm_get_active_count(void)
{
    uint32_t count = 0;

    for (int i = 0;
         i < ALARM_COUNT;
         i++)
    {
        if (alarm_table[i].state !=
            ALARM_STATE_CLEAR)
        {
            count++;
        }
    }

    return count;
}

const char *alarm_state_name(
    alarm_state_t state)
{
    switch (state)
    {
    case ALARM_STATE_CLEAR:
        return "CLEAR";

    case ALARM_STATE_ACTIVE:
        return "ACTIVE";

    case ALARM_STATE_ACK:
        return "ACK";

    default:
        return "?";
    }
}

float alarm_get_value(
    alarm_type_t type)
{
    if ((type <= ALARM_NONE) ||
        (type >= ALARM_COUNT))
    {
        return 0.0f;
    }

    return alarm_table[type].value;
}
