#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>
#include <stdint.h>

#define SECONDS_PER_DAY 86400U

#ifdef __cplusplus
extern "C"
{
#endif

    /*==========================================================
     * Initialisation
     *=========================================================*/

    bool relay_init(void);

    /*==========================================================
     * Commande
     *=========================================================*/

    /**
     * @brief Demande un changement d'état du relais.
     *
     * Le changement est refusé si le délai minimum
     * entre deux commutations n'est pas écoulé.
     *
     * @param state Nouvel état demandé.
     *
     * @return true si le relais a changé d'état,
     *         false si la commutation a été refusée.
     */
    bool relay_set(bool state);

    /**
     * @brief Inverse l'état du relais.
     *
     * @return true si la commutation a été effectuée.
     */
    bool relay_toggle(void);

    /*==========================================================
     * Lecture
     *=========================================================*/

    bool relay_get(void);

    typedef struct
    {
        bool state;

        bool can_switch;

        uint32_t switch_count;

        uint32_t last_switch_time;

        uint32_t min_switch_delay;

        uint32_t elapsed_delay;

        uint32_t remaining_delay;

    } relay_status_t;

    void relay_get_status(relay_status_t *status);

    /**
     * @brief Retourne le nombre de changements d'état du relais.
     *
     * Le compteur est incrémenté uniquement lors d'une vraie
     * commutation OFF->ON ou ON->OFF.
     */
    uint32_t relay_get_switch_count(void);

    uint32_t relay_get_elapsed_delay(void);
    uint32_t relay_get_remaining_delay(void);

    /*==========================================================
     * Tests
     *=========================================================*/

    /**
     * @brief Remise à zéro du compteur.
     *
     * Utilisé uniquement par les tests unitaires.
     */
    void relay_reset_switch_count(void);

    /*==========================================================
     * Protection anti-cycles courts
     *=========================================================*/

    /**
     * @brief Vérifie si une nouvelle commutation est autorisée.
     */
    bool relay_can_switch(void);

    /**
     * @brief Définit le délai minimum entre deux commutations.
     *
     * @param seconds Délai en secondes.
     */
    void relay_set_min_switch_delay(uint32_t seconds);

    /**
     * @brief Retourne le délai minimum configuré.
     *
     * @return Délai en secondes.
     */
    uint32_t relay_get_min_switch_delay(void);

    /**
     * @brief Centralise toute la configuration spécifique aux tests
     *
     */
    void relay_test_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* RELAY_H */
