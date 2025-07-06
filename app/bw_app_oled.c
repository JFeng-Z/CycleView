#include "bw_app_oled.h"
#include "u8g2.h"
#include "hal_u8g2.h"

u8g2_t u8g2;

void BW_OLED_Init(void)
{ 
    u8g2_Setup_sh1106_128x64_noname_f(&u8g2, U8G2_R2, u8x8_byte_hw_spi, u8x8_gpio_and_delay_hw);
    u8g2_InitDisplay(&u8g2);                                                                       
    u8g2_SetPowerSave(&u8g2, 0);                                                                   
    u8g2_ClearBuffer(&u8g2);
}

void BW_OLED_DeInit(void)
{
    // u8g2_SetPowerSave(&u8g2, 1);
}
