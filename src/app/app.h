#ifndef APP_H
#define APP_H

#include <stdbool.h>

#define APP_SIMULATION_MODE    1
#define APP_SIMULATION_TICKS   150
#define APP_SIMULATION_DELAY_MS 100

#ifdef __cplusplus
extern "C" {
#endif


bool app_init(void);

void app_run(void);


#ifdef __cplusplus
}
#endif

#endif /* APP_H */
