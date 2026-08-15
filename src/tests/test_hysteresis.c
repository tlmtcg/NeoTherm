#include "test_hysteresis.h"

#include <stdio.h>

#include "clock.h"
#include "climate.h"
#include "relay.h"
#include "thermostat_internal.h"
#include "test_utils.h"
#include "../console/console_utils.h"

bool test_hysteresis_run(void)
{
    console_print_header(
        "THERMOSTAT HYSTERESIS");

    /*
     * ==================================================
     * INITIALISATION
     * ==================================================
     */

    clock_init();
    climate_init();
    relay_init();
    relay_test_reset();
    relay_set_min_switch_delay(0);
    thermostat_init();

    /*
     * Ce test porte uniquement sur
     * l'hystérésis.
     */
    relay_set_min_switch_delay(0);

    ASSERT_TRUE(
        thermostat_set_mode(
            THERMOSTAT_AUTO));

    /*
     * ==================================================
     * CONFIGURATION HYSTERESIS
     * ==================================================
     */

    thermostat_set_hysteresis(0.3f);

    /*
     * En AUTO, la consigne effective est
     * celle du programme.
     */
    thermostat_update();

    const thermostat_status_t *status =
        thermostat_get_status();

    float setpoint =
        status->setpoint;

    float hysteresis =
        status->hysteresis;

    float on_threshold =
        setpoint - hysteresis;

    float off_threshold =
        setpoint + hysteresis;

    printf(
        "Setpoint       : %.2f C\n",
        setpoint);

    printf(
        "Hysteresis     : %.2f C\n",
        hysteresis);

    printf(
        "ON threshold   : %.2f C\n",
        on_threshold);

    printf(
        "OFF threshold  : %.2f C\n",
        off_threshold);

    /*
     * ==================================================
     * SOUS LE SEUIL ON
     * ==================================================
     *
     * La température est sous le seuil ON.
     *
     * => demande ON
     * => relais ON
     */

    console_print_header(
        "SOUS LE SEUIL ON");

    climate_test_set_temperature(
        on_threshold - 0.1f);

    thermostat_update();

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Below ON       : request ON / relay ON\n");

    /*
     * ==================================================
     * SEUIL ON
     * ==================================================
     *
     * Le seuil ON est inclus :
     *
     * temperature <= ON threshold
     *
     * => demande ON
     * => relais ON
     */

    console_print_header(
        "SEUIL ON");

    climate_test_set_temperature(
        on_threshold);

    thermostat_update();

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "At ON threshold: request ON / relay ON\n");

    /*
     * ==================================================
     * ZONE DE MAINTIEN - RELAIS ON
     * ==================================================
     *
     * La température revient dans la bande
     * d'hystérésis après une demande de chauffage.
     *
     * Le chauffage doit rester actif.
     *
     * => demande ON
     * => relais ON
     */

    console_print_header(
        "ZONE DE MAINTIEN - RELAIS ON");

    climate_test_set_temperature(
        setpoint);

    thermostat_update();

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Inside band    : request ON / relay remains ON\n");

    /*
     * ==================================================
     * AU-DESSUS DU SEUIL OFF
     * ==================================================
     *
     * La température dépasse le seuil OFF.
     *
     * => demande OFF
     * => relais OFF
     */

    console_print_header(
        "AU-DESSUS DU SEUIL OFF");

    climate_test_set_temperature(
        off_threshold + 0.1f);

    thermostat_update();

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    ASSERT_FALSE(
        relay_get());

    printf(
        "Above OFF      : request OFF / relay OFF\n");

    /*
     * ==================================================
     * ZONE DE MAINTIEN - RELAIS OFF
     * ==================================================
     *
     * Après l'arrêt du chauffage, la température
     * revient dans la bande.
     *
     * Le chauffage ne doit PAS redémarrer.
     *
     * => demande OFF
     * => relais OFF
     */

    console_print_header(
        "ZONE DE MAINTIEN - RELAIS OFF");

    climate_test_set_temperature(
        setpoint);

    thermostat_update();

    ASSERT_FALSE(
        thermostat_get_status()->heating_request);

    ASSERT_FALSE(
        relay_get());

    printf(
        "Inside band    : request OFF / relay remains OFF\n");

    /*
     * ==================================================
     * RETOUR SOUS LE SEUIL ON
     * ==================================================
     *
     * La température repasse sous le seuil ON.
     *
     * => demande ON
     * => relais ON
     */

    console_print_header(
        "RETOUR SOUS LE SEUIL ON");

    climate_test_set_temperature(
        on_threshold - 0.1f);

    thermostat_update();

    ASSERT_TRUE(
        thermostat_get_status()->heating_request);

    ASSERT_TRUE(
        relay_get());

    printf(
        "Below ON again : request ON / relay ON\n");

    /*
     * ==================================================
     * VERIFICATION DES SEUILS
     * ==================================================
     */

    console_print_header(
        "VERIFICATION DES SEUILS");

    ASSERT_TRUE(
        on_threshold <
        setpoint);

    ASSERT_TRUE(
        off_threshold >
        setpoint);

    ASSERT_EQ_FLOAT(
        0.3f,
        hysteresis);

    printf(
        "ON threshold   : %.2f C\n",
        on_threshold);

    printf(
        "OFF threshold  : %.2f C\n",
        off_threshold);

    printf(
        "PASS : Thermostat hysteresis\n");

    return true;
}
