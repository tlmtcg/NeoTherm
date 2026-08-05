#include "test_climate.h"

#include <stdio.h>
#include <math.h>

#include "climate.h"
#include "thermal_model.h"
#include "event.h"
#include "runtime.h"
#include "test_utils.h"

bool test_climate_run(void)
{
    printf("\n=============== CLIMATE TEST ===============\n");

    /*
     * Initialisation environnement
     */

    event_init();

    thermal_model_init();

    /*
     * Initialisation climate
     */

    climate_init();

    float temp =
        climate_get_temperature();

    printf("Initial : %.2f C\n",
           temp);

    /*
     * Après initialisation, la température
     * doit être initialisée avec la consigne
     * courante du runtime.
     */
    ASSERT_EQ_FLOAT(
        runtime_get()->setpoint,
        temp);

    /*
     * Test modification température
     */

    climate_update(
        18.75f);

    temp =
        climate_get_temperature();

    printf("Updated : %.2f C\n",
           temp);

    ASSERT_EQ_FLOAT(
        18.75f,
        temp);

    printf("\nPASS : Climate\n");

    return true;
}
