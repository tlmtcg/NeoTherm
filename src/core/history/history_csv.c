#include "history_csv.h"

#include <stdio.h>

#include "history.h"
#include "logger.h"
#include "thermostat.h"

bool history_export_csv(
    const char *filename)
{
    FILE *fp = fopen(
        filename,
        "w");


    if (fp == NULL)
    {
        LOG_ERROR("HISTORY",
                  "Unable to create CSV file");

        return false;
    }


    fprintf(fp,
            "Date,Inside,Outside,Setpoint,Mode,Relay,Heating\n");


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
                "%04u-%02u-%02u %02u:%02u:%02u,"
                "%.2f,"
                "%.2f,"
                "%.2f,"
                "%s,"
                "%s,"
                "%s\n",

                record.timestamp.year,
                record.timestamp.month,
                record.timestamp.day,

                record.timestamp.hour,
                record.timestamp.minute,
                record.timestamp.second,

                record.inside_temperature,
                record.outside_temperature,
                record.setpoint,

                thermostat_mode_to_string(record.mode),

                record.relay ? "ON" : "OFF",

                record.heating ? "YES" : "NO");
    }


    fclose(fp);


    LOG_INFO("HISTORY",
             "CSV exported : %s (%u records)",
             filename,
             count);


    return true;
}

void history_csv_task_callback(void)
{
    history_export_csv("../history.csv");
}