#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stdint.h>
#include "logger.h"

#ifdef __cplusplus
extern "C"
{
#endif

    bool app_init(void);

    bool app_run(void);

    bool app_request_exit(void);

    bool app_exit_requested(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
