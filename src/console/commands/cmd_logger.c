#include "cmd_logger.h"

#include "console_utils.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>

bool cmd_logger(const char *args)
{
    if ((args == NULL) ||
        (*args == '\0') ||
        (strcmp(args, "status") == 0))
    {
        console_print_header("Logger");

        console_print_string(
            "Level",
            logger_level_name(
                logger_get_level()));

        return true;
    }

    if (strcmp(args, "debug") == 0)
    {
        logger_set_level(LOG_LEVEL_DEBUG);
    }
    else if (strcmp(args, "info") == 0)
    {
        logger_set_level(LOG_LEVEL_INFO);
    }
    else if (strcmp(args, "warn") == 0)
    {
        logger_set_level(LOG_LEVEL_WARN);
    }
    else if (strcmp(args, "error") == 0)
    {
        logger_set_level(LOG_LEVEL_ERROR);
    }
    else
    {
        printf("Usage:\n");
        printf("  logger status\n");
        printf("  logger debug\n");
        printf("  logger info\n");
        printf("  logger warn\n");
        printf("  logger error\n");

        return false;
    }

    console_print_string(
        "Level",
        logger_level_name(
            logger_get_level()));

    return true;
}
