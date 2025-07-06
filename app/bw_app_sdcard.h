#ifndef _BW_APP_SDCARD_H_
#define _BW_APP_SDCARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_conf.h"

void BW_SDCard_Init(void);
void BW_SDCard_DeInit(void);
void SDCard_state_control(ui_t *ui);
bool file_delete(ui_t *ui);
void sdcard_format(ui_t *ui);

#ifdef __cplusplus
}
#endif

#endif
