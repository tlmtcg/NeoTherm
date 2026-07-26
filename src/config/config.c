#include "config.h"
#include <string.h>
#include <stdio.h>

static config_runtime_t s_runtime;

static char s_current_section[CONFIG_SECTION_LENGTH];

static bool config_add_entry(
    const char *section,
    const char *key,
    const char *value)
{
    if (section == NULL || key == NULL || value == NULL)
    {
        return false;
    }

    if (s_runtime.count >= CONFIG_MAX_ENTRIES)
    {
        printf("Erreur : tableau de configuration plein\n");
        return false;
    }

    config_entry_t *entry = &s_runtime.entries[s_runtime.count];

    strncpy(entry->section, section, sizeof(entry->section) - 1);
    entry->section[sizeof(entry->section) - 1] = '\0';

    strncpy(entry->key, key, sizeof(entry->key) - 1);
    entry->key[sizeof(entry->key) - 1] = '\0';

    strncpy(entry->value, value, sizeof(entry->value) - 1);
    entry->value[sizeof(entry->value) - 1] = '\0';

    s_runtime.count++;

    return true;
}

static bool config_parse_line(char *line)
{
    if (line == NULL)
    {
        return false;
    }

    /* TODO */

    line[strcspn(line, "\r\n")] = '\0';
    if (line[0] == '\0')
    {
        return true;
    }
    if (line[0] == '#')
    {
        return true;
    }

    if (line[0] == ';')
    {
        return true;
    }
    if (line[0] == '[')
    {
        char *end = strchr(line, ']');

        if (end == NULL)
        {
            return false;
        }

        *end = '\0';

        char *section_name = line + 1;

        printf("SECTION : %s\n", section_name);

        strncpy(s_current_section,
                section_name,
                sizeof(s_current_section));

        return true;
    }
    char *equal = strchr(line, '=');
    if (equal == NULL)
    {
        return false;
    }
    *equal = '\0';
    char *key = line;
    char *value = equal + 1;
    if (!config_add_entry(s_current_section, key, value))
    {
        printf("Impossible d'ajouter l'entrée\n");
        return false;
    }
    return true;
}

bool config_init(const char *filename)
{
    s_runtime.count = 0;
    s_runtime.initialized = true;
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        printf("Lecture : %s\n", line);

        config_parse_line(line);
    }

    fclose(fp);
    return true;
}

bool config_get_string(
    const char *section,
    const char *key,
    char *buffer,
    size_t buffer_size)
{
    (void)section;
    (void)key;
    (void)buffer;
    (void)buffer_size;

    return false;
}

bool config_get_int(
    const char *section,
    const char *key,
    int *value)
{
    (void)section;
    (void)key;
    (void)value;

    return false;
}

bool config_get_float(
    const char *section,
    const char *key,
    float *value)
{
    (void)section;
    (void)key;
    (void)value;

    return false;
}

bool config_get_bool(
    const char *section,
    const char *key,
    bool *value)
{
    (void)section;
    (void)key;
    (void)value;

    return false;
}

const config_runtime_t *config_get_runtime(void)
{
    return &s_runtime;
}
