#include "nor_flash.h"
#include "bw_app_w25q64.h"
#include "fonts.h"
#include "dispDriver.h"

#define FLASH_START_ADDR 0x000000
#define FLASH_END_ADDR   0x7FFFFF

#define FLASH_FONT_START_ADDR 0x000000
#define FLASH_FONT_END_ADDR   0x000FFF

void font_write(void)
{
    BW_W25Q64_Write(FLASH_FONT_START_ADDR, (uint8_t *)font_menu_main_h12w6.ptr, font_menu_main_h12w6.size);
}

void font_test(void)
{
    
    BW_W25Q64_Read(FLASH_FONT_START_ADDR, (uint8_t *)font_menu_main_h12w6.ptr, font_menu_main_h12w6.size);
}

uint8_t BW_W25Q64_OneByteRead(uint8_t *address)
{
    uint8_t data = 0;
    // uint32_t addr = *(&msk);
    // BW_W25Q64_Read(FLASH_FONT_START_ADDR + addr, &data, 1);
    return data;
}