#include "http_client.h"

#include <stdio.h>
#include <string.h>

#include <curl/curl.h>

#include "logger.h"


static bool s_initialized = false;


/*==========================================================
 * Buffer CURL
 *=========================================================*/

typedef struct
{
    char *buffer;
    size_t size;
    size_t capacity;

} http_buffer_t;



static size_t http_write_callback(
        void *contents,
        size_t size,
        size_t nmemb,
        void *userp)
{
    size_t total = size * nmemb;

    http_buffer_t *response =
        (http_buffer_t *)userp;


    if ((response->size + total) >= response->capacity)
    {
        LOG_ERROR("HTTP",
                  "Response buffer too small");

        return 0;
    }


    memcpy(response->buffer + response->size,
           contents,
           total);


    response->size += total;

    response->buffer[response->size] = '\0';


    return total;
}


/*==========================================================
 * Initialisation
 *=========================================================*/

bool http_client_init(void)
{
    CURLcode result =
        curl_global_init(
            CURL_GLOBAL_DEFAULT);


    if (result != CURLE_OK)
    {
        LOG_ERROR("HTTP",
                  "CURL initialization failed");

        return false;
    }


    s_initialized = true;


    LOG_INFO("HTTP",
             "HTTP client initialized");


    return true;
}


/*==========================================================
 * GET
 *=========================================================*/

bool http_client_get(
        const char *url,
        char *buffer,
        size_t buffer_size)
{
    if (!s_initialized)
    {
        LOG_ERROR("HTTP",
                  "HTTP client not initialized");

        return false;
    }


    if ((url == NULL) ||
        (buffer == NULL) ||
        (buffer_size == 0))
    {
        return false;
    }


    CURL *curl =
        curl_easy_init();


    if (curl == NULL)
    {
        return false;
    }


    http_buffer_t response =
    {
        .buffer = buffer,
        .size = 0,
        .capacity = buffer_size
    };


    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url);


    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        http_write_callback);


    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &response);


    /*
     * HTTPS
     */
    curl_easy_setopt(
        curl,
        CURLOPT_SSL_VERIFYPEER,
        1L);


    curl_easy_setopt(
        curl,
        CURLOPT_SSL_VERIFYHOST,
        2L);


    /*
     * Timeout
     */
    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        10L);



    CURLcode result =
        curl_easy_perform(curl);



    curl_easy_cleanup(curl);



    if (result != CURLE_OK)
    {
        LOG_ERROR("HTTP",
                  "GET failed : %s",
                  curl_easy_strerror(result));

        return false;
    }


    LOG_INFO("HTTP",
             "GET OK (%zu bytes)",
             response.size);


    return true;
}


/*==========================================================
 * Etat
 *=========================================================*/

bool http_client_is_available(void)
{
    return s_initialized;
}


/*==========================================================
 * Debug
 *=========================================================*/

void http_client_dump(void)
{
    LOG_INFO("HTTP",
             "HTTP client : %s",
             s_initialized ? "READY" : "OFF");
}
