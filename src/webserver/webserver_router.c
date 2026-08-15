#include "webserver_router.h"

#ifdef _WIN32

#include <stddef.h>
#include <string.h>

#include "webserver_http.h"
#include "webserver_request.h"

#include "api/api_thermostat.h"
#include "api/api_history.h"
#include "api/api_system.h"
#include "api/api_weather.h"
#include "api/api_alarm.h"
#include "api/api_climate.h"
#include "api/api_config.h"
#include "api/api_schedule.h"
#include "api/api_relay.h"
#include "api/api_thermal.h"
#include "api/api_thermal_learning.h"
#include "api/api_thermal_prediction.h"
#include "api/api_clock.h"
#include "api/api_event.h"
#include "api/api_runtime.h"
#include "api/api_scheduler.h"
#include "api/api_storage.h"
#include "api/api_pages.h"
#include "webserver_static.h"

/*==========================================================
 * Types
 *=========================================================*/

typedef enum
{
    WEBSERVER_HANDLER_SIMPLE,
    WEBSERVER_HANDLER_REQUEST

} webserver_handler_type_t;

typedef struct
{
    const char *method;
    const char *path;

    webserver_handler_type_t type;

    union
    {
        void (*simple)(
            SOCKET client_socket);

        void (*request)(
            SOCKET client_socket,
            const char *request);

    } handler;

} webserver_route_t;

/*==========================================================
 * Table des routes
 *=========================================================*/

static const webserver_route_t s_routes[] =
    {
        // {"GET",
        //  "/",
        //  WEBSERVER_HANDLER_SIMPLE,
        //  {.simple =
        //       api_pages_handle_index}},

        {"GET",
         "/api/status",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermostat_handle_status}},

        {"GET",
         "/api/clock",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_clock_handle_status}},

        {"GET",
         "/api/thermostat/weather",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermostat_handle_weather}},

        {"GET",
         "/api/temperature",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermostat_handle_temperature}},

        {"GET",
         "/api/history",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_history_handle}},

        {"GET",
         "/api/weather",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_weather_handle}},

        {"GET",
         "/api/alarms",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_alarm_handle}},

        {"GET",
         "/api/climate",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_climate_handle}},

        {"POST",
         "/api/mode",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_thermostat_handle_mode}},

        {"GET",
         "/api/config",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_config_handle}},

        {"GET",
         "/api/schedule",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_schedule_handle}},

        {"POST",
         "/api/setpoint",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_thermostat_handle_setpoint}},

        {"POST",
         "/api/alarm/ack",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_alarm_handle_ack}},

        {"POST",
         "/api/alarm/clear",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_alarm_handle_clear}},

        {"POST",
         "/api/schedule",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_schedule_handle_set}},

        {"DELETE",
         "/api/schedule",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_schedule_handle_remove}},

        {"GET",
         "/api/relay",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_relay_handle_status}},

        {"POST",
         "/api/relay",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_relay_handle_set}},

        {"GET",
         "/api/thermal",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_handle_status}},

        {"GET",
         "/api/thermal/outside_temperature",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_handle_outside_temperature}},

        {"GET",
         "/api/thermal/heat_power",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_handle_heat_power}},

        {"GET",
         "/api/thermal/loss_factor",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_handle_loss_factor}},

        {"GET",
         "/api/thermal/mass",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_handle_mass}},

        {"POST",
         "/api/thermal/outside_temperature",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_thermal_handle_set_outside_temperature}},

        {"POST",
         "/api/thermal/heat_power",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_thermal_handle_set_heat_power}},

        {"POST",
         "/api/thermal/loss_factor",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_thermal_handle_set_loss_factor}},

        {"POST",
         "/api/thermal/mass",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_thermal_handle_set_mass}},

        {"GET",
         "/api/thermal/learning",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_learning_handle_status}},

        {"GET",
         "/api/thermal/learning/heat_rate",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_learning_handle_heat_rate}},

        {"GET",
         "/api/thermal/learning/cooling_rate",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_learning_handle_cooling_rate}},

        {"GET",
         "/api/thermal/learning/overshoot",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_learning_handle_overshoot}},

        {"GET",
         "/api/thermal/learning/valid",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_learning_handle_valid}},

        {"GET",
         "/api/thermal_prediction",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_prediction_handle_status}},

        {"GET",
         "/api/thermal_prediction/current",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_prediction_handle_current}},

        {"GET",
         "/api/thermal_prediction/next_tick",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_prediction_handle_next_tick}},

        {"GET",
         "/api/thermal_prediction/valid",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_prediction_handle_valid}},

        {"GET",
         "/api/thermal_prediction/heated",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_prediction_handle_heated}},

        {"GET",
         "/api/thermal_prediction/natural",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_thermal_prediction_handle_natural}},

        {"GET",
         "/api/events",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_event_handle_status}},

        {"POST",
         "/api/events",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_event_handle_post}},

        {"POST",
         "/api/events/dispatch",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_event_handle_dispatch}},

        {"GET",
         "/api/runtime",
         WEBSERVER_HANDLER_REQUEST,
         {.simple =
              api_runtime_handle_status}},

        {"POST",
         "/api/runtime",
         WEBSERVER_HANDLER_REQUEST,
         {.request =
              api_runtime_handle_update}},

        {
            "GET",
            "/api/scheduler",
            WEBSERVER_HANDLER_REQUEST,
            {.simple =
                 api_scheduler_handle_status},
        },

        {
            "POST",
            "/api/scheduler/enable",
            WEBSERVER_HANDLER_REQUEST,
            {.request =
                 api_scheduler_handle_enable},
        },

        {
            "POST",
            "/api/scheduler/disable",
            WEBSERVER_HANDLER_REQUEST,
            {.request =
                 api_scheduler_handle_disable},
        },

        {
            "POST",
            "/api/scheduler/remove",
            WEBSERVER_HANDLER_REQUEST,
            {.request =
                 api_scheduler_handle_remove},
        },

        {"GET",
         "/api/storage",
         WEBSERVER_HANDLER_REQUEST,
         {.simple = api_storage_handle_get}},

        {"POST",
         "/api/storage",
         WEBSERVER_HANDLER_REQUEST,
         {.request = api_storage_handle_post}},

        {"DELETE",
         "/api/storage",
         WEBSERVER_HANDLER_SIMPLE,
         {.simple =
              api_storage_handle_delete}},

};

