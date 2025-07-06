#ifndef _BW_APP_W25Q64_H_
#define _BW_APP_W25Q64_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

void BW_W25Q64_Register(void);
void BW_W25Q64_Init(void);
void BW_W25Q64_DeInit(void);
void BW_W25Q64_Read(uint32_t address, uint8_t *data, uint32_t length);
void BW_W25Q64_Write(uint32_t address, uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
