#ifndef __MCU_FLASH_H
#define __MCU_FLASH_H

#include "gd32e23x.h"
void mcu_flash_write(uint8_t *data , uint16_t len);
void mcu_flash_read(uint8_t *data , uint16_t len);
void mcu_flash_erase(void);

#endif