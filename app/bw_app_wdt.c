#include "bw_app_wdt.h"
#include "nrf_drv_wdt.h"
#include "nrf_drv_clock.h"
#include "nrf_log.h"

nrf_drv_wdt_channel_id m_channel_id;

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    NRF_LOG_ERROR("WDT timeout occurred!");
    NVIC_SystemReset();
}

void BW_WDT_Init(void)
{
    ret_code_t err_code;

    // Initialize the clock
    err_code = nrf_drv_clock_init();
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Clock initialization failed: %d", err_code);
    }
    nrf_drv_clock_lfclk_request(NULL);

    // Initialize the WDT
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);

    // Start the WDT
    nrf_drv_wdt_enable();
}

void BW_WDT_Refresh(void)
{
    // Refresh the WDT to prevent timeout
    nrf_drv_wdt_channel_feed(m_channel_id);
}