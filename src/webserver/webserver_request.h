#ifndef WEBSERVER_REQUEST_H
#define WEBSERVER_REQUEST_H

#include <stdbool.h>
#include <stddef.h>

#ifdef _WIN32

#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================
 * Parsing de la ligne HTTP
 *=========================================================*/

bool webserver_request_parse(
    const char *request,
    char *method,
    size_t method_size,
    char *path,
    size_t path_size);

/*==========================================================
 * Extraction du corps HTTP
 *=========================================================*/

const char *webserver_request_get_body(
    const char *request);

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

#endif /* WEBSERVER_REQUEST_H */
