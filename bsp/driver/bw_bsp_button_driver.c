#include "bw_bsp_button_driver.h"
#include "nrf_gpio.h"

#define BSP_BUTTON_PIN_0       (NRF_GPIO_PIN_MAP(0, 28))
#define BSP_BUTTON_PIN_1       (NRF_GPIO_PIN_MAP(0, 3))
#define BSP_BUTTON_PIN_2       (NRF_GPIO_PIN_MAP(1, 10))
#define BSP_BUTTON_PIN_MODE  (NRF_GPIO_PIN_PULLUP)

static gpio_attribute_t gpio_attribute[BW_BSP_BUTTON_DRIVER_MAX_NUM] = 
{
    {BSP_BUTTON_PIN_0, BSP_BUTTON_PIN_MODE},
    {BSP_BUTTON_PIN_1, BSP_BUTTON_PIN_MODE},
    {BSP_BUTTON_PIN_2, BSP_BUTTON_PIN_MODE},
};

static void gpio_driver_init(uint8_t number)
{
    nrf_gpio_cfg_input((uint32_t )gpio_attribute[number].pin, (nrf_gpio_pin_pull_t )gpio_attribute[number].mode);
}

static void gpio_driver_deinit(uint8_t number)
{
    nrf_gpio_cfg_default((uint32_t )gpio_attribute[number].pin);
}

static uint8_t gpio_driver_read(uint8_t number)
{
    return (uint8_t )nrf_gpio_pin_read((uint32_t )gpio_attribute[number].pin);
}

int8_t bsp_button_driver_instance(uint8_t number,
                                  bsp_button_driver_t *pbutton_driver, 
                                  gpio_driver_interface_t *pfgpiodariver_interface)
{
    if(pbutton_driver == NULL || pfgpiodariver_interface == NULL)
    {
        return -1;
    }
    
    if(pfgpiodariver_interface->pfinit != NULL && pfgpiodariver_interface->pfdeinit != NULL && pfgpiodariver_interface->pfread != NULL)
    {
        return 0;
    }

    pfgpiodariver_interface->attribute.pin = gpio_attribute[number].pin;
    pfgpiodariver_interface->attribute.mode = gpio_attribute[number].mode;
    pfgpiodariver_interface->pfinit = gpio_driver_init;
    pfgpiodariver_interface->pfdeinit = gpio_driver_deinit;
    pfgpiodariver_interface->pfread = gpio_driver_read;

    pbutton_driver->pfgpiodariver_interface = pfgpiodariver_interface;
    pbutton_driver->pfgpiodariver_interface->stat = BW_BSP_BUTTON_STAT_READ;

    return 0;
}

int8_t bsp_button_driver_init(bsp_button_driver_t *pbutton_driver)
{
    if(pbutton_driver == NULL)
    {
        return -1;
    }
    pbutton_driver->pfgpiodariver_interface->pfinit(pbutton_driver->ID);
    pbutton_driver->pfgpiodariver_interface->stat = BW_BSP_BUTTON_STAT_INITED;
    return 0;
}

int8_t bsp_button_driver_deinit(bsp_button_driver_t *pbutton_driver)
{
    if(pbutton_driver == NULL)
    {
        return -1;
    }
    pbutton_driver->pfgpiodariver_interface->pfdeinit(pbutton_driver->ID);
    pbutton_driver->pfgpiodariver_interface->stat = BW_BSP_BUTTON_STAT_DEINITED;
    return 0;
}

int8_t bsp_button_driver_read(bsp_button_driver_t *pbutton_driver)
{
    if(pbutton_driver == NULL || pbutton_driver->pfgpiodariver_interface->stat != BW_BSP_BUTTON_STAT_INITED)
    {
        return -1;
    }
    return (int8_t )pbutton_driver->pfgpiodariver_interface->pfread(pbutton_driver->ID);
}
