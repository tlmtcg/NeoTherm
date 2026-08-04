#include "cmd_config.h"
#include "app_config.h"
#include <stdbool.h>

bool cmd_config(const char *args)
{
    (void)args;

    app_config_dump();

    return true;
}
