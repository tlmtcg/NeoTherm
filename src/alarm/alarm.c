#include "alarm.h"
#include "console_utils.h"

#include <string.h>
#include <stdio.h>

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
    if (type <= ALARM_NONE ||
        type >= ALARM_COUNT)
    {
        return false;
    }

    alarm_table[type].state =
        ALARM_STATE_ACTIVE;

    alarm_table[type].value =
        value;

    return true;
}

bool alarm_clear(
    alarm_type_t type)
{
    if (type <= ALARM_NONE ||
        type >= ALARM_COUNT)
    {
        return false;
    }

    alarm_table[type].state =
        ALARM_STATE_CLEAR;

    return true;
}

bool alarm_is_active(
    alarm_type_t type)
{
    if (type <= ALARM_NONE ||
        type >= ALARM_COUNT)
    {
        return false;
    }

    return alarm_table[type].state ==
           ALARM_STATE_ACTIVE;
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
    [ALARM_NONE]                = "None",

    [ALARM_TEMP_HIGH]           = "Temperature High",
    [ALARM_TEMP_LOW]            = "Temperature Low",

    [ALARM_SENSOR_DHT_ERROR]    = "DHT Error",
    [ALARM_SENSOR_SHT31_ERROR]  = "SHT31 Error",

    [ALARM_SENSOR_DHT]          = "DHT Sensor",
    [ALARM_SENSOR_SHT31]        = "SHT31 Sensor",
    [ALARM_SENSOR_INVALID]      = "Invalid Sensor",

    [ALARM_RELAY_ERROR]         = "Relay Error",
    [ALARM_HEATING_TIMEOUT]     = "Heating Timeout",

    [ALARM_CONFIG_ERROR]        = "Configuration Error",

    [ALARM_STORAGE]             = "Storage Error",
    [ALARM_HISTORY]             = "History Error",

    [ALARM_CONFIG]              = "Configuration",

    [ALARM_I2C]                 = "I2C Bus"
};

const char *alarm_get_name(alarm_type_t type)
{
    if (type >= ALARM_COUNT)
    {
        return "Unknown";
    }

    return alarm_names[type];
}
