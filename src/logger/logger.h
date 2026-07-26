#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

/*----------------------------------------------------------
 * Types
 *---------------------------------------------------------*/

typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR

} log_level_t;

/*----------------------------------------------------------
 * API
 *---------------------------------------------------------*/

bool logger_init(void);

void logger_log(log_level_t level,
                const char *tag,
                const char *fmt,
                ...);

/*----------------------------------------------------------
 * Macros
 *---------------------------------------------------------*/

#define LOG_DEBUG(tag, fmt, ...) \
    logger_log(LOG_LEVEL_DEBUG, tag, fmt, ##__VA_ARGS__)

#define LOG_INFO(tag, fmt, ...) \
    logger_log(LOG_LEVEL_INFO, tag, fmt, ##__VA_ARGS__)

#define LOG_WARN(tag, fmt, ...) \
    logger_log(LOG_LEVEL_WARN, tag, fmt, ##__VA_ARGS__)

#define LOG_ERROR(tag, fmt, ...) \
    logger_log(LOG_LEVEL_ERROR, tag, fmt, ##__VA_ARGS__)

#endif
