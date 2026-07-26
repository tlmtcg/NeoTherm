#include "app.h"


int main(void)
{
    if (!app_init())
    {
        return 1;
    }


    app_run();


    return 0;
}
