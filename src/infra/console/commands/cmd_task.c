#include "scheduler.h"
#include <stdbool.h>

bool cmd_task(const char *args)
{
    (void)args;

    scheduler_dump();

    return true;
}