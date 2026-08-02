#include "alarm.h"

bool cmd_alarms(const char *args)
{
    alarm_dump();

    return true;
}