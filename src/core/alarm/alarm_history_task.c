#include "alarm_history_task.h"
#include "alarm_history.h"


 void alarm_history_save_task(void)
{
    if (alarm_history_is_dirty())
    {
        alarm_history_save();
    }
}
