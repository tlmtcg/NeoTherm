#include "ini_internal.h"
#include <string.h>
#include <stdio.h>

bool ini_set_string(
    const char *section,
    const char *key,
    const char *value)
{
    if ((section == NULL) ||
        (key == NULL) ||
        (value == NULL))
    {
        return false;
    }

    for (size_t i = 0; i < s_runtime.count; i++)
    {
        config_entry_t *e = &s_runtime.entries[i];

        if ((strcmp(e->section, section) == 0) &&
            (strcmp(e->key, key) == 0))
        {
            strncpy(e->value,
                    value,
                    CONFIG_VALUE_LENGTH - 1);

            e->value[CONFIG_VALUE_LENGTH - 1] = '\0';

            return ini_save();
        }
    }

    if (!ini_add_entry(section, key, value))
    {
        return false;
    }

    return ini_save();
}

bool ini_set_int(
    const char *section,
    const char *key,
    int value)
{
    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "%d",
        value);

    return ini_set_string(
        section,
        key,
        buffer);
}

bool ini_set_bool(
    const char *section,
    const char *key,
    bool value)
{
    return ini_set_string(
        section,
        key,
        value ? "true" : "false");
}

bool ini_set_float(
    const char *section,
    const char *key,
    float value)
{
    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "%.6f",
        value);

    return ini_set_string(
        section,
        key,
        buffer);
}

bool ini_remove_key(
    const char *section,
    const char *key)
{
    if ((section == NULL) ||
        (key == NULL))
    {
        return false;
    }

    for (size_t i = 0; i < s_runtime.count; i++)
    {
        config_entry_t *e = &s_runtime.entries[i];

        if ((strcmp(e->section, section) == 0) &&
            (strcmp(e->key, key) == 0))
        {
            /*
             * Décalage des entrées suivantes
             */

            for (size_t j = i; j < s_runtime.count - 1; j++)
            {
                s_runtime.entries[j] =
                    s_runtime.entries[j + 1];
            }

            s_runtime.count--;

            /*
             * Sauvegarde du fichier
             */

            return ini_save();
        }
    }

    /*
     * Clé inexistante
     */

    return false;
}
