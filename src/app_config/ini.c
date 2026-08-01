#include "ini.h"

#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "ini_internal.h"

ini_runtime_t s_runtime;

char s_current_section[CONFIG_SECTION_LENGTH];

bool ini_init(const char *filename)
{
    if (filename == NULL)
    {
        return false;
    }

    memset(&s_runtime, 0, sizeof(s_runtime));

    strncpy(s_runtime.filename,
            filename,
            sizeof(s_runtime.filename) - 1);

    s_runtime.filename[sizeof(s_runtime.filename) - 1] = '\0';

    s_current_section[0] = '\0';

    if (!ini_parse_file(filename))
    {
        return false;
    }

    s_runtime.initialized = true;

    return true;
}

const ini_runtime_t *ini_get_runtime(void)
{
    return &s_runtime;
}
