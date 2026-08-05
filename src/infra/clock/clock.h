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
        uint32_t month;
        uint32_t day;
        uint32_t hour;
        uint32_t minute;
        uint32_t second;

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

    void clock_add_second(void);

    bool clock_sync_to_runtime(void);

    /*
     * Synchronise l'horloge NeoTherm
     * avec l'heure du système.
     *
     * PC      : heure Windows/Linux
     * ESP32   : remplacé par NTP + RTC
     */
    bool clock_sync_from_system(void);

    bool clock_sync_from_system(void);

    uint32_t clock_get_timestamp(void);

#ifdef __cplusplus
}
#endif

#endif