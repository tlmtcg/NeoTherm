#include "alarm_history.h"

#include <string.h>

#include "clock.h"
#include "console_utils.h"
#include <stdio.h>
#include <time.h>

static alarm_history_entry_t s_history[ALARM_HISTORY_SIZE];

static uint32_t s_head = 0;
static uint32_t s_count = 0;


void alarm_history_init(void)
{
    memset(
        s_history,
        0,
        sizeof(s_history));

    s_head = 0;
    s_count = 0;
}


void alarm_history_add(
    alarm_type_t type,
    alarm_state_t state,
    float value)
{
    alarm_history_entry_t *entry =
        &s_history[s_head];

    entry->type = type;
    entry->state = state;
    entry->value = value;
    entry->timestamp = clock_get_timestamp();

    s_head++;

    if (s_head >= ALARM_HISTORY_SIZE)
    {
        s_head = 0;
    }

    if (s_count < ALARM_HISTORY_SIZE)
    {
        s_count++;
    }
}


uint32_t alarm_history_count(void)
{
    return s_count;
}


const alarm_history_entry_t *alarm_history_get(
    uint32_t index)
{
    if (index >= s_count)
    {
        return NULL;
    }

    uint32_t first =
        (s_head + ALARM_HISTORY_SIZE - s_count)
        % ALARM_HISTORY_SIZE;

    uint32_t pos =
        (first + index)
        % ALARM_HISTORY_SIZE;

    return &s_history[pos];
}


void alarm_history_dump(void)
{
    console_print_header("Alarm History");

    printf("%-12s %-10s %-8s %s\n",
           "Alarm",
           "State",
           "Value",
           "Date");

    console_print_separator();

    for (uint32_t i = 0;
         i < s_count;
         i++)
    {
        const alarm_history_entry_t *entry =
            alarm_history_get(i);

        if (entry == NULL)
        {
            continue;
        }

        const char *state = "CLEAR";

        switch (entry->state)
        {
        case ALARM_STATE_ACTIVE:
            state = "ACTIVE";
            break;

        case ALARM_STATE_ACK:
            state = "ACK";
            break;

        default:
            break;
        }

        char date[32] = "-";

        time_t t = (time_t)entry->timestamp;
        struct tm *tm = localtime(&t);

        if (tm != NULL)
        {
            snprintf(date,
                     sizeof(date),
                     "%04d-%02d-%02d %02d:%02d:%02d",
                     tm->tm_year + 1900,
                     tm->tm_mon + 1,
                     tm->tm_mday,
                     tm->tm_hour,
                     tm->tm_min,
                     tm->tm_sec);
        }

        printf("%-12s %-10s %-8.2f %s\n",
               alarm_get_command_name(entry->type),
               state,
               entry->value,
               date);
    }

    console_print_separator();
}

