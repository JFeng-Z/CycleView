#ifndef _BW_BSP_GPS_HANDLE_H_
#define _BW_BSP_GPS_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef struct
{
    void *pgps_driver;
    int8_t(*pfinit)(void);
    int8_t(*pfdeinit)(void);
    int8_t(*pfread)(void *);
    int8_t(*pfwrite)(uint8_t);
    void(*callback)(void *);
    void *rtos_thread_handle;
    uint32_t(*get_tick)(void);
}bsp_gps_handle_t;

int8_t bsp_gps_handle_init(void);
int8_t bsp_gps_handle_deinit(void);
int8_t bsp_gps_handle_read(void *data);
int8_t bsp_gps_handle_write(uint8_t data);
bsp_gps_handle_t *bsp_gps_handle_register(int8_t (*pfcallback)(void *));

#ifdef __cplusplus
}
#endif

#endif
