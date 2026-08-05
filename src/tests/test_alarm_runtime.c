#include "test_alarm_runtime.h"

#include <stdio.h>

#include "alarm.h"
#include "alarm_runtime.h"

#include "climate.h"
#include "thermostat.h"
#include "thermal_model.h"
#include "event.h"

#include "test_utils.h"


bool test_alarm_runtime_run(void)
{
    printf("\n=============== ALARM RUNTIME TEST ===============\n");


    /*
     * Initialisation système minimale
     */

    event_init();

    thermal_model_init();

    climate_init();

    alarm_init();

    alarm_runtime_init();

    thermostat_init();



    /*
     * Situation normale
     */

    climate_update(20.0f);

    thermostat_update();


    ASSERT_FALSE(
        alarm_is_active(ALARM_TEMP_HIGH));


    ASSERT_FALSE(
        alarm_is_active(ALARM_TEMP_LOW));



    /*
     * Température trop haute
     */

    climate_update(35.0f);

    thermostat_update();

    printf("ALARM_TEMP_HIGH = %s\n",
       alarm_is_active(ALARM_TEMP_HIGH) ? "ON" : "OFF");

    ASSERT_TRUE(
        alarm_is_active(ALARM_TEMP_HIGH));


    ASSERT_FALSE(
        alarm_is_active(ALARM_TEMP_LOW));



    /*
     * Retour température normale
     */

    climate_update(20.0f);

    thermostat_update();


    ASSERT_FALSE(
        alarm_is_active(ALARM_TEMP_HIGH));



    /*
     * Température trop basse
     */

    climate_update(2.0f);

    thermostat_update();


    ASSERT_TRUE(
        alarm_is_active(ALARM_TEMP_LOW));


    ASSERT_FALSE(
        alarm_is_active(ALARM_TEMP_HIGH));



    /*
     * Retour température normale
     */

    climate_update(20.0f);

    thermostat_update();


    ASSERT_FALSE(
        alarm_is_active(ALARM_TEMP_LOW));



    printf("ALARM RUNTIME TEST PASS\n");

    return true;
}
