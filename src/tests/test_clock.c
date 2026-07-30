#include <stdio.h>
#include "clock.h"
#include "test_utils.h"

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
     * Vérification de clock_seconds_today() à 12:00:00 
     * 12 * 3600 = 43200 secondes
     */
    ASSERT_EQ_UINT32(43200, clock_seconds_today());

    /*
     * +30 secondes
     */
    clock_tick(30);
    clock_get_time(&now);
    printf("+30 s   : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year, now.month, now.day, now.hour, now.minute, now.second);
    ASSERT_EQ_UINT32(43230, clock_seconds_today());

    /*
     * +40 secondes (passage à la minute : 1 minute et 10 secondes écoulées au total)
     */
    clock_tick(40);
    clock_get_time(&now);
    printf("+40 s   : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year, now.month, now.day, now.hour, now.minute, now.second);
    ASSERT_EQ_UINT32(43270, clock_seconds_today());

    /*
     * +1 heure (3600 secondes)
     */
    clock_tick(3600);
    clock_get_time(&now);
    printf("+1 h    : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year, now.month, now.day, now.hour, now.minute, now.second);
    ASSERT_EQ_UINT32(46870, clock_seconds_today());

    /*
     * Test de clock_add_seconds() et clock_add_second()
     */
    clock_add_second();
    clock_get_time(&now);
    ASSERT_EQ_UINT32(46871, clock_seconds_today());

    clock_add_seconds(10);
    clock_get_time(&now);
    ASSERT_EQ_UINT32(46881, clock_seconds_today());

    /*
     * Test du passage à minuit (rollover de clock_seconds_today)
     */
    clock_time_t presque_minuit =
    {
        .year   = 2026,
        .month  = 1,
        .day    = 1,
        .hour   = 23,
        .minute = 59,
        .second = 50
    };
    clock_set_time(&presque_minuit);
    ASSERT_EQ_UINT32(86390, clock_seconds_today());

    // Avance de 20 secondes (passe au jour suivant 00:00:10)
    clock_tick(20);
    clock_get_time(&now);
    printf("Minuit+ : %04u-%02u-%02u %02u:%02u:%02u\n",
           now.year, now.month, now.day, now.hour, now.minute, now.second);
    
    // clock_seconds_today() doit repartir à 10 secondes
    ASSERT_EQ_UINT32(10, clock_seconds_today());
    ASSERT_EQ_UINT32(2, now.day); // Passage au 2 janvier

    printf("PASS : Clock\n");

    return true;
}
