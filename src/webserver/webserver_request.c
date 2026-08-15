#include "webserver_request.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

/*==========================================================
 * Parsing de la ligne HTTP
 *
 * Exemple :
 *
 * GET /api/status HTTP/1.1
 *
 * devient :
 *
 * method = "GET"
 * path   = "/api/status"
 *=========================================================*/

bool webserver_request_parse(
    const char *request,
    char *method,
    size_t method_size,
    char *path,
    size_t path_size)
{
    if (request == NULL ||
        method == NULL ||
        path == NULL ||
        method_size == 0 ||
        path_size == 0)
    {
        return false;
    }

    /*
     * Valeurs par défaut.
     */

    method[0] = '\0';
    path[0] = '\0';

    /*
     * Lecture de :
     *
     * METHOD PATH HTTP_VERSION
     *
     * Les tailles correspondent aux buffers
     * utilisés par le router :
     *
     * method[16]
     * path[256]
     */

    if (method_size < 16 ||
        path_size < 256)
    {
        return false;
    }

    int result =
        sscanf(
            request,
            "%15s %255s",
            method,
            path);

    if (result != 2)
    {
        return false;
    }

    /*
     * Sécurité des chaînes.
     */

    method[method_size - 1] = '\0';
    path[path_size - 1] = '\0';

    return true;
}

/*==========================================================
 * Extraction du corps HTTP
 *
 * Exemple :
 *
 * POST /api/mode HTTP/1.1
 *
 * ...
 *
 * {"mode":"AUTO"}
 *
 * retourne un pointeur vers :
 *
 * {"mode":"AUTO"}
 *=========================================================*/

const char *webserver_request_get_body(
    const char *request)
{
    if (request == NULL)
    {
        return NULL;
    }

    const char *body =
        strstr(
            request,
            "\r\n\r\n");

    if (body == NULL)
    {
        return NULL;
    }

    return body + 4;
}

#endif /* _WIN32 */
