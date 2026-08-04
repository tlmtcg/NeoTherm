#include "program.h"

#include "clock.h"
#include "schedule.h"
#include "logger.h"

bool program_init(void)
{
    if (!schedule_init())
    {
        LOG_ERROR("PROGRAM",
                  "Unable to initialize schedule");

        return false;
    }

    LOG_INFO("PROGRAM",
             "Program initialized");

    return true;
}

/*==========================================================
 * Retourne la consigne programmée
 *=========================================================*/

float program_get_setpoint(void)
{
    float setpoint = schedule_get_setpoint();

    LOG_DEBUG("PROGRAM",
             "Program setpoint = %.1f",
             setpoint);

    return setpoint;
}
