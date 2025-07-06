#ifndef _HAL_U8G2_H_
#define _HAL_U8G2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "u8g2.h"

uint8_t u8x8_byte_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_hw(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#ifdef __cplusplus
}
#endif

#endif
