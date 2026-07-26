#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

/*==========================================================
 * Configuration
 *=========================================================*/

#define CONFIG_MAX_ENTRIES      128
#define CONFIG_SECTION_LENGTH    32
#define CONFIG_KEY_LENGTH        32
#define CONFIG_VALUE_LENGTH      64

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

} config_runtime_t;

/*==========================================================
 * API
 *=========================================================*/

bool config_init(const char *filename);

bool config_get_string(
        const char *section,
        const char *key,
        char *buffer,
        size_t buffer_size);

bool config_get_int(
        const char *section,
        const char *key,
        int *value);

bool config_get_float(
        const char *section,
        const char *key,
        float *value);

bool config_get_bool(
        const char *section,
        const char *key,
        bool *value);

const config_runtime_t *config_get_runtime(void);

#endif