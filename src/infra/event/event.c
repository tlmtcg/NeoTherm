#include "event.h"

#include <stdio.h>
#include <string.h>

#include "console_utils.h"

/*==========================================================
 * Runtime
 *=========================================================*/

typedef struct
{
    event_t events[EVENT_QUEUE_SIZE];

    size_t head;
    size_t tail;
    size_t count;

} event_runtime_t;

static event_runtime_t s_runtime;

static uint32_t s_event_count[EVENT_COUNT];

static uint32_t s_event_total = 0;

/*==========================================================
 * Noms événements
 *=========================================================*/

static const char *s_event_names[EVENT_COUNT] =
    {
        [EVENT_NONE] =
            "NONE",

        [EVENT_CLIMATE_UPDATE] =
            "CLIMATE_UPDATE",

        [EVENT_RELAY_ON] =
            "RELAY_ON",

        [EVENT_RELAY_OFF] =
            "RELAY_OFF",

        [EVENT_MODE_CHANGED] =
            "MODE_CHANGED",

        [EVENT_SETPOINT_CHANGED] =
            "SETPOINT_CHANGED",

        [EVENT_TIMER_1S] =
            "TIMER_1S",

        [EVENT_QUIT] =
            "QUIT",

        [EVENT_SENSOR_DHT] =
            "SENSOR_DHT",

        [EVENT_SENSOR_SHT31] =
            "SENSOR_SHT31",

        [EVENT_SENSOR_ERROR_DHT] =
            "SENSOR_ERROR_DHT",

        [EVENT_SENSOR_ERROR_SHT31] =
            "SENSOR_ERROR_SHT31",

        [EVENT_MODE_CHANGE_REQUEST] =
            "MODE_CHANGE_REQUEST",

        [EVENT_MANUAL_SETPOINT_REQUEST] =
            "MANUAL_SETPOINT_REQUEST",

        [EVENT_WEATHER_UPDATE] =
            "WEATHER_UPDATE",

        [EVENT_NET_TIME_SYNCED] =
            "NET_TIME_SYNCED",

        [EVENT_RELAY_CHANGED] =
            "RELAY_CHANGED",

        [EVENT_HISTORY_SAVE] =
            "HISTORY_SAVE",

        [EVENT_STORAGE_SAVE] =
            "STORAGE_SAVE",

        [EVENT_ALARM_ACTIVE] =
            "ALARM_ACTIVE",

        [EVENT_ALARM_CLEAR] =
            "ALARM_CLEAR",
};

/*==========================================================
 * Initialisation
 *=========================================================*/

void event_init(void)
{
    memset(&s_runtime,
           0,
           sizeof(s_runtime));

    memset(s_event_count,
           0,
           sizeof(s_event_count));

    s_event_total = 0;
}

/*==========================================================
 * Etat queue
 *=========================================================*/

bool event_is_empty(void)
{
    return (s_runtime.count == 0);
}

bool event_is_full(void)
{
    return (s_runtime.count >= EVENT_QUEUE_SIZE);
}

void event_clear(void)
{
    s_runtime.head = 0;
    s_runtime.tail = 0;
    s_runtime.count = 0;
}

/*==========================================================
 * Post
 *=========================================================*/

bool event_post(
    const event_t *event)
{
    if (event == NULL)
    {
        return false;
    }

    if (event_is_full())
    {
        return false;
    }

    s_runtime.events[s_runtime.tail] = *event;

    s_runtime.tail++;

    if (s_runtime.tail >= EVENT_QUEUE_SIZE)
    {
        s_runtime.tail = 0;
    }

    if ((event->type >= 0) &&
        (event->type < EVENT_COUNT))
    {
        s_event_count[event->type]++;
        s_event_total++;
    }

    s_runtime.count++;

    return true;
}

/*==========================================================
 * Get
 *=========================================================*/

bool event_get(
    event_t *event)
{
    if (event == NULL)
    {
        return false;
    }

    if (event_is_empty())
    {
        return false;
    }

    *event =
        s_runtime.events[s_runtime.head];

    s_runtime.head++;

    if (s_runtime.head >= EVENT_QUEUE_SIZE)
    {
        s_runtime.head = 0;
    }

    s_runtime.count--;

    return true;
}

/*==========================================================
 * Nom événement
 *=========================================================*/

const char *event_name(
    event_type_t type)
{
    if ((type < 0) ||
        (type >= EVENT_COUNT))
    {
        return "UNKNOWN";
    }

    if (s_event_names[type] == NULL)
    {
        return "UNKNOWN";
    }

    return s_event_names[type];
}

/*==========================================================
 * Statistiques
 *=========================================================*/

uint32_t event_get_count(
    event_type_t type)
{
    if ((type < 0) ||
        (type >= EVENT_COUNT))
    {
        return 0;
    }

    return s_event_count[type];
}

uint32_t event_get_total_count(void)
{
    return s_event_total;
}

/*==========================================================
 * Debug
 *=========================================================*/

void event_dump(void)
{
    console_print_header(
        "Events");

    printf("%-30s %s\n",
           "Event",
           "Count");

    console_print_separator();

    for (uint32_t i = 0;
         i < EVENT_COUNT;
         i++)
    {
        uint32_t count =
            s_event_count[i];

        if (count == 0)
        {
            continue;
        }

        printf("%-30s %u\n",
               event_name(
                   (event_type_t)i),
               count);
    }

    printf("\nTotal events : %u\n",
           s_event_total);

    console_print_separator();
}
