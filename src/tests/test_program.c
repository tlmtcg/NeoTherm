#include "test_program.h"

#include <stdio.h>

#include "clock.h"
#include "program.h"
#include "schedule.h"
#include "test_utils.h"

bool test_program_run(void)
{
    printf("\n=============== PROGRAM TEST ===============\n");

    /*
     * Initialisation du module program / schedule
     */
    ASSERT_TRUE(program_init());
    ASSERT_TRUE(schedule_init());

    printf("\n=============== SCHEDULE SET POINT TEST ===============\n");

    // Modifie la consigne du jeudi (jour 3) à 06:00 pour forcer 21.0°C (pour valider le test de consigne)
    ASSERT_TRUE(schedule_set_point(3, 6, 0, 21.0f));

    clock_time_t new_t =
        {
            .year = 2026,
            .month = 1,
            .day = 1, // Jeudi 1er janvier 2026
            .hour = 6,
            .minute = 0,
            .second = 0};

    ASSERT_TRUE(clock_set_time(&new_t));
    clock_sync_to_runtime();

    ASSERT_EQ_FLOAT(21.0f, schedule_get_setpoint());

    printf("PASS : Schedule Set Point\n");

    clock_time_t t =
        {
            .year = 2026,
            .month = 1,
            .day = 1,
            .hour = 5,
            .minute = 59,
            .second = 0};

    /*
     * Avant début de journée
     */

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    ASSERT_EQ_FLOAT(
        17.0f,
        schedule_get_setpoint());

    /*
     * Passage en période confort
     */

    t.hour = 6;
    t.minute = 0;

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    ASSERT_EQ_FLOAT(
        21.0f,
        schedule_get_setpoint());

    /*
     * Retour période réduite
     */

    t.hour = 23;
    t.minute = 0;

    ASSERT_TRUE(
        clock_set_time(&t));

    clock_sync_to_runtime();

    ASSERT_EQ_FLOAT(
        17.0f,
        schedule_get_setpoint());

    printf("PASS : Program\n");

    return true;
}
