#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint32_t year;
        uint8_t month;
        uint8_t day;

        uint8_t hour;
        uint8_t minute;
        uint8_t second;

    } clock_time_t;

    /*
     * Initialisation horloge
     */
    void clock_init(void);

    /*
     * Avance l'horloge virtuelle
     * delta en secondes
     */
    void clock_tick(uint32_t seconds);

    /*
     * Lecture heure courante
     */
    bool clock_get_time(
        clock_time_t *time);

    /*
     * Mise à heure
     */
    bool clock_set_time(
        const clock_time_t *time);

    /*
     * Affichage debug
     */
    void clock_dump(void);

    void clock_add_seconds(uint32_t seconds);

    uint32_t clock_seconds_today(void);

#ifdef __cplusplus
}
#endif

#endif