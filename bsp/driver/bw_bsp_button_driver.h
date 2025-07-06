#ifndef _BW_BSP_BUTTON_DRIVER_H_
#define _BW_BSP_BUTTON_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define BW_BSP_BUTTON_DRIVER_MAX_NUM    (3)

typedef enum {
    BW_BSP_BUTTON_STAT_DEINITED,
    BW_BSP_BUTTON_STAT_INITED,
    BW_BSP_BUTTON_STAT_READ
}gpio_stat_t;

typedef struct {
    uint8_t pin;
    uint8_t mode;
}gpio_attribute_t;

typedef struct {
    gpio_attribute_t attribute;
    void (*pfinit)(uint8_t number);
    void (*pfdeinit)(uint8_t number);
    uint8_t (*pfread)(uint8_t number);
    gpio_stat_t stat;
}gpio_driver_interface_t;

typedef struct {
    gpio_driver_interface_t *pfgpiodariver_interface;
    uint8_t ID;
}bsp_button_driver_t;

int8_t bsp_button_driver_instance(uint8_t number,
                                  bsp_button_driver_t *pbutton_driver, 
                                  gpio_driver_interface_t *pfgpiodariver_interface);
int8_t bsp_button_driver_init(bsp_button_driver_t *pbutton_driver);
int8_t bsp_button_driver_deinit(bsp_button_driver_t *pbutton_driver);
int8_t bsp_button_driver_read(bsp_button_driver_t *pbutton_driver);

#ifdef __cplusplus
}
#endif

#endif
