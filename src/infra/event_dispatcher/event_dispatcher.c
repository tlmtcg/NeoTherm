#include "event_dispatcher.h"

#include "logger.h"
#include <stdlib.h>

bool event_dispatcher_init(void)
{
    return true;
}

void event_dispatcher_dispatch(
    const event_t *event)
{
    if (event == NULL)
    {
        return;
    }

    switch(event->type)
    {
        case EVENT_NONE:
            break;

        case EVENT_CLIMATE_UPDATE:

            LOG_DEBUG("DISPATCHER",
                     "EVENT_CLIMATE_UPDATE");

            break;

        case EVENT_TIMER_1S:

            LOG_DEBUG("DISPATCHER",
                      "EVENT_TIMER_1S");

            break;

        case EVENT_QUIT:

            LOG_INFO("DISPATCHER",
                     "EVENT_QUIT");

            break;

        default:

            LOG_WARN("DISPATCHER",
                     "Unknown event : %d",
                     event->type);

            break;
    }
}
