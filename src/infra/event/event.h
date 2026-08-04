#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <stdint.h>

#define EVENT_QUEUE_SIZE 32

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

    EVENT_RELAY_ON,

    EVENT_RELAY_OFF,

    EVENT_MODE_CHANGED,

    EVENT_SETPOINT_CHANGED,

    EVENT_TIMER_1S,

    EVENT_QUIT,

    EVENT_SENSOR_DHT,

    EVENT_SENSOR_SHT31,

    EVENT_SENSOR_ERROR_DHT,

    EVENT_SENSOR_ERROR_SHT31,

    EVENT_MODE_CHANGE_REQUEST,

    EVENT_MANUAL_SETPOINT_REQUEST,

    EVENT_WEATHER_UPDATE,

    EVENT_NET_TIME_SYNCED,

    EVENT_RELAY_CHANGED,

    EVENT_HISTORY_SAVE,

    EVENT_STORAGE_SAVE,

    EVENT_COUNT

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

    void event_dump(void);

    const char *event_name(event_type_t type);

    uint32_t event_get_count(event_type_t type);

    uint32_t event_get_total_count(void);

    const char *event_name(event_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_H */