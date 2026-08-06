#include "alarm_history.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "clock.h"
#include "console_utils.h"
#include "alarm_storage.h"

static alarm_history_t s_history;

void alarm_history_init(void)
{
    memset(
        &s_history,
        0,
        sizeof(s_history));

    s_history.count = 0;
    s_history.head = 0;
    s_history.dirty = false;
}

void alarm_history_add(
    alarm_type_t type,
    alarm_state_t state,
    float value)
{
    alarm_history_entry_t *entry =
        &s_history.entries[s_history.head];


    entry->type = type;
    entry->state = state;
    entry->value = value;
    entry->timestamp = clock_get_timestamp();


    s_history.head =
        (s_history.head + 1)
        % ALARM_HISTORY_SIZE;


    if (s_history.count < ALARM_HISTORY_SIZE)
    {
        s_history.count++;
    }


    s_history.dirty = true;

}

uint32_t alarm_history_count(void)
{
    return s_history.count;
}

const alarm_history_entry_t *alarm_history_get(
    uint32_t index)
{
    if (index >= s_history.count)
    {
        return NULL;
    }

    uint32_t first =
        (s_history.count == ALARM_HISTORY_SIZE)
            ? s_history.head
            : 0;

    uint32_t pos =
        (first + index) % ALARM_HISTORY_SIZE;

    return &s_history.entries[pos];
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
         i < s_history.count;
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

        char date[64] = "-";

        time_t t =
            (time_t)entry->timestamp;

        struct tm *tm =
            localtime(&t);

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

bool alarm_history_clear(void)
{

    memset(&s_history,
           0,
           sizeof(s_history));

    s_history.count = 0;
    s_history.head = 0;
    s_history.dirty = true;

    return true;
}

bool alarm_history_save(void)
{
    if (!s_history.dirty)
    {
        return true;
    }

    if (!alarm_storage_save(&s_history))
    {
        return false;
    }

    s_history.dirty = false;

    return true;
}

bool alarm_history_load(void)
{
    alarm_history_t history;

    memset(&history,
           0,
           sizeof(history));


    bool ok = alarm_storage_load(&history);

    if (!ok)
    {
        return false;
    }


    s_history = history;


    if (s_history.count > ALARM_HISTORY_SIZE)
    {
        s_history.count = ALARM_HISTORY_SIZE;
    }


    if (s_history.head >= ALARM_HISTORY_SIZE)
    {
        s_history.head = 0;
    }


    s_history.dirty = false;

    return true;
}

void alarm_history_task(void)
{
    if (!s_history.dirty)
    {
        return;
    }

    if (alarm_history_save())
    {
        s_history.dirty = false;
    }
}

bool alarm_history_is_dirty(void)
{
    return s_history.dirty;
}
