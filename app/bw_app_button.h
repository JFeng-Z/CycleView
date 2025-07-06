#ifndef _BW_APP_BUTTON_H_
#define _BW_APP_BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

void BW_Button_Init(void);
void BW_Button_DeInit(void);
uint8_t app_button_action_read(void);

#ifdef __cplusplus
}
#endif

#endif
