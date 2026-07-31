#include "history.h"

#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "thermostat.h"

bool history_save(
    const char *filename)
{
    FILE *fp = fopen(
        filename,
        "w");

    if (fp == NULL)
    {
        LOG_ERROR("HISTORY",
                  "Unable to save history");

        return false;
    }

    uint32_t count =
        history_count();

    for (uint32_t i = 0; i < count; i++)
    {
        history_record_t record;

        if (!history_get(i, &record))
        {
            continue;
        }

        fprintf(fp,
                "%u;"
                "%04u-%02u-%02u %02u:%02u:%02u;"
                "%.2f;"
                "%.2f;"
                "%.2f;"
                "%d;"
                "%d;"
                "%d\n",

                record.tick,

                record.timestamp.year,
                record.timestamp.month,
                record.timestamp.day,
                record.timestamp.hour,
                record.timestamp.minute,
                record.timestamp.second,

                record.inside_temperature,
                record.outside_temperature,
                record.setpoint,

                record.mode,
                record.relay,
                record.heating);
    }

    fclose(fp);

    LOG_INFO("HISTORY",
             "History saved : %u records",
             count);

    return true;
}

bool history_load(
    const char *filename)
{
    FILE *fp = fopen(
        filename,
        "r");

    if (fp == NULL)
    {
        LOG_WARN("HISTORY",
                 "No history file found");

        return false;
    }

    history_clear();

    char line[256];

    uint32_t index = 0;

    while ((index < HISTORY_SIZE) &&
           fgets(line,
                 sizeof(line),
                 fp))
    {
        history_record_t record;

        int mode;
        int relay;
        int heating;

        if (sscanf(line,
                   "%u;"
                   "%u-%u-%u %u:%u:%u;"
                   "%f;"
                   "%f;"
                   "%f;"
                   "%d;"
                   "%d;"
                   "%d",

                   &record.tick,

                   &record.timestamp.year,
                   &record.timestamp.month,
                   &record.timestamp.day,

                   &record.timestamp.hour,
                   &record.timestamp.minute,
                   &record.timestamp.second,

                   &record.inside_temperature,
                   &record.outside_temperature,
                   &record.setpoint,

                   &mode,
                   &relay,
                   &heating) != 13)
        {
            continue;
        }

        record.mode = (thermostat_mode_t)mode;
        record.relay = (bool)relay;
        record.heating = (bool)heating;

        if (history_push(&record))
        {
            index++;
        }
    }

    fclose(fp);

    LOG_INFO("HISTORY",
             "History loaded : %u records",
             index);

    return true;
}

void history_task_callback(void)
{
    history_save("history.csv");
}