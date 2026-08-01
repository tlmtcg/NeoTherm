#ifndef INI_INTERNAL_H
#define INI_INTERNAL_H

#include "ini.h"

extern ini_runtime_t s_runtime;

// extern char s_current_section[CONFIG_SECTION_LENGTH];

bool ini_save(void);

bool ini_add_entry(
    const char *section,
    const char *key,
    const char *value);

bool ini_parse_file(
    const char *filename);


#endif
