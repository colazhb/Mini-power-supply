#ifndef __GUI_H
#define __GUI_H
#include "gd32e23x.h"
#include <stdio.h>

enum
{
	GUI_S_INIT,
	GUI_S_STOP,
	GUI_S_RUN,
	GUI_S_SET_CC_CV,
	GUI_S_SET_V,
	GUI_SET_V_NUM_H,
	GUI_SET_V_NUM_L,
	GUI_S_SET_I_NUM_H,
	GUI_S_SET_I_NUM_L,
	GUI_S_WAIT,
};

void gui_init();
void gui_loop();
void gui_set_status(uint8_t status);
void gui_logo();
void gui_init_board();
#endif