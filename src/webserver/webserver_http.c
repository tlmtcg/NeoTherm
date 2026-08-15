#include "webserver_http.h"

#ifdef _WIN32

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define WEBSERVER_HTTP_BUFFER_SIZE 65536

/*==========================================================
 * Envoi complet d'un buffer TCP
 *=========================================================*/

static bool webserver_http_send_all(
    SOCKET client_socket,
    const char *buffer,
    size_t length)
{
    if (buffer == NULL)
    {
        return false;
    }

    size_t total_sent = 0;

    while (total_sent < length)
    {
        int sent =
            send(
                client_socket,
                buffer + total_sent,
                (int)(length - total_sent),
                0);

        if (sent == SOCKET_ERROR ||
            sent <= 0)
        {
            return false;
        }

        total_sent +=
            (size_t)sent;
    }

    return true;
}

/*==========================================================
 * Réponse HTTP
 *=========================================================*/

void webserver_http_send_response(
    SOCKET client_socket,
    int status_code,
    const char *status_text,
    const char *content_type,
    const char *body)
{
    /*
     * Valeurs par défaut.
     */

    if (status_text == NULL)
    {
        status_text = "";
    }

    if (content_type == NULL)
    {
        content_type = "text/plain";
    }

    if (body == NULL)
    {
        body = "";
    }

    /*
     * Taille du corps.
     */

    size_t body_length =
        strlen(body);

    /*
     * Construction de la réponse HTTP.
     */

    char response[
        WEBSERVER_HTTP_BUFFER_SIZE];

    int length =
        snprintf(
            response,
            sizeof(response),

            "HTTP/1.1 %d %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",

            status_code,
            status_text,
            content_type,
            body_length,
            body);

    /*
     * Erreur de construction.
     */

    if (length < 0)
    {
        return;
    }

    /*
     * Réponse trop grande.
     */

    if ((size_t)length >=
        sizeof(response))
    {
        return;
    }

    /*
     * Envoi complet.
     */

    webserver_http_send_all(
        client_socket,
        response,
        (size_t)length);
}

#endif /* _WIN32 */
