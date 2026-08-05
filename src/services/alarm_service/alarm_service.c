#include "alarm_service.h"

#include <stdio.h>

#include "logger.h"


static bool s_initialized = false;



bool alarm_service_init(void)
{
    alarm_init();

    s_initialized = true;


    LOG_INFO(
        "ALARM_SERVICE",
        "Alarm service initialized");


    return true;
}



uint32_t alarm_service_count_active(void)
{
    if (!s_initialized)
    {
        return 0;
    }


    uint32_t count = 0;


    for (int i = 0;
         i < ALARM_COUNT;
         i++)
    {
        const alarm_t *alarm =
            alarm_get(i);


        if (alarm == NULL)
        {
            continue;
        }


        if (alarm->state ==
            ALARM_STATE_ACTIVE)
        {
            count++;
        }
    }


    return count;
}



bool alarm_service_has_error(void)
{
    return
        alarm_service_count_active() > 0;
}



const alarm_t *
alarm_service_get_first_active(void)
{
    if (!s_initialized)
    {
        return NULL;
    }


    for (int i = 0;
         i < ALARM_COUNT;
         i++)
    {
        const alarm_t *alarm =
            alarm_get(i);


        if (alarm == NULL)
        {
            continue;
        }


        if (alarm->state ==
            ALARM_STATE_ACTIVE)
        {
            return alarm;
        }
    }


    return NULL;
}



void alarm_service_dump(void)
{
    printf("\n");
    printf("==============================\n");
    printf("Alarm service\n");
    printf("==============================\n");


    printf("Active alarms : %u\n",
           alarm_service_count_active());


    const alarm_t *alarm =
        alarm_service_get_first_active();


    if (alarm != NULL)
    {
        printf("First alarm   : %s\n",
               alarm_get_name(
                   alarm->type));

        printf("Value         : %.2f\n",
               alarm->value);
    }
    else
    {
        printf("No alarm\n");
    }


    printf("------------------------------\n");
}
