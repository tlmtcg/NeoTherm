#include "webserver_static.h"

#ifdef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#include "webserver_http.h"

static const char *webserver_static_content_type(
    const char *path)
{
    if (path == NULL)
    {
        return "application/octet-stream";
    }

    if (strstr(path, ".html") != NULL)
    {
        return "text/html";
    }

    if (strstr(path, ".css") != NULL)
    {
        return "text/css";
    }

    if (strstr(path, ".js") != NULL)
    {
        return "application/javascript";
    }

    if (strstr(path, ".json") != NULL)
    {
        return "application/json";
    }

    return "application/octet-stream";
}

bool webserver_static_handle(
    SOCKET client_socket,
    const char *path)
{
    if (path == NULL)
    {
        return false;
    }

    const char *file_path;

    if (strcmp(path, "/") == 0 ||
        strcmp(path, "/index.html") == 0)
    {
        file_path = "../web/index.html";
    }
    else if (strcmp(path, "/css/style.css") == 0)
    {
        file_path = "../web/css/style.css";
    }
    else if (strcmp(path, "/js/app.js") == 0)
    {
        file_path = "../web/js/app.js";
    }
    else
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "text/plain",
            "File not found");

        return false;
    }

    FILE *fp = fopen(
        file_path,
        "rb");

    if (fp == NULL)
    {
        webserver_http_send_response(
            client_socket,
            404,
            "Not Found",
            "text/plain",
            "File not found");

        return false;
    }

    fseek(fp, 0, SEEK_END);

    long size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    if (size < 0)
    {
        fclose(fp);

        return false;
    }

    char *buffer =
        malloc((size_t)size + 1);

    if (buffer == NULL)
    {
        fclose(fp);

        return false;
    }

    size_t read =
        fread(
            buffer,
            1,
            (size_t)size,
            fp);

    fclose(fp);

    buffer[read] = '\0';

    webserver_http_send_response(
        client_socket,
        200,
        "OK",
        webserver_static_content_type(file_path),
        buffer);

    free(buffer);

    return true;
}

#endif /* _WIN32 */
