#ifndef _SDCARD_H_
#define _SDCARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "ff.h"
#include "ui_conf.h"
#include "ui.h"

typedef struct
{
    uint32_t capacity;     ///< Total capacity of the SDC in MB.
    uint32_t free_space;   ///< Free space available on the SDC in MB.
    uint32_t used_space;   ///< Used space on the SDC in MB.
    const char *vendor;    ///< Vendor string.
    const char *product;   ///< Product string.
    const char *revision;  ///< Revision string.
} sdcard_info_t;

void SDcard_Init(void);
void SDcard_DeInit(void);
void SDcard_Init_for_UI(void);
void SDcard_DeInit_for_UI(void);
uint8_t SDcard_State_Get(void);
void SDcard_GetInfo(void);

#ifdef __cplusplus
}
#endif

#endif
