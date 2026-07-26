#include "debug.h"

#include <stdio.h>

/*==========================================================
 * Initialisation
 *=========================================================*/

void debug_init(void)
{
    printf("Debug initialized\n");
}

/*==========================================================
 * Dump Configuration
 *=========================================================*/

void debug_dump_config(const config_runtime_t *runtime)
{
    if (runtime == NULL)
    {
        printf("\n===== CONFIGURATION =====\n");
        printf("Runtime is NULL\n");
        printf("=========================\n");

        return;
    }

    printf("\n===== CONFIGURATION =====\n");

    printf("Initialized : %s\n",
           runtime->initialized ? "YES" : "NO");

    printf("Entries     : %zu\n\n",
           runtime->count);

    printf("%-4s %-15s %-20s %-20s\n",
           "Id",
           "Section",
           "Key",
           "Value");

    printf("------------------------------------------------------------\n");

    for (size_t i = 0; i < runtime->count; i++)
    {
        const config_entry_t *entry = &runtime->entries[i];

        printf("%-4zu %-15s %-20s %-20s\n",
               i,
               entry->section,
               entry->key,
               entry->value);
    }

    printf("============================================================\n");
}