#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

bool logger_init(void)
{
    return true;
}

/*----------------------------------------------------------
 * API publique
 *---------------------------------------------------------*/

void logger_log(log_level_t level,
                const char *tag,
                const char *fmt,
                ...)
{
    const char *level_str = "?????";

    switch (level)
    {
        case LOG_LEVEL_DEBUG:
            level_str = "DEBUG";
            break;

        case LOG_LEVEL_INFO:
            level_str = "INFO ";
            break;

        case LOG_LEVEL_WARN:
            level_str = "WARN ";
            break;

        case LOG_LEVEL_ERROR:
            level_str = "ERROR";
            break;
    }

    printf("[%s] %-12s ", level_str, tag);

    va_list args;

    va_start(args, fmt);

    vprintf(fmt, args);

    va_end(args);

    printf("\n");
}
