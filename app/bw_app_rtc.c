#include "bw_app_rtc.h"
#include "bw_bsp_rtc_handle.h"
#include "time.h"

time_t ms = 0;
time_t s = 0;
struct tm tm;

void BW_RTC_Init(void)
{
    RTC_Init();
}

void BW_RTC_DeInit(void)
{
    RTC_DeInit();
}

void BW_RTC_SetTime(GPSData_t gps)
{
    tm.tm_year = gps.date.year - 1900;
    tm.tm_mon = gps.date.month - 1;
    tm.tm_mday = gps.date.day;
    tm.tm_hour = gps.time.hour;
    tm.tm_min = gps.time.minute;
    tm.tm_sec = gps.time.second;
    s = mktime(&tm);
}

uint32_t BW_Millis(void)
{
    return millis();
}