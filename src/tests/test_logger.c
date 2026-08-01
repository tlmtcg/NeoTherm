#include <stdio.h>

#include "logger.h"
#include "test_utils.h"

bool test_logger_run(void)
{
    printf("\n================ LOGGER TEST ================\n");

    /*
     * Initialisation
     */

    ASSERT_TRUE(
        logger_init());



    /*
     * Tous les niveaux de log
     */

    logger_log(
        LOG_LEVEL_DEBUG,
        "TEST",
        "Message DEBUG");

    logger_log(
        LOG_LEVEL_INFO,
        "TEST",
        "Message INFO");

    logger_log(
        LOG_LEVEL_WARN,
        "TEST",
        "Message WARN");

    logger_log(
        LOG_LEVEL_ERROR,
        "TEST",
        "Message ERROR");



    /*
     * Formatage
     */

    logger_log(
        LOG_LEVEL_INFO,
        "FORMAT",
        "Valeur=%d Texte=%s Float=%.1f",
        42,
        "Bonjour",
        3.5);



    /*
     * Niveau inconnu
     */

    logger_log(
        (log_level_t)255,
        "TEST",
        "Unknown level");



    printf("PASS : Logger\n");

    return true;
}