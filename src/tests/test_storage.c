#include "test_storage.h"

#include <stdio.h>
#include <math.h>

#include "logger.h"
#include "storage.h"
#include "thermostat.h"


#define FLOAT_EPSILON 0.01f


bool test_storage_run(void)
{
    printf("\n=============== STORAGE TEST ===============\n");


    /*
     * Initialisation
     */

    storage_init();


    /*
     * Sauvegarde initiale
     */

    printf("\nSaving values...\n");


    if (!storage_save_setpoint(19.5f))
    {
        printf("FAIL : save_setpoint\n");
        return false;
    }


    if (!storage_save_mode(
            THERMOSTAT_MANUAL))
    {
        printf("FAIL : save_mode\n");
        return false;
    }


    /*
     * Lecture
     */

    float setpoint = 0.0f;

    thermostat_mode_t mode =
        THERMOSTAT_OFF;


    if (!storage_load_setpoint(
            &setpoint))
    {
        printf("FAIL : load_setpoint\n");
        return false;
    }


    if (!storage_load_mode(
            &mode))
    {
        printf("FAIL : load_mode\n");
        return false;
    }


    printf("Loaded setpoint : %.1f\n",
           setpoint);

    printf("Loaded mode     : %s\n",
           thermostat_mode_name(mode));


    /*
     * Vérification
     */

    if (fabsf(setpoint - 19.5f) > FLOAT_EPSILON)
    {
        printf("FAIL : setpoint expected=19.5 got=%.2f\n",
               setpoint);

        return false;
    }


    if (mode != THERMOSTAT_MANUAL)
    {
        printf("FAIL : mode expected=MANUAL got=%s\n",
               thermostat_mode_name(mode));

        return false;
    }



    /*
     * Test écrasement
     */

    printf("\nOverwrite values...\n");


    if (!storage_save_setpoint(22.0f))
    {
        printf("FAIL : overwrite setpoint save\n");
        return false;
    }


    if (!storage_save_mode(
            THERMOSTAT_AUTO))
    {
        printf("FAIL : overwrite mode save\n");
        return false;
    }


    setpoint = 0.0f;
    mode = THERMOSTAT_OFF;


    if (!storage_load_setpoint(
            &setpoint))
    {
        printf("FAIL : reload setpoint\n");
        return false;
    }


    if (!storage_load_mode(
            &mode))
    {
        printf("FAIL : reload mode\n");
        return false;
    }


    printf("Reloaded setpoint : %.1f\n",
           setpoint);

    printf("Reloaded mode     : %s\n",
           thermostat_mode_name(mode));


    if (fabsf(setpoint - 22.0f) > FLOAT_EPSILON)
    {
        printf("FAIL : overwrite setpoint\n");
        return false;
    }


    if (mode != THERMOSTAT_AUTO)
    {
        printf("FAIL : overwrite mode\n");
        return false;
    }


    printf("\nPASS : Storage\n");

    return true;
}
