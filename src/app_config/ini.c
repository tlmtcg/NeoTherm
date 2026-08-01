#include "ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "logger.h"

static ini_runtime_t s_runtime;
static char s_current_section[CONFIG_SECTION_LENGTH];

static bool ini_add_entry(const char *section, const char *key, const char *value)
{
    if (!section || !key || !value)
        return false;
    if (s_runtime.count >= CONFIG_MAX_ENTRIES)
        return false;

    config_entry_t *e = &s_runtime.entries[s_runtime.count++];

    strncpy(e->section, section, CONFIG_SECTION_LENGTH - 1);
    e->section[CONFIG_SECTION_LENGTH - 1] = '\0';

    strncpy(e->key, key, CONFIG_KEY_LENGTH - 1);
    e->key[CONFIG_KEY_LENGTH - 1] = '\0';

    strncpy(e->value, value, CONFIG_VALUE_LENGTH - 1);
    e->value[CONFIG_VALUE_LENGTH - 1] = '\0';

    return true;
}

static char *trim(char *str)
{
    while (isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str == '\0')
    {
        return str;
    }

    char *end = str + strlen(str) - 1;

    while ((end > str) &&
           isspace((unsigned char)*end))
    {
        end--;
    }

    end[1] = '\0';

    return str;
}

static bool ini_parse_line(char *line)
{
    line[strcspn(line, "\r\n")] = '\0';

    line = trim(line); // <-- trim de la ligne complète

    if (line[0] == '\0' || line[0] == '#' || line[0] == ';')
        return true;

    if (line[0] == '[')
    {
        char *end = strchr(line, ']');
        if (!end)
            return false;

        *end = '\0';

        char *section = trim(line + 1); // <-- trim de la section

        strncpy(s_current_section,
                section,
                sizeof(s_current_section) - 1);

        s_current_section[sizeof(s_current_section) - 1] = '\0';

        return true;
    }

    char *eq = strchr(line, '=');

    if (!eq)
        return false;

    if (s_current_section[0] == '\0')
        return false;

    *eq = '\0';

    char *key = trim(line);     // <-- trim de la clé
    char *value = trim(eq + 1); // <-- trim de la valeur

    return ini_add_entry(
        s_current_section,
        key,
        value);
}

bool ini_init(const char *filename)
{
    if (!filename)
        return false;

    memset(&s_runtime, 0, sizeof(s_runtime));

    strncpy(s_runtime.filename,
            filename,
            sizeof(s_runtime.filename) - 1);

    s_runtime.filename[sizeof(s_runtime.filename) - 1] = '\0';

    s_current_section[0] = '\0';

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        LOG_ERROR("INI", "Unable to open %s", filename);
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (!ini_parse_line(line))
        {
            fclose(fp);
            LOG_ERROR("INI", "Parse error on line: %s", line);
            return false;
        }
    }

    fclose(fp);
    s_runtime.initialized = true;
    return true;
}

static bool ini_save(void)
{
    FILE *fp = fopen(s_runtime.filename, "w");

    if (fp == NULL)
    {
        return false;
    }

    const char *current_section = "";

    for (size_t i = 0; i < s_runtime.count; i++)
    {
        const config_entry_t *e = &s_runtime.entries[i];

        if (strcmp(current_section, e->section) != 0)
        {
            fprintf(fp, "\n[%s]\n", e->section);

            current_section = e->section;
        }

        fprintf(fp,
                "%s=%s\n",
                e->key,
                e->value);
    }

    fclose(fp);

    return true;
}

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

const ini_runtime_t *ini_get_runtime(void)
{
    return &s_runtime;
}
