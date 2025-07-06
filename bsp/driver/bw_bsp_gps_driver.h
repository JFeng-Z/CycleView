#ifndef _BW_BSP_GPS_DRIVER_H_
#define _BW_BSP_GPS_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef enum
{
    BSP_GPS_DRIVER_STAT_DEINITED,
    BSP_GPS_DRIVER_STAT_INITED
}bsp_gps_driver_init_stat_t;

typedef struct
{
    int8_t (*init)(void);
    int8_t (*deinit)(void);
    int8_t (*read)(void *pdata);
    int8_t (*write)(uint8_t data);
}bsp_gps_driver_interface_t;

typedef struct
{
    bsp_gps_driver_interface_t *pfinterface;
    bsp_gps_driver_init_stat_t init_stat;
    int8_t (*pfcallback)(void *pfdata);
}bsp_gps_driver_t;

int8_t bsp_gps_driver_instance(int8_t (*pcallback)(void *pdata),
                               bsp_gps_driver_t *pgps_driver,
                               bsp_gps_driver_interface_t *pfgpsdriver_interface);
int8_t bsp_gps_driver_init(void);
int8_t bsp_gps_driver_deinit(void);
int8_t bsp_gps_driver_read(void *pdata);
int8_t bsp_gps_driver_write(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
