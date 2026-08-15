#include "api_config.h"

#ifdef _WIN32

#include <stdio.h>

#include "app_config.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/config
 *=========================================================*/

void api_config_handle(
    SOCKET client_socket)
{
    const app_config_t *cfg =
        app_config_get();

    if (cfg == NULL)
    {
        webserver_http_send_response(
            client_socket,
            500,
            "Internal Server Error",
            "application/json",
            "{\"error\":\"configuration unavailable\"}");

        return;
    }

    char body[2048];

    snprintf(
        body,
        sizeof(body),

        "{"
        "\"logger_level\":\"%s\","
        "\"relay_gpio\":%u,"
        "\"relay_default_switch_delay\":%u,"
        "\"climate_period\":%u,"
        "\"thermostat_period\":%u,"
        "\"history_save_period\":%u,"
        "\"history_csv_period\":%u,"
        "\"history_max_records\":%u,"
        "\"schedule_max_points\":%u,"
        "\"schedule_default_setpoint\":%.2f,"
        "\"climate_initial_temperature\":%.2f,"
        "\"debug_enabled\":%s,"
        "\"debug_dump_config\":%s,"
        "\"thermal_outside_temperature\":%.2f,"
        "\"thermal_heat_power\":%.2f,"
        "\"thermal_loss_factor\":%.4f,"
        "\"thermal_mass\":%.2f"
        "}",

        cfg->logger_level,

        cfg->relay_gpio,
        cfg->relay_default_switch_delay,

        cfg->climate_period,
        cfg->thermostat_period,
        cfg->history_save_period,
        cfg->history_csv_period,

        cfg->history_max_records,

        cfg->schedule_max_points,
        cfg->schedule_default_setpoint,

        cfg->climate_initial_temperature,

        cfg->debug_enabled
            ? "true"
            : "false",

        cfg->debug_dump_config
            ? "true"
            : "false",

        cfg->thermal_outside_temperature,
        cfg->thermal_heat_power,
        cfg->thermal_loss_factor,
        cfg->thermal_mass);

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        body);
}

#endif /* _WIN32 */
