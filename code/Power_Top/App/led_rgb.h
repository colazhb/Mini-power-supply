#ifndef __LED_RGB_H
#define __LED_RGB_H


#include "gd32e23x.h"

enum
{
    LED_R = 0,
    LED_G,
    LED_B,
    LED_MAX
};

void led_rgb_switch(uint8_t id, uint8_t on_off);
void led_rgb_init(void);
void led_led_on(uint8_t id);
#endif