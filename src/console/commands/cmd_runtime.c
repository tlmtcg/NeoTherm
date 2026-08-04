#include "cmd_runtime.h"
#include "runtime.h"

bool cmd_runtime(const char *args)
{
    (void)args;

    runtime_dump();

    return true;
}