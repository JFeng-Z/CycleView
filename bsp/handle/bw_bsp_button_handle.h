#ifndef _BW_BSP_BUTTON_HANDLE_H_
#define _BW_BSP_BUTTON_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef enum
{
    BW_BSP_BUTTON_HANDLE_EVENT_PRESS,
    BW_BSP_BUTTON_HANDLE_EVENT_RELEASE,
    BW_BSP_BUTTON_HANDLE_EVENT_LONG_PRESS,
    BW_BSP_BUTTON_HANDLE_EVENT_ONCE_PRESS
}bsp_button_handle_event_t;

typedef struct
{
    void(*callback)(void *);
    void *rtos_thread_handle;
    uint32_t(*get_tick)(void);
    bsp_button_handle_event_t event;
    uint8_t ID;
}bsp_button_handle_t;

uint8_t bsp_button_handle_init(uint8_t num);
uint8_t bsp_button_handle_deinit(uint8_t num);
uint8_t bsp_button_handle_read_level(uint8_t num);
void bsp_button_handle_register(uint8_t num);

#ifdef __cplusplus
}
#endif

#endif
