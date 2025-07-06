#include "bw_app_w25q64.h"
#include "bw_bsp_w25q64_handle.h"
#include "nrf_log.h"

static int8_t bsp_w25q64_driver_callback(void *pdata)
{
    NRF_LOG_INFO("W25Q64 callback");
    return 0;
}

void BW_W25Q64_Register(void)
{
    bsp_w25q64_handle_register(bsp_w25q64_driver_callback);
}

void BW_W25Q64_Init(void)
{
    bsp_w25q64_handle_init();
}

void BW_W25Q64_DeInit(void)
{
    bsp_w25q64_handle_deinit();
}

void BW_W25Q64_Read(uint32_t address, uint8_t *pdata, uint32_t size)
{
    bsp_w25q64_handle_read(address, pdata, size);
}

void BW_W25Q64_Write(uint32_t address, uint8_t *pdata, uint32_t size)
{
    bsp_w25q64_handle_write(address, pdata, size);
}
