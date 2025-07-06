#include "bw_app_timer.h"
#include "app_error.h"
#include "app_timer.h"

void BW_Timer_Init(void)
{
    ret_code_t err_code = app_timer_init();

    APP_ERROR_CHECK(err_code);
}