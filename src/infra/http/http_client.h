#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdbool.h>
#include <stddef.h>

/*==========================================================
 * Initialisation
 *=========================================================*/

/**
 * @brief Initialise le client HTTP.
 *
 * Sur PC :
 *  - préparation du client HTTP
 *
 * Sur ESP32 :
 *  - initialisation esp_http_client
 *
 * @return true si initialisation réussie.
 */
bool http_client_init(void);


/*==========================================================
 * Requête HTTP GET
 *=========================================================*/

/**
 * @brief Effectue une requête HTTP GET.
 *
 * @param url URL complète à interroger.
 * @param buffer Buffer de réception.
 * @param buffer_size Taille du buffer.
 *
 * @return true si la réponse HTTP est valide.
 */
bool http_client_get(
    const char *url,
    char *buffer,
    size_t buffer_size);


/*==========================================================
 * Etat
 *=========================================================*/

/**
 * @brief Indique si le client HTTP est disponible.
 */
bool http_client_is_available(void);


/*==========================================================
 * Debug
 *=========================================================*/

/**
 * @brief Affiche l'état du client HTTP.
 */
void http_client_dump(void);


#endif /* HTTP_CLIENT_H */
