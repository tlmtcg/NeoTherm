#include "test_history.h"

#include <stdio.h>

#include "clock.h"
#include "history.h"
#include "history_debug.h"
#include "logger.h"
#include "thermostat.h"

bool test_history_run(void)
{
    printf("\n=============== HISTORY TEST ===============\n");

    history_init();

    clock_time_t t =
    {
        .year   = 2026,
        .month  = 1,
        .day    = 1,
        .hour   = 12,
        .minute = 0,
        .second = 0
    };

    clock_set_time(&t);

    /*
     * Création de 10 enregistrements
     */

    for (uint32_t i = 0; i < 10; i++)
    {
        history_add(
            20.0f + (float)i * 0.1f,
            5.0f,
            21.0f,
            THERMOSTAT_AUTO,
            (i & 1U),
            (i & 1U));

        clock_tick(10);
    }

    printf("History count : %u\n",
           history_count());

    history_record_t record;

    if (history_get_latest(&record))
    {
        printf("Latest record\n");

        printf(" Tick        : %u\n", record.tick);
        printf(" Temperature : %.2f\n", record.inside_temperature);
        printf(" Relay       : %s\n",
               record.relay ? "ON" : "OFF");
    }

    history_dump();

    history_save("../history.dat");

    history_clear();

    printf("After clear : %u\n",
           history_count());

    history_load("../history.dat");

    printf("After load  : %u\n",
           history_count());

    history_dump();

    printf("PASS : History\n");

    return true;
}
