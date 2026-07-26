#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static config_runtime_t s_runtime;
static char s_current_section[CONFIG_SECTION_LENGTH];

/*==========================================================
 * Fonctions privées
 *=========================================================*/

static bool config_add_entry(
    const char *section,
    const char *key,
    const char *value)
{
    if ((section == NULL) ||
        (key == NULL) ||
        (value == NULL))
    {
        printf("config_add_entry() : argument NULL\n");
        return false;
    }

    if (s_runtime.count >= CONFIG_MAX_ENTRIES)
    {
        printf("config_add_entry() : tableau plein\n");
        return false;
    }

    config_entry_t *entry = &s_runtime.entries[s_runtime.count];

    strncpy(entry->section,
            section,
            sizeof(entry->section) - 1);
    entry->section[sizeof(entry->section) - 1] = '\0';

    strncpy(entry->key,
            key,
            sizeof(entry->key) - 1);
    entry->key[sizeof(entry->key) - 1] = '\0';

    strncpy(entry->value,
            value,
            sizeof(entry->value) - 1);
    entry->value[sizeof(entry->value) - 1] = '\0';

    s_runtime.count++;

    return true;
}

static bool config_parse_line(char *line)
{
    if (line == NULL)
    {
        printf("config_parse_line() : ligne NULL\n");
        return false;
    }

    /* Supprime CR/LF */
    line[strcspn(line, "\r\n")] = '\0';

    /* Ligne vide */
    if (line[0] == '\0')
    {
        return true;
    }

    /* Commentaires */
    if ((line[0] == '#') || (line[0] == ';'))
    {
        return true;
    }

    /* Section */
    if (line[0] == '[')
    {
        char *end = strchr(line, ']');

        if (end == NULL)
        {
            printf("ERREUR : ] manquant\n");
            return false;
        }

        *end = '\0';

        const char *section_name = line + 1;

        strncpy(s_current_section,
                section_name,
                sizeof(s_current_section) - 1);

        s_current_section[sizeof(s_current_section) - 1] = '\0';

        return true;
    }

    /* clé = valeur */

    char *equal = strchr(line, '=');

    if (equal == NULL)
    {
        printf("ERREUR : '=' introuvable\n");
        return false;
    }

    if (s_current_section[0] == '\0')
    {
        printf("ERREUR : aucune section courante\n");
        return false;
    }

    *equal = '\0';

    char *key = line;
    char *value = equal + 1;

    return config_add_entry(
        s_current_section,
        key,
        value);
}

/*==========================================================
 * API publique
 *=========================================================*/

bool config_init(const char *filename)
{
    if (filename == NULL)
    {
        printf("config_init() : filename NULL\n");
        return false;
    }

    s_runtime.count = 0;
    s_runtime.initialized = false;

    s_current_section[0] = '\0';

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Impossible d'ouvrir le fichier.\n");
        return false;
    }

    char line[256];
    int line_number = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        line_number++;

        if (!config_parse_line(line))
        {
            printf("\nERREUR DE PARSING LIGNE %d\n",
                   line_number);

            fclose(fp);

            return false;
        }
    }

    fclose(fp);

    s_runtime.initialized = true;

    return true;
}

bool config_get_int(
    const char *section,
    const char *key,
    int *value)
{
    char buffer[32];

    if (!config_get_string(section,
                           key,
                           buffer,
                           sizeof(buffer)))
    {
        return false;
    }

    *value = (int)strtol(buffer, NULL, 10);

    return true;
}

bool config_get_float(
    const char *section,
    const char *key,
    float *value)
{
    char buffer[32];

    if (!config_get_string(section,
                           key,
                           buffer,
                           sizeof(buffer)))
    {
        return false;
    }

    *value = strtof(buffer, NULL);

    return true;
}

bool config_get_bool(
    const char *section,
    const char *key,
    bool *value)
{
    char buffer[16];

    if (value == NULL)
    {
        return false;
    }

    if (!config_get_string(section,
                           key,
                           buffer,
                           sizeof(buffer)))
    {
        return false;
    }

    if ((strcmp(buffer, "true") == 0) ||
        (strcmp(buffer, "TRUE") == 0) ||
        (strcmp(buffer, "1") == 0) ||
        (strcmp(buffer, "yes") == 0))
    {
        *value = true;
        return true;
    }

    if ((strcmp(buffer, "false") == 0) ||
        (strcmp(buffer, "FALSE") == 0) ||
        (strcmp(buffer, "0") == 0) ||
        (strcmp(buffer, "no") == 0))
    {
        *value = false;
        return true;
    }

    return false;
}

const config_runtime_t *config_get_runtime(void)
{
    return &s_runtime;
}

bool config_get_string(
    const char *section,
    const char *key,
    char *buffer,
    size_t buffer_size)
{
    if ((section == NULL) ||
        (key == NULL) ||
        (buffer == NULL) ||
        (buffer_size == 0))
    {
        return false;
    }

    for (size_t i = 0; i < s_runtime.count; i++)
    {
        const config_entry_t *entry = &s_runtime.entries[i];

        if ((strcmp(entry->section, section) == 0) &&
            (strcmp(entry->key, key) == 0))
        {
            strncpy(buffer,
                    entry->value,
                    buffer_size - 1);

            buffer[buffer_size - 1] = '\0';

            return true;
        }
    }

    return false;
}
