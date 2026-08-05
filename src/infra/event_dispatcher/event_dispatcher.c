#include "event_dispatcher.h"
#include "../services/alarm_service/alarm_service.h"

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

    switch (event->type)
    {
    case EVENT_NONE:
        break;

    /* System */
    case EVENT_TIMER_1S:

        LOG_DEBUG("DISPATCHER",
                  "EVENT_TIMER_1S");

        break;

    case EVENT_QUIT:

        LOG_DEBUG("DISPATCHER",
                  "EVENT_QUIT");

        break;

    /* Climate */
    case EVENT_CLIMATE_UPDATE:

        LOG_DEBUG("DISPATCHER",
                  "EVENT_CLIMATE_UPDATE");

        break;

    /* Alarm */
    case EVENT_ALARM_ACTIVE:

        LOG_INFO(
            "DISPATCHER",
            "Alarm activated : %s",
            alarm_get_name((alarm_type_t)event->data.value));
        alarm_service_on_alarm_active(event);

        break;

    case EVENT_ALARM_CLEAR:

        LOG_INFO(
            "DISPATCHER",
            "Alarm cleared");
            alarm_service_on_alarm_clear(event);

        break;

    case EVENT_ALARM_ACK:

        LOG_INFO(
            "DISPATCHER",
            "Alarm acknowledged");
            alarm_service_on_alarm_ack(event);

        break;

        /* Weather */
    case EVENT_WEATHER_UPDATE:

        LOG_DEBUG("DISPATCHER",
                  "EVENT_WEATHER_UPDATE");

        break;

    /* Storage */
    case EVENT_STORAGE_SAVE:

        LOG_DEBUG("DISPATCHER",
                  "EVENT_STORAGE_SAVE");

        break;

    /* History */
    case EVENT_HISTORY_SAVE:

        LOG_DEBUG("DISPATCHER",
                  "EVENT_HISTORY_SAVE");

        break;

    default:

        LOG_WARN("DISPATCHER",
                 "Unknown event : %d",
                 event->type);

        break;
    }
}
