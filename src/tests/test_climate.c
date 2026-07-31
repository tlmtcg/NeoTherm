#include "test_climate.h"

#include <stdio.h>
#include <math.h>

#include "climate.h"
#include "thermal_model.h"
#include "event.h"

#define ASSERT_EQ_FLOAT(expected, actual)             \
    do                                                \
    {                                                 \
        if (fabsf((expected) - (actual)) > 0.01f)     \
        {                                             \
            printf("FAIL : expected %.2f got %.2f\n", \
                   expected, actual);                 \
            return false;                             \
        }                                             \
    } while (0)

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

    ASSERT_EQ_FLOAT(
        21.5f,
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
