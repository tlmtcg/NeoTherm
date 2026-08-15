#include "test_setpoint.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "relay.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../console/console_utils.h"


bool test_setpoint_run(void)
{
    console_print_header(
        "THERMOSTAT SETPOINT");


    /*
     * ==================================================
     * INITIALISATION
     * ==================================================
     */

    clock_init();
    climate_init();
    relay_init();
    relay_test_reset();
    thermostat_init();

    relay_set_min_switch_delay(0);

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));


    /*
     * ==================================================
     * MODIFICATION NORMALE
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_setpoint(22.0f));

    ASSERT_EQ_FLOAT(
        22.0f,
        thermostat_get_setpoint());

    printf(
        "Setpoint 22.0 C : OK\n");


    /*
     * ==================================================
     * LIMITE BASSE
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_setpoint(5.0f));

    ASSERT_EQ_FLOAT(
        5.0f,
        thermostat_get_setpoint());

    printf(
        "Setpoint 5.0 C  : OK\n");


    /*
     * ==================================================
     * LIMITE HAUTE
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_setpoint(35.0f));

    ASSERT_EQ_FLOAT(
        35.0f,
        thermostat_get_setpoint());

    printf(
        "Setpoint 35.0 C : OK\n");


    /*
     * ==================================================
     * VALEUR TROP BASSE
     * ==================================================
     */

    ASSERT_FALSE(
        thermostat_set_setpoint(4.9f));

    ASSERT_EQ_FLOAT(
        35.0f,
        thermostat_get_setpoint());

    printf(
        "Setpoint 4.9 C  : rejected\n");


    /*
     * ==================================================
     * VALEUR TROP HAUTE
     * ==================================================
     */

    ASSERT_FALSE(
        thermostat_set_setpoint(35.1f));

    ASSERT_EQ_FLOAT(
        35.0f,
        thermostat_get_setpoint());

    printf(
        "Setpoint 35.1 C : rejected\n");


    /*
     * ==================================================
     * HORS GEL
     * ==================================================
     *
     * La consigne utilisateur reste mémorisée,
     * mais la consigne effective est forcée
     * à HORS_GEL_SETPOINT.
     */

    ASSERT_TRUE(
        thermostat_set_setpoint(20.0f));

    ASSERT_EQ_FLOAT(
        20.0f,
        thermostat_get_setpoint());

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_HORS_GEL));

    ASSERT_EQ_FLOAT(
        HORS_GEL_SETPOINT,
        thermostat_get_setpoint());

    printf(
        "HORS GEL       : %.1f C\n",
        thermostat_get_setpoint());


    /*
     * ==================================================
     * SORTIE HORS GEL
     * ==================================================
     *
     * La consigne utilisateur doit redevenir active.
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));

    ASSERT_EQ_FLOAT(
        20.0f,
        thermostat_get_setpoint());

    printf(
        "AUTO           : 20.0 C\n");


    /*
     * ==================================================
     * RESULTAT
     * ==================================================
     */

    printf(
        "PASS : Thermostat setpoint\n");

    return true;
}
