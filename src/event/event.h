#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*==========================================================
     * Types
     *=========================================================*/

    typedef enum
    {
        EVENT_NONE = 0,

        EVENT_CLIMATE_UPDATE,

        EVENT_TIMER_1S,

        EVENT_QUIT

    } event_type_t;

    typedef union
    {
        float temperature;
        int32_t value;
        void *ptr;

    } event_data_t;

    typedef struct
    {
        event_type_t type;
        event_data_t data;

    } event_t;

    /*==========================================================
     * API
     *=========================================================*/

    void event_init(void);

    bool event_post(const event_t *event);

    bool event_get(event_t *event);

    bool event_is_empty(void);

    bool event_is_full(void);

    void event_clear(void);

    const char *event_type_to_string(event_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_H */