#include "ini_internal.h"
#include <string.h>
#include <stdlib.h>

bool ini_get_string(const char *section, const char *key, char *buffer, size_t n)
{
    if (!section || !key || !buffer || n == 0)
        return false;

    for (size_t i = 0; i < s_runtime.count; i++)
    {
        const config_entry_t *e = &s_runtime.entries[i];
        if (strcmp(e->section, section) == 0 && strcmp(e->key, key) == 0)
        {
            strncpy(buffer, e->value, n - 1);
            buffer[n - 1] = '\0';
            return true;
        }
    }
    return false;
}

bool ini_get_int(const char *s, const char *k, int *v)
{
    char b[32];
    if (!v || !ini_get_string(s, k, b, sizeof(b)))
        return false;
    *v = (int)strtol(b, NULL, 10);
    return true;
}

bool ini_get_float(const char *s, const char *k, float *v)
{
    char b[32];
    if (!v || !ini_get_string(s, k, b, sizeof(b)))
        return false;
    *v = strtof(b, NULL);
    return true;
}

bool ini_get_bool(const char *s, const char *k, bool *v)
{
    char b[16];
    if (!v || !ini_get_string(s, k, b, sizeof(b)))
        return false;

    if (!strcmp(b, "true") || !strcmp(b, "TRUE") || !strcmp(b, "1") || !strcmp(b, "yes"))
    {
        *v = true;
        return true;
    }

    if (!strcmp(b, "false") || !strcmp(b, "FALSE") || !strcmp(b, "0") || !strcmp(b, "no"))
    {
        *v = false;
        return true;
    }

    return false;
}


bool ini_key_exists(
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
        const config_entry_t *e =
            &s_runtime.entries[i];

        if ((strcmp(e->section, section) == 0) &&
            (strcmp(e->key, key) == 0))
        {
            return true;
        }
    }


    return false;
}
