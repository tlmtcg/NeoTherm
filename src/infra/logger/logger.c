#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

static log_level_t s_log_level = LOG_LEVEL_INFO;

bool logger_init(void)
{
    s_log_level = LOG_LEVEL_INFO;

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

    if (level < s_log_level)
    {
        return;
    }

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

void logger_set_level(log_level_t level)
{
    s_log_level = level;
}

log_level_t logger_get_level(void)
{
    return s_log_level;
}

const char *logger_level_name(log_level_t level)
{
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        return "DEBUG";

    case LOG_LEVEL_INFO:
        return "INFO";

    case LOG_LEVEL_WARN:
        return "WARN";

    case LOG_LEVEL_ERROR:
        return "ERROR";

    default:
        return "UNKNOWN";
    }
}
