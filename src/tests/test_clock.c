#include <stdio.h>

#include "clock.h"

bool test_clock_run(void)
{
    printf("\n================ CLOCK TEST ================\n");

    clock_init();

    clock_time_t t =
    {
        .year   = 2026,
        .month  = 1,
        .day    = 1,
        .hour   = 12,
        .minute = 0,
        .second = 0
    };

    if (!clock_set_time(&t))
    {
        printf("FAIL : clock_set_time()\n");
        return false;
    }

    clock_time_t now;

    if (!clock_get_time(&now))
    {
        printf("FAIL : clock_get_time()\n");
        return false;
    }

    printf("Initial : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year,
           now.month,
           now.day,
           now.hour,
           now.minute,
           now.second);

    /*
     * +30 secondes
     */

    clock_tick(30);

    clock_get_time(&now);

    printf("+30 s   : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year,
           now.month,
           now.day,
           now.hour,
           now.minute,
           now.second);

    /*
     * +40 secondes
     */

    clock_tick(40);

    clock_get_time(&now);

    printf("+40 s   : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year,
           now.month,
           now.day,
           now.hour,
           now.minute,
           now.second);

    /*
     * +1 heure
     */

    clock_tick(3600);

    clock_get_time(&now);

    printf("+1 h    : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year,
           now.month,
           now.day,
           now.hour,
           now.minute,
           now.second);

    printf("PASS : Clock\n");

    return true;
}