#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#include "thermostat.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*==========================================================
     * Configuration runtime persistante
     *=========================================================*/

    typedef struct
    {
        thermostat_mode_t mode;

        float setpoint;

        float hysteresis;

        uint32_t relay_delay;

        float latitude;

        float longitude;

    } runtime_config_t;

    typedef enum
    {
        STORAGE_LOAD_OK = 0,
        STORAGE_LOAD_DEFAULT = 1,
        STORAGE_LOAD_ERROR = -1

    } storage_load_result_t;

    /*==========================================================
     * Initialisation
     *=========================================================*/

    /**
     * @brief Initialise le runtime.
     *
     * Charge la configuration sauvegardée.
     * Si aucune configuration n'existe,
     * les valeurs par défaut sont conservées.
     *
     * @return true si initialisation réussie.
     */
    bool runtime_init(void);

    /*==========================================================
     * Accès configuration
     *=========================================================*/

    /**
     * @brief Retourne la configuration runtime courante.
     *
     * Le pointeur retourné est en lecture seule.
     *
     * @return Pointeur vers la configuration.
     */
    const runtime_config_t *runtime_get(void);

    /*==========================================================
     * Modification configuration
     *=========================================================*/

    bool runtime_set_mode(
        thermostat_mode_t mode);

    bool runtime_set_setpoint(
        float value);

    bool runtime_set_hysteresis(
        float value);

    bool runtime_set_relay_delay(
        uint32_t seconds);

    bool runtime_set_location(
        float latitude,
        float longitude);

    /*==========================================================
     * Sauvegarde / chargement
     *=========================================================*/

    /**
     * @brief Sauvegarde la configuration runtime.
     *
     * @return true si sauvegarde réussie.
     */
    bool runtime_save(void);

    /**
     * @brief Recharge la configuration depuis le stockage.
     *
     * @return true si chargement réussi.
     */
    bool runtime_load(void);

    /*
     * Configuration par défaut
     */
    extern const runtime_config_t runtime_default_config;

#ifdef __cplusplus
}
#endif

#endif /* RUNTIME_H */
