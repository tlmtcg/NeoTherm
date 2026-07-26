#include "thermostat_test.h"

#include "thermostat.h"
#include "climate.h"
#include "relay.h"
#include "logger.h"


static void test_case(
    const char *name,
    float temperature,
    bool expected_relay)
{
    LOG_INFO("TEST",
             "----- %s -----",
             name);


    climate_test_set_temperature(temperature);


    thermostat_update();


    bool result = relay_get();


    if (result == expected_relay)
    {
        LOG_INFO("TEST",
                 "PASS Temp=%.1f Relay=%s",
                 temperature,
                 result ? "ON" : "OFF");
    }
    else
    {
        LOG_ERROR("TEST",
                  "FAIL Temp=%.1f Expected=%s Got=%s",
                  temperature,
                  expected_relay ? "ON" : "OFF",
                  result ? "ON" : "OFF");
    }
}


void thermostat_test_run(void)
{
    LOG_INFO("TEST",
             "===== THERMOSTAT TEST =====");


    /*
     * Mode AUTO
     */

    thermostat_set_mode(
        THERMOSTAT_AUTO);


    test_case(
        "AUTO chauffage",
        20.0f,
        true);


    test_case(
        "AUTO maintien",
        20.5f,
        true);


    test_case(
        "AUTO arret",
        20.8f,
        false);



    /*
     * Mode OFF
     */

    thermostat_set_mode(
        THERMOSTAT_OFF);


    test_case(
        "OFF force",
        15.0f,
        false);



    /*
     * Mode HORS GEL
     */

    thermostat_set_mode(
        THERMOSTAT_HORS_GEL);


    test_case(
        "HORS GEL ON",
        5.0f,
        true);


    test_case(
        "HORS GEL OFF",
        9.0f,
        false);



    /*
     * Mode MANUAL
     */

    thermostat_set_mode(
        THERMOSTAT_MANUAL);


    thermostat_manual_set_relay(true);


    test_case(
        "MANUAL ON",
        25.0f,
        true);


    thermostat_manual_set_relay(false);


    test_case(
        "MANUAL OFF",
        25.0f,
        false);
        
    thermostat_set_mode(THERMOSTAT_AUTO);

    LOG_INFO("TEST",
             "===== END TEST =====");
}