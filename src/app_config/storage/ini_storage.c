#include "ini_internal.h"
#include <stdio.h>
#include <string.h>

bool ini_save(void)
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
