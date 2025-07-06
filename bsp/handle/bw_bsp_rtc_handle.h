#ifndef _BW_BSP_RTC_HANDLE_H_
#define _BW_BSP_RTC_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void RTC_Init(void);
void RTC_DeInit(void);
uint32_t millis(void);

#ifdef __cplusplus
}
#endif

#endif
