#include "bw_system.h"
#include "bw_app_button.h"
#include "bw_app_gps.h"
#include "bw_app_sdcard.h"
#include "bw_app_rtc.h"
#include "bw_app_oled.h"
#include "bw_app_battery.h"
#include "bw_app_power.h"
#include "bw_app_log.h"
#include "bw_app_timer.h"
#include "bw_app_sysclock.h"
#include "bw_app_ui.h"
#include "bw_app_w25q64.h"
#include "bw_app_wdt.h"

#include "nrf_delay.h"

#include "stdlib.h"

extern GPSData_t gps_data;
int Wave_TestData;

void BW_System_Init(void)
{
    BW_RTC_Init();
    BW_Battery_Init();
    BW_Button_Init();
    BW_GPS_Register();
    BW_GPS_Init();
    nrf_delay_ms(2000);
    BW_GPS_DeInit();
    BW_RTC_SetTime(gps_data);
    BW_OLED_Init();
    BW_UI_Init();
    GPS_List_Load();
    BW_WDT_Init();
}

void BW_System_LowPower(void)
{
    BW_GPS_DeInit();
    BW_SDCard_DeInit();
    BW_Battery_DeInit();
}

void BW_System_Task(void)
{
    Wave_TestData = rand() % 255;
    BW_UI_Task();
    gpx_file_write_task();
    BW_PowerManagement_EnterSleep();
    BW_WDT_Refresh();
}