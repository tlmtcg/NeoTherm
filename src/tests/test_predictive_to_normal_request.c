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
     * TEMPERATURE INITIALE
     * --------------------------------------------------
     *
     * Consigne : 18.0 °C
     * Hystérésis : 0.3 °C
     *
     * Seuil ON : 17.7 °C
     *
     * À 17.9 °C :
     * - demande normale = NON
     * - prédiction naturelle sous la marge
     * - demande prédictive = OUI
     */

    climate_test_set_temperature(
        17.90f);

    thermostat_set_mode(
        THERMOSTAT_AUTO);

    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    ASSERT_TRUE(
        status->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Predictive request : ON\n");

    /*
     * --------------------------------------------------
     * DESCENTE SOUS LE SEUIL ON
     * --------------------------------------------------
     *
     * La température passe sous 17.70 °C.
     *
     * La demande devient maintenant normale.
     */

    climate_test_set_temperature(
        17.60f);

    thermostat_update();

    status =
        thermostat_get_status();

    /*
     * La demande doit toujours être active.
     */

    ASSERT_TRUE(
        status->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Normal request     : ON\n");

    /*
     * --------------------------------------------------
     * VERIFICATION DU SEUIL
     * --------------------------------------------------
     */

    ASSERT_TRUE(
        17.60f <=
        (status->setpoint -
         status->hysteresis));

    printf("PASS\n");

    return true;
}
