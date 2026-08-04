#include "cmd_clear.h"

#include <stdlib.h>

bool cmd_clear(const char *args)
{
    (void)args;

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    return true;
}