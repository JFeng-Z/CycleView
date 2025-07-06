#ifndef _BW_APP_RTC_H_
#define _BW_APP_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "TinyGPSPlus_link.h"

void BW_RTC_Init(void);
void BW_RTC_DeInit(void);
uint32_t BW_Millis(void);
void BW_RTC_SetTime(GPSData_t gps);

#ifdef __cplusplus
}
#endif

#endif
