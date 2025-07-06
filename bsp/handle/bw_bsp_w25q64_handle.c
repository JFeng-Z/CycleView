#include "bw_bsp_w25q64_handle.h"
#include "bw_bsp_w25q64_driver.h"

static bsp_w25q64_handle_t bw_bsp_w25q64_handle;

int8_t bsp_w25q64_handle_init(void)
{
    return bw_bsp_w25q64_handle.pfinit();
}

int8_t bsp_w25q64_handle_deinit(void)
{
    return bw_bsp_w25q64_handle.pfdeinit();
}

int8_t bsp_w25q64_handle_read(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    return bw_bsp_w25q64_handle.pfread(addr, pdata, size);
}

int8_t bsp_w25q64_handle_write(uint32_t addr, uint8_t *pdata, uint32_t size)
{
    return bw_bsp_w25q64_handle.pfwrite(addr, pdata, size);
}

bsp_w25q64_handle_t *bsp_w25q64_handle_register(int8_t (*pfcallback)(void *))
{
    static bsp_w25q64_driver_t bw_bsp_w25q64_driver;
    static bsp_w25q64_interface_t bw_bsp_w25q64_interface;

    bsp_w25q64_driver_instance(pfcallback, &bw_bsp_w25q64_driver, &bw_bsp_w25q64_interface);

    bw_bsp_w25q64_handle.pw25q64_driver = &bw_bsp_w25q64_driver;
    bw_bsp_w25q64_handle.pfinit = bsp_w25q64_driver_init;
    bw_bsp_w25q64_handle.pfdeinit = bsp_w25q64_driver_deinit;
    bw_bsp_w25q64_handle.pfread = bsp_w25q64_driver_read;
    bw_bsp_w25q64_handle.pfwrite = bsp_w25q64_driver_write;

    return &bw_bsp_w25q64_handle;
}
