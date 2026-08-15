#include "api_history.h"

#ifdef _WIN32

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "history.h"
#include "thermostat.h"
#include "webserver_http.h"

/*==========================================================
 * GET /api/history
 *=========================================================*/

void api_history_handle(
    SOCKET client_socket,
    const char *request)
{
    uint32_t count =
        history_count();

    /*
     * Limite par défaut.
     */

    uint32_t limit = 20;

    /*
     * Recherche éventuelle de ?limit=
     */

    if (request != NULL)
    {
        const char *limit_param =
            strstr(
                request,
                "limit=");

        if (limit_param != NULL)
        {
            int value = 0;

            if (sscanf(
                    limit_param,
                    "limit=%d",
                    &value) == 1)
            {
                if (value > 0)
                {
                    limit =
                        (uint32_t)value;
                }
            }
        }
    }

    /*
     * Limite à la taille réelle
     * de l'historique.
     */

    if (limit > count)
    {
        limit = count;
    }

    if (limit > HISTORY_SIZE)
    {
        limit = HISTORY_SIZE;
    }

    /*
     * Buffer JSON.
     */

    static char response[65536];

    size_t offset = 0;

    int written =
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "{\"count\":%u,\"records\":[",
            limit);

    if (written < 0)
    {
        return;
    }

    offset +=
        (size_t)written;

    /*
     * Premier index à envoyer.
     */

    uint32_t start = 0;

    if (count > limit)
    {
        start =
            count - limit;
    }

    /*
     * Enregistrements.
     */

    for (uint32_t i = start;
         i < count;
         i++)
    {
        history_record_t record;

        if (!history_get(
                i,
                &record))
        {
            continue;
        }

        if (offset >=
            sizeof(response))
        {
            break;
        }

        /*
         * Séparateur JSON.
         */

        if (i > start)
        {
            written =
                snprintf(
                    response + offset,
                    sizeof(response) - offset,
                    ",");

            if (written < 0)
            {
                break;
            }

            offset +=
                (size_t)written;
        }

        /*
         * Mode.
         */

        const char *mode =
            thermostat_mode_to_string(
                record.mode);

        if (mode == NULL)
        {
            mode = "UNKNOWN";
        }

        /*
         * Enregistrement JSON.
         */

        written =
            snprintf(
                response + offset,
                sizeof(response) - offset,

                "{"
                "\"tick\":%u,"
                "\"time\":\"%02d:%02d:%02d\","
                "\"temperature\":%.2f,"
                "\"outside_temperature\":%.2f,"
                "\"setpoint\":%.2f,"
                "\"mode\":\"%s\","
                "\"relay\":%s,"
                "\"heating\":%s"
                "}",

                record.tick,

                record.timestamp.hour,
                record.timestamp.minute,
                record.timestamp.second,

                record.inside_temperature,

                record.outside_temperature,

                record.setpoint,

                mode,

                record.relay
                    ? "true"
                    : "false",

                record.heating
                    ? "true"
                    : "false");

        if (written < 0)
        {
            break;
        }

        offset +=
            (size_t)written;
    }

    /*
     * Fin du tableau JSON.
     */

    if (offset <
        sizeof(response))
    {
        snprintf(
            response + offset,
            sizeof(response) - offset,
            "]}");
    }
    else
    {
        response[
            sizeof(response) - 1] =
            '\0';
    }

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        "application/json",
        response);
}

#endif /* _WIN32 */
