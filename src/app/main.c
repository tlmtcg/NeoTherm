#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "debug.h"

int main(void)
{
    logger_init();
    if (!config_init("../config.ini"))
    {
        LOG_ERROR("TEST", "Echec du chargement de config.ini");
        return 1;
    }

    if (!config_init("config.ini"))
    {
        LOG_ERROR("CONFIG", "Unable to load configuration.");
        return EXIT_FAILURE;
    }

    debug_dump_config(config_get_runtime());

    LOG_INFO("TEST", "Tous les tests sont OK.");

    return 0;
}
