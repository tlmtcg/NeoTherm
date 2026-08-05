#include "cmd_quit.h"
#include "app.h"

#include <stdlib.h>
#include "console_utils.h"

bool cmd_quit(const char *args)
{
    (void)args;

    console_print_header("NeoTherm Simulator");

    console_print_string(
        "Status",
        "Stopping application");

    console_print_separator();

    app_request_exit();

    return true;
}