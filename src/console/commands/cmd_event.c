#include "cmd_event.h"

#include "event.h"
#include "console_utils.h"

#include <stdio.h>
#include <string.h>


bool cmd_event(const char *args)
{
    (void)args;

    console_print_header("Event Counters");


    printf("%-30s %s\n",
           "Event",
           "Count");

    console_print_separator();


    for (event_type_t i = EVENT_NONE + 1;
         i < EVENT_COUNT;
         i++)
    {
        printf("%-30s %u\n",
               event_name(i),
               event_get_count(i));
    }


    console_print_separator();


    printf("Total events : %u\n",
           event_get_total_count());


    printf("\n");


    return true;
}
