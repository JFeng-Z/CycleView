#ifndef _BW_BSP_W25Q64_HANDLE_H_
#define _BW_BSP_W25Q64_HANDLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    void *pw25q64_driver;
    int8_t(*pfinit)(void);
    int8_t(*pfdeinit)(void);
    int8_t(*pfread)(uint32_t addr, uint8_t *pdata, uint32_t size);
    int8_t(*pfwrite)(uint32_t addr, uint8_t *pdata, uint32_t size);
    void(*callback)(void *);
    void *rtos_thread_handle;
    uint32_t(*get_tick)(void);
}bsp_w25q64_handle_t;

int8_t bsp_w25q64_handle_init(void);
int8_t bsp_w25q64_handle_deinit(void);
int8_t bsp_w25q64_handle_read(uint32_t addr, uint8_t *pdata, uint32_t size);
int8_t bsp_w25q64_handle_write(uint32_t addr, uint8_t *pdata, uint32_t size);
bsp_w25q64_handle_t *bsp_w25q64_handle_register(int8_t (*pfcallback)(void *));

#ifdef __cplusplus
}
#endif

#endif
