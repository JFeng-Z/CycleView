#include "bw_bsp_rtc_handle.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "time.h"

static const nrf_drv_rtc_t rtc_drv = NRF_DRV_RTC_INSTANCE(2);

static volatile uint32_t millis_cnt = 0;

extern time_t ms;
extern time_t s;
extern struct tm tm;

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    static uint16_t ticks = 0;
    if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        ticks += 1000;
        if(ticks >= 1024)
        {
            ticks -= 1024;
            millis_cnt++;
        }
        ms ++;
        if(ms >= 1000)
        {
            ms = 0;
            s ++;
            tm = *localtime(&s);
        }
    }
}

void RTC_Init(void)
{
    ret_code_t errCode;
    
    nrf_drv_rtc_config_t rtcConfig = NRF_DRV_RTC_DEFAULT_CONFIG;		//Initialize RTC instance
    rtcConfig.prescaler = 31; 										
    
	errCode = nrf_drv_rtc_init(&rtc_drv, &rtcConfig, rtc_handler);
    APP_ERROR_CHECK(errCode);

    nrf_drv_rtc_tick_enable(&rtc_drv, true);						// Enable tick event & interrupt 
    nrf_drv_rtc_enable(&rtc_drv);									// Power on RTC instance
}

void RTC_DeInit(void)
{
    nrf_drv_rtc_tick_disable(&rtc_drv);
    nrf_drv_rtc_disable(&rtc_drv);
    nrf_drv_rtc_uninit(&rtc_drv);
}

uint32_t millis(void)
{
    return millis_cnt;
}