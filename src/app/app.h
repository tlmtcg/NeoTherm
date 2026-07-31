#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stdint.h>
#include "logger.h"

#define APP_SIMULATION_MODE 1
#define APP_SIMULATION_TICKS 150
#define APP_SIMULATION_DELAY_MS 100
#define TEST_MODE 1

#ifdef __cplusplus
extern "C"
{
#endif

    bool app_init(void);

    bool app_run(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
