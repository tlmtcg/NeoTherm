#include "test_predictive_to_normal_request.h"
#include "test_utils.h"
#include "../console/console_utils.h"
#include "climate.h"
#include "thermostat.h"
#include "relay.h"

bool test_predictive_to_normal_request_run(void)
{
    console_print_header(
        "PREDICTIVE TO NORMAL REQUEST");


    /*
     * --------------------------------------------------
     * INITIALISATION
     * --------------------------------------------------
     *
     * Consigne : 18.0 °C
     * Hystérésis : 0.3 °C
     *
     * Seuil ON : 17.7 °C
     */


    climate_test_set_temperature(
        17.90f);

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();


    /*
     * À 17.90 °C :
     *
     * 17.90 > 17.70
     *
     * donc pas de demande normale.
     *
     * La prédiction doit cependant demander
     * le chauffage.
     */

    ASSERT_TRUE(
        status->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Predictive request : ON\n");


    /*
     * --------------------------------------------------
     * PASSAGE SOUS LE SEUIL ON
     * --------------------------------------------------
     *
     * 17.60 <= 17.70
     *
     * La demande devient maintenant normale.
     *
     * La demande de chauffage doit rester active.
     */

    climate_test_set_temperature(
        17.60f);

    thermostat_update();

    status =
        thermostat_get_status();


    ASSERT_TRUE(
        status->heating_request);

    ASSERT_TRUE(
        relay_get());


    /*
     * Vérification du seuil.
     */

    ASSERT_TRUE(
        17.60f <=
        (status->setpoint -
         status->hysteresis));


    printf(
        "Normal request     : ON\n");

    printf(
        "PASS\n");

    return true;
}
