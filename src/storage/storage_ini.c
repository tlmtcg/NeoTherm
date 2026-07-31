#include "storage_ini.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>



/*==========================================================
 * Fonction interne
 *=========================================================*/

static bool storage_ini_find(
    FILE *fp,
    const char *key,
    char *value,
    size_t value_size)
{
    char line[STORAGE_LINE_SIZE];

    rewind(fp);

    while (fgets(line, sizeof(line), fp))
    {
        char *p = line;

        /* ignorer espaces */

        while (*p == ' ' || *p == '\t')
        {
            p++;
        }

        /* commentaires */

        if (*p == ';' || *p == '#')
        {
            continue;
        }

        /* section */

        if (*p == '[')
        {
            continue;
        }

        char *equal = strchr(p, '=');

        if (equal == NULL)
        {
            continue;
        }

        *equal = '\0';

        char *key_name = p;
        char *key_value = equal + 1;

        key_name[strcspn(key_name, "\r\n")] = '\0';
        key_value[strcspn(key_value, "\r\n")] = '\0';

        if (strcmp(key_name, key) == 0)
        {
            strncpy(value,
                    key_value,
                    value_size - 1);

            value[value_size - 1] = '\0';

            return true;
        }
    }

    return false;
}

/*==========================================================
 * Lecture
 *=========================================================*/

bool storage_ini_read_string(
    FILE *fp,
    const char *key,
    char *value,
    size_t value_size)
{
    return storage_ini_find(
        fp,
        key,
        value,
        value_size);
}

bool storage_ini_read_float(
    FILE *fp,
    const char *key,
    float *value)
{
    char text[64];

    if (!storage_ini_find(
            fp,
            key,
            text,
            sizeof(text)))
    {
        return false;
    }

    *value = strtof(text, NULL);

    return true;
}

bool storage_ini_read_uint32(
    FILE *fp,
    const char *key,
    uint32_t *value)
{
    char text[64];

    if (!storage_ini_find(
            fp,
            key,
            text,
            sizeof(text)))
    {
        return false;
    }

    *value = (uint32_t)strtoul(
        text,
        NULL,
        10);

    return true;
}

/*==========================================================
 * Ecriture
 *=========================================================*/

bool storage_ini_write_string(
    FILE *fp,
    const char *key,
    const char *value)
{
    return fprintf(fp,
                   "%s=%s\n",
                   key,
                   value) > 0;
}

bool storage_ini_write_float(
    FILE *fp,
    const char *key,
    float value)
{
    return fprintf(fp,
                   "%s=%.3f\n",
                   key,
                   value) > 0;
}

bool storage_ini_write_uint32(
    FILE *fp,
    const char *key,
    uint32_t value)
{
    return fprintf(fp,
                   "%s=%u\n",
                   key,
                   value) > 0;
}
