#include "app_config_internal.h"

#include "ini.h"

void app_config_load_logger(void)
{
    if (!ini_key_exists(
            "logger",
            "level"))
    {
        ini_set_string(
            "logger",
            "level",
            "INFO");
    }


    ini_get_string(
        "logger",
        "level",
        s_config.logger_level,
        sizeof(s_config.logger_level));
}
