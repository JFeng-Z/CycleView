#ifndef _BW_BSP_W25Q64_DRIVER_H_
#define _BW_BSP_W25Q64_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

typedef enum
{
    BSP_w25q64_DRIVER_STAT_DEINITED,
    BSP_w25q64_DRIVER_STAT_INITED
}bsp_w25q64_driver_init_stat_t;

typedef struct {
    int8_t (*pfinit)(void);
    int8_t (*pfdeinit)(void);
    int8_t (*pfread)(uint32_t addr, uint8_t *pdata, uint32_t size);
    int8_t (*pfwrite)(uint32_t addr, uint8_t *pdata, uint32_t size);
}bsp_w25q64_interface_t;

typedef struct {
    bsp_w25q64_interface_t *pinterface;
    int8_t (*pcallback)(void *pdata);
    bsp_w25q64_driver_init_stat_t init_stat;
}bsp_w25q64_driver_t;

int8_t bsp_w25q64_driver_init(void);
int8_t bsp_w25q64_driver_deinit(void);
int8_t bsp_w25q64_driver_read(uint32_t addr, uint8_t *pdata, uint32_t size);
int8_t bsp_w25q64_driver_write(uint32_t addr, uint8_t *pdata, uint32_t size);
int8_t bsp_w25q64_driver_instance(int8_t (*pcallback)(void *pdata),
                                  bsp_w25q64_driver_t *pw25q64_driver, 
                                  bsp_w25q64_interface_t *pw25q64driver_interface);

#ifdef __cplusplus
}
#endif

#endif
