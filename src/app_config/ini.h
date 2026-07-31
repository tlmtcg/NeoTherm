#ifndef INI_H
#define INI_H

#include <stdbool.h>
#include <stddef.h>

/*==========================================================
 * Configuration
 *=========================================================*/

#define CONFIG_MAX_ENTRIES       128
#define CONFIG_SECTION_LENGTH     32
#define CONFIG_KEY_LENGTH         32
#define CONFIG_VALUE_LENGTH       64

/*==========================================================
 * Types
 *=========================================================*/

typedef struct
{
    char section[CONFIG_SECTION_LENGTH];
    char key[CONFIG_KEY_LENGTH];
    char value[CONFIG_VALUE_LENGTH];

} config_entry_t;

typedef struct
{
    config_entry_t entries[CONFIG_MAX_ENTRIES];

    size_t count;

    bool initialized;

} ini_runtime_t;

/*==========================================================
 * API
 *=========================================================*/

/*
 * Charge un fichier INI.
 */
bool ini_init(const char *filename);

/*
 * Lecture d'une valeur sous forme de chaîne.
 */
bool ini_get_string(
    const char *section,
    const char *key,
    char *buffer,
    size_t buffer_size);

/*
 * Lecture d'un entier.
 */
bool ini_get_int(
    const char *section,
    const char *key,
    int *value);

/*
 * Lecture d'un flottant.
 */
bool ini_get_float(
    const char *section,
    const char *key,
    float *value);

/*
 * Lecture d'un booléen.
 */
bool ini_get_bool(
    const char *section,
    const char *key,
    bool *value);

/*
 * Accès à la structure interne (debug uniquement).
 */
const ini_runtime_t *ini_get_runtime(void);

#endif /* INI_H */
