#include "test_hors_gel.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "relay.h"
#include "thermostat.h"
#include "test_utils.h"
#include "../console/console_utils.h"


bool test_hors_gel_run(void)
{
    console_print_header(
        "THERMOSTAT HORS GEL");


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


    /*
     * ==================================================
     * ACTIVATION HORS GEL
     * ==================================================
     */

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_HORS_GEL));

    ASSERT_EQ_INT(
        THERMOSTAT_HORS_GEL,
        thermostat_get_mode());

    ASSERT_EQ_FLOAT(
        HORS_GEL_SETPOINT,
        thermostat_get_setpoint());

    printf(
        "Mode        : HORS GEL\n");

    printf(
        "Setpoint    : %.2f C\n",
        thermostat_get_setpoint());


    /*
     * ==================================================
     * TEMPERATURE SOUS LE SEUIL ON
     * ==================================================
     *
     * Consigne : 7.0 °C
     * Hystérésis : 0.3 °C
     *
     * Seuil ON = 6.7 °C
     */

    climate_test_set_temperature(
        HORS_GEL_SETPOINT -
        HORS_GEL_HYSTERESIS -
        0.5f);

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    printf(
        "Below ON    : relay ON\n");


    /*
     * ==================================================
     * DANS LA BANDE D'HYSTERESIS
     * ==================================================
     *
     * Le relais doit rester ON.
     */

    climate_test_set_temperature(
        HORS_GEL_SETPOINT);

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    printf(
        "Hysteresis   : relay remains ON\n");


    /*
     * ==================================================
     * AU-DESSUS DU SEUIL OFF
     * ==================================================
     *
     * Seuil OFF = 7.3 °C
     */

    climate_test_set_temperature(
        HORS_GEL_SETPOINT +
        HORS_GEL_HYSTERESIS +
        0.5f);

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    printf(
        "Above OFF    : relay OFF\n");


    /*
     * ==================================================
     * RETOUR SOUS LE SEUIL
     * ==================================================
     */

    climate_test_set_temperature(
        HORS_GEL_SETPOINT -
        HORS_GEL_HYSTERESIS -
        0.5f);

    thermostat_update();

    ASSERT_TRUE(
        relay_get());

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    printf(
        "Below ON     : relay ON again\n");


    /*
     * ==================================================
     * SORTIE HORS GEL
     * ==================================================
     *
     * Passage en AUTO avec température élevée.
     * Le relais doit être coupé.
     */

    climate_test_set_temperature(
        20.0f);

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));

    thermostat_update();

    ASSERT_FALSE(
        relay_get());

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    printf(
        "AUTO         : relay OFF\n");


    printf(
        "PASS : Thermostat hors gel\n");

    return true;
}
