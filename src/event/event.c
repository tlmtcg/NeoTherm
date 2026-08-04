#include "event.h"
#include "console_utils.h"
#include <stdio.h>
#include <string.h>

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

void event_init(void)
{
    memset(&s_runtime, 0, sizeof(s_runtime));
}

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

bool event_post(const event_t *event)
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

    if (event->type < EVENT_COUNT)
    {
        s_event_count[event->type]++;
         s_event_total++;
    }

    s_runtime.count++;

    return true;
}

bool event_get(event_t *event)
{
    if (event == NULL)
    {
        return false;
    }

    if (event_is_empty())
    {
        return false;
    }

    *event = s_runtime.events[s_runtime.head];

    s_runtime.head++;

    if (s_runtime.head >= EVENT_QUEUE_SIZE)
    {
        s_runtime.head = 0;
    }

    s_runtime.count--;

    return true;
}

const char *event_type_to_string(event_type_t type)
{
    switch (type)
    {
        case EVENT_NONE:
            return "NONE";

        case EVENT_CLIMATE_UPDATE:
            return "CLIMATE_UPDATE";

        case EVENT_TIMER_1S:
            return "TIMER_1S";

        case EVENT_QUIT:
            return "QUIT";

        case EVENT_SENSOR_DHT:
            return "SENSOR_DHT";

        case EVENT_SENSOR_SHT31:
            return "SENSOR_SHT31";

        case EVENT_SENSOR_ERROR_DHT:
            return "SENSOR_ERROR_DHT";

        case EVENT_SENSOR_ERROR_SHT31:
            return "SENSOR_ERROR_SHT31";

        case EVENT_MODE_CHANGE_REQUEST:
            return "MODE_CHANGE_REQUEST";

        case EVENT_MANUAL_SETPOINT_REQUEST:
            return "MANUAL_SETPOINT_REQUEST";

        case EVENT_WEATHER_UPDATE:
            return "WEATHER_UPDATE";

        case EVENT_NET_TIME_SYNCED:
            return "NET_TIME_SYNCED";

        case EVENT_RELAY_CHANGED:
            return "RELAY_CHANGED";

        case EVENT_HISTORY_SAVE:
            return "HISTORY_SAVE";

        case EVENT_STORAGE_SAVE:
            return "STORAGE_SAVE";

        default:
            return "UNKNOWN";
    }
}

uint32_t event_get_count(event_type_t type)
{
    if (type >= EVENT_COUNT)
    {
        return 0;
    }

    return s_event_count[type];
}

void event_dump(void)
{
    console_print_header("Events");

    printf("%-30s %s\n",
           "Event",
           "Count");

    console_print_separator();

    uint32_t total = 0;

    for (uint32_t i = 0; i < EVENT_COUNT; i++)
    {
        uint32_t count = s_event_count[i];

        if (count == 0)
        {
            continue;
        }

        printf("%-30s %u\n",
               event_name((event_type_t)i),
               count);

        total += count;
    }

    printf("\nTotal events : %u\n", total);

    console_print_separator();
}

const char *event_name(event_type_t type)
{
    switch (type)
    {
        case EVENT_NONE:
            return "NONE";

        case EVENT_CLIMATE_UPDATE:
            return "CLIMATE_UPDATE";

        case EVENT_RELAY_ON:
            return "RELAY_ON";

        case EVENT_RELAY_OFF:
            return "RELAY_OFF";

        case EVENT_MODE_CHANGED:
            return "MODE_CHANGED";

        case EVENT_SETPOINT_CHANGED:
            return "SETPOINT_CHANGED";

        case EVENT_TIMER_1S:
            return "TIMER_1S";

        case EVENT_QUIT:
            return "QUIT";

        case EVENT_SENSOR_DHT:
            return "SENSOR_DHT";

        case EVENT_SENSOR_SHT31:
            return "SENSOR_SHT31";

        case EVENT_SENSOR_ERROR_DHT:
            return "SENSOR_ERROR_DHT";

        case EVENT_SENSOR_ERROR_SHT31:
            return "SENSOR_ERROR_SHT31";

        case EVENT_MODE_CHANGE_REQUEST:
            return "MODE_CHANGE_REQUEST";

        case EVENT_MANUAL_SETPOINT_REQUEST:
            return "MANUAL_SETPOINT_REQUEST";

        case EVENT_WEATHER_UPDATE:
            return "WEATHER_UPDATE";

        case EVENT_NET_TIME_SYNCED:
            return "NET_TIME_SYNCED";

        case EVENT_RELAY_CHANGED:
            return "RELAY_CHANGED";

        case EVENT_HISTORY_SAVE:
            return "HISTORY_SAVE";

        case EVENT_STORAGE_SAVE:
            return "STORAGE_SAVE";

        default:
            return "UNKNOWN";
    }
}

uint32_t event_get_total_count(void)
{
    return s_event_total;
}

