
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "ini_internal.h"
#include "logger.h"


char *trim(char *str)
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

bool ini_parse_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        LOG_ERROR("INI", "Unable to open %s", filename);
        return false;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (!ini_parse_line(line))
        {
            fclose(fp);

            LOG_ERROR("INI", "Parse error: %s", line);

            return false;
        }
    }

    fclose(fp);

    return true;
}
