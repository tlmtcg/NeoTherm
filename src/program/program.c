#include "program.h"

#include "clock.h"
#include "schedule.h"
#include "logger.h"

bool program_init(void)
{
    schedule_init();

    LOG_INFO("PROGRAM",
             "Program initialized");

    return true;
}

/*==========================================================
 * Retourne la consigne programmée
 *=========================================================*/

float program_get_setpoint(void)
{
    clock_time_t now;

    if (!clock_get_time(&now))
    {
        return 20.5f;
    }

    LOG_INFO("PROGRAM",
             "Program setpoint = %.1f",
             schedule_get_setpoint());

    return schedule_get_setpoint();
}