#define WEBSERVER_ROUTE_COUNT \
    (sizeof(s_routes) / sizeof(s_routes[0]))

/*==========================================================
 * Comparaison route
 *=========================================================*/

static bool webserver_router_match(
    const webserver_route_t *route,
    const char *method,
    const char *path)
{
    if (route == NULL ||
        method == NULL ||
        path == NULL)
    {
        return false;
    }

    /*
     * Vérification de la méthode HTTP.
     */

    if (strcmp(
            route->method,
            method) != 0)
    {
        return false;
    }

    /*
     * Vérification du chemin.
     *
     * Exemple accepté :
     *
     * /api/history
     * /api/history?limit=10
     */

    size_t path_length =
        strlen(route->path);

    if (strncmp(
            path,
            route->path,
            path_length) != 0)
    {
        return false;
    }

    /*
     * Après le chemin attendu,
     * seul '?' ou '\0' est accepté.
     */

    if (path[path_length] != '\0' &&
        path[path_length] != '?')
    {
        return false;
    }

    return true;
}

/*==========================================================
 * Recherche d'une route
 *=========================================================*/

static const webserver_route_t *
webserver_router_find(
    const char *method,
    const char *path)
{
    if (method == NULL ||
        path == NULL)
    {
        return NULL;
    }

    for (size_t i = 0;
         i < WEBSERVER_ROUTE_COUNT;
         i++)
    {
        const webserver_route_t *route =
            &s_routes[i];

        if (webserver_router_match(
                route,
                method,
                path))
        {
            return route;
        }
    }

    return NULL;
}

/*==========================================================
 * Exécution d'une route
 *=========================================================*/

static void webserver_router_dispatch(
    const webserver_route_t *route,
    SOCKET client_socket,
    const char *request)
{
    if (route == NULL)
    {
        return;
    }

    switch (route->type)
    {
    case WEBSERVER_HANDLER_SIMPLE:

        if (route->handler.simple != NULL)
        {
            route->handler.simple(
                client_socket);
        }

        break;

    case WEBSERVER_HANDLER_REQUEST:

        if (route->handler.request != NULL)
        {
            route->handler.request(
                client_socket,
                request);
        }

        break;

    default:
        break;
    }
}

/*==========================================================
 * Routage HTTP
 *=========================================================*/

bool webserver_router_handle(
    SOCKET client_socket,
    const char *request)
{
    if (request == NULL)
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid request\"}");

        return false;
    }

    /*
     * Extraction méthode + chemin.
     */

    char method[16];
    char path[256];

    if (!webserver_request_parse(
            request,
            method,
            sizeof(method),
            path,
            sizeof(path)))
    {
        webserver_http_send_response(
            client_socket,
            400,
            "Bad Request",
            "application/json",
            "{\"error\":\"invalid request\"}");

        return false;
    }

    /*
     * Fichiers statiques.
     */

    if (strcmp(method, "GET") == 0 &&
        strncmp(path, "/api/", 5) != 0)
    {
        return webserver_static_handle(
            client_socket,
            path);
    }

    /*
     * Recherche de la route.
     */

    const webserver_route_t *route =
        webserver_router_find(
            method,
            path);

    if (route == NULL)
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "application/json",
            "{\"error\":\"not found\"}");

        return false;
    }

    /*
     * Exécution du handler.
     */

    webserver_router_dispatch(
        route,
        client_socket,
        request);

    return true;
}

#endif /* _WIN32 */
