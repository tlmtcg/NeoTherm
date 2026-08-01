#include "app_config_internal.h"

#include "ini.h"

void app_config_load_files(void)
{
    /*
     * Valeurs par défaut
     */

    if (!ini_key_exists(
            "files",
            "runtime"))
    {
        ini_set_string(
            "files",
            "runtime",
            "runtime.ini");
    }


    if (!ini_key_exists(
            "files",
            "schedule"))
    {
        ini_set_string(
            "files",
            "schedule",
            "program.ini");
    }


    if (!ini_key_exists(
            "files",
            "history"))
    {
        ini_set_string(
            "files",
            "history",
            "history.dat");
    }


    if (!ini_key_exists(
            "files",
            "history_csv"))
    {
        ini_set_string(
            "files",
            "history_csv",
            "history.csv");
    }



    /*
     * Chargement configuration
     */

    (void)ini_get_string(
        "files",
        "runtime",
        s_config.runtime_file,
        sizeof(s_config.runtime_file));


    (void)ini_get_string(
        "files",
        "schedule",
        s_config.schedule_file,
        sizeof(s_config.schedule_file));


    (void)ini_get_string(
        "files",
        "history",
        s_config.history_file,
        sizeof(s_config.history_file));


    (void)ini_get_string(
        "files",
        "history_csv",
        s_config.history_csv_file,
        sizeof(s_config.history_csv_file));
}
