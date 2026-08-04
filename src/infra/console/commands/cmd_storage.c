#include "cmd_storage.h"

#include "storage.h"
#include "runtime.h"
#include "console_utils.h"

#include <stdio.h>
#include <string.h>


#include "cmd_storage.h"

#include "storage.h"
#include "runtime.h"
#include "console_utils.h"

#include <stdio.h>
#include <string.h>


bool cmd_storage(const char *args)
{
    if (args == NULL || args[0] == '\0')
    {
        printf("Usage : storage save|load\n");
        return false;
    }


    if (strcmp(args, "save") == 0)
    {
        console_print_header("Storage Save");

        if (runtime_save())
        {
            printf("Runtime configuration saved\n");
            return true;
        }

        printf("Storage save failed\n");
        return false;
    }


    if (strcmp(args, "load") == 0)
    {
        console_print_header("Storage Load");

        if (runtime_load())
        {
            printf("Runtime configuration loaded\n");
            return true;
        }

        printf("Storage load failed\n");
        return false;
    }


    printf("Unknown storage command : %s\n", args);
    printf("Usage : storage save|load\n");

    return false;
}
