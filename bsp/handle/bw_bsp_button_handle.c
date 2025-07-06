#include "bw_bsp_button_handle.h"
#include "bw_bsp_button_driver.h"

static gpio_driver_interface_t button_interface[BW_BSP_BUTTON_DRIVER_MAX_NUM];
static bsp_button_driver_t button_driver[BW_BSP_BUTTON_DRIVER_MAX_NUM] = 
{
    {.ID = 0, .pfgpiodariver_interface = &button_interface[0]}
    ,{.ID = 1, .pfgpiodariver_interface = &button_interface[1]}
    ,{.ID = 2, .pfgpiodariver_interface = &button_interface[2]}
};

uint8_t bsp_button_handle_init(uint8_t num)
{
    int8_t ret;
    ret = bsp_button_driver_init(&button_driver[num]);
    if(ret == -1)
    {
        return 0xff;
    }
    return (uint8_t )ret;
}

uint8_t bsp_button_handle_deinit(uint8_t num)
{
    int8_t ret;
    ret = bsp_button_driver_deinit(&button_driver[num]);
    if(ret == -1)
    {
        return 0xff;
    }
    return (uint8_t )ret;
}

uint8_t bsp_button_handle_read_level(uint8_t num)
{
    int8_t ret;
    ret = bsp_button_driver_read(&button_driver[num]);
    if(ret == -1)
    {
        return 0xff;
    }
    return (uint8_t )ret;
}

void bsp_button_handle_register(uint8_t num)
{
    bsp_button_driver_instance(num, &button_driver[num], &button_interface[num]);
}