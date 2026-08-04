#include "cmd_history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "console_utils.h"

bool cmd_history(const char *args)
{
    if ((args == NULL) || (*args == '\0'))
    {
        printf("Usage:\n");
        printf("  history status\n");
        printf("  history last [count]\n");

        return false;
    }

    if (strcmp(args, "reset") == 0)
    {
        history_clear();

        return true;
    }

    if (strcmp(args, "last") == 0)
    {
        history_dump_last(10);

        return true;
    }

    if (strncmp(args, "last ", 5) == 0)
    {
        uint32_t count =
            (uint32_t)atoi(args + 5);

        if (count == 0)
        {
            count = 10;
        }

        history_dump_last(count);

        return true;
    }

    printf("Unknown history command\n");

    return false;
}
