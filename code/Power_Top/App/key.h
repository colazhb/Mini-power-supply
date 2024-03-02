#ifndef __KEY_H
#define __KEY_H
#include "gd32e23x.h"
void key_init(void);
uint8_t ec_val_get();
void ec_cal_timer_init();
void ec_init(void);
int multi_button_init(void);

#endif