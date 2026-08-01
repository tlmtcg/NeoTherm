#include "ini_internal.h"
#include <string.h>
#include <ctype.h>


bool ini_add_entry(const char *section, const char *key, const char *value)
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

