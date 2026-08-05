#include "app.h"


int main(void)
{
    if (!app_init())
    {
        return 1;
    }

    if (!app_run())
    {
        return 1;
    }

    return 0;
}
