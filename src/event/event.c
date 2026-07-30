#include "event.h"

#include <string.h>

#define EVENT_QUEUE_SIZE    32

typedef struct
{
    event_t events[EVENT_QUEUE_SIZE];

    size_t head;
    size_t tail;
    size_t count;

} event_runtime_t;

static event_runtime_t s_runtime;

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
    switch(type)
    {
        case EVENT_NONE:
            return "NONE";

        case EVENT_CLIMATE_UPDATE:
            return "CLIMATE_UPDATE";

        case EVENT_TIMER_1S:
            return "TIMER_1S";

        case EVENT_QUIT:
            return "QUIT";

        default:
            return "UNKNOWN";
    }
}
