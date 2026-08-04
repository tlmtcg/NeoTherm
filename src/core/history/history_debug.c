#include "history_debug.h"

#include "history.h"

#include <stdio.h>

/*==========================================================
 * Affichage de l'historique
 *=========================================================*/

void history_dump(void)
{
    history_record_t record;

    printf("\n");
    printf("=============== HISTORY ===============\n\n");

    printf("%-19s %-8s %-8s %-8s %-10s %-6s %-8s\n",
           "Date",
           "Inside",
           "Outside",
           "Set",
           "Mode",
           "Relay",
           "Heating");

    printf("----------------------------------------------------------\n");

    for (uint32_t i = 0; i < history_count(); i++)
    {
        if (!history_get(i, &record))
        {
            continue;
        }

        const char *mode = "UNKNOWN";

        switch (record.mode)
        {
        case THERMOSTAT_OFF:
            mode = "OFF";
            break;

        case THERMOSTAT_MANUAL:
            mode = "MANUAL";
            break;

        case THERMOSTAT_AUTO:
            mode = "AUTO";
            break;

        case THERMOSTAT_HORS_GEL:
            mode = "HORS_GEL";
            break;

        default:
            break;
        }

        printf("%04u-%02u-%02u %02u:%02u:%02u "
               "%-8.2f %-8.2f %-8.2f %-10s %-6s %-6s\n",
               record.timestamp.year,
               record.timestamp.month,
               record.timestamp.day,
               record.timestamp.hour,
               record.timestamp.minute,
               record.timestamp.second,
               record.inside_temperature,
               record.outside_temperature,
               record.setpoint,
               mode,
               record.relay ? "ON" : "OFF",
               record.heating ? "YES" : "NO");
    }

    printf("\n");
}
