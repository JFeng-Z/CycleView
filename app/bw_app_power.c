#include "bw_app_power.h"
#include "nrf_drv_power.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"

void BW_PowerManagement_Init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

void BW_PowerManagement_EnterSleep(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
        __SEV();
        __WFE();
        __WFE();
    }
}

void BW__LowPower(void)
{
    // uint32_t err_code;
    // err_code = 
}