#ifndef __LCD_SPI_DRV_H
#define __LCD_SPI_DRV_H
#include "gd32e23x.h"
#include <stdio.h>

struct lcd_spi_drv_obj
{
    void(*cs)(uint8_t level);
    void(*rst)(uint8_t level);
    void(*dc)(uint8_t level);
    void(*blk)(uint8_t level);

    void(*clk)(uint8_t level);
    void(*sda)(uint8_t level);

    void(*spi_send)(uint8_t data); 

};
void lcd_spi_drv_init(struct lcd_spi_drv_obj *obj);
extern struct lcd_spi_drv_obj lcd_drv;
#endif