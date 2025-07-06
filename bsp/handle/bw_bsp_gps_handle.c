#include "bw_bsp_gps_handle.h"
#include "bw_bsp_gps_driver.h"
#include "stdio.h"
#include "nrf_log.h"

static bsp_gps_handle_t bw_bsp_gps_handle;

int8_t bsp_gps_handle_init(void)
{
    if(bw_bsp_gps_handle.pfinit == NULL) return -1;
    return bw_bsp_gps_handle.pfinit();
}

int8_t bsp_gps_handle_deinit(void)
{
    if(bw_bsp_gps_handle.pfdeinit == NULL) return -1;
    return bw_bsp_gps_handle.pfdeinit();
}

int8_t bsp_gps_handle_read(void *data)
{
    if(bw_bsp_gps_handle.pfread == NULL) return -1;
    return bw_bsp_gps_handle.pfread(data);
}

int8_t bsp_gps_handle_write(uint8_t data)
{
    return bw_bsp_gps_handle.pfwrite(data);
}

bsp_gps_handle_t *bsp_gps_handle_register(int8_t (*pfcallback)(void *))
{
    static bsp_gps_driver_t bw_bsp_gps_driver;
    static bsp_gps_driver_interface_t bw_bsp_gps_driver_interface;

    bsp_gps_driver_instance(pfcallback, &bw_bsp_gps_driver, &bw_bsp_gps_driver_interface);

    bw_bsp_gps_handle.pgps_driver = &bw_bsp_gps_driver;
    bw_bsp_gps_handle.pfinit = bsp_gps_driver_init;
    bw_bsp_gps_handle.pfdeinit = bsp_gps_driver_deinit;
    bw_bsp_gps_handle.pfread = bsp_gps_driver_read;
    bw_bsp_gps_handle.pfwrite = bsp_gps_driver_write;

    return &bw_bsp_gps_handle;
}