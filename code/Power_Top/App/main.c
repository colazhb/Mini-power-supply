#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "lcd_spi_drv.h"
#include "lcd.h"
#include "frame_ctl_pwb.h"
#include "smarttimer.h"
#include "board_pin_def.h"
#include "beep.h"
#include "gui.h"
#include "uart_com.h"
#include "led_rgb.h"
#include "multi_button.h"
#include "key.h"
#include "gui.h"
#include "multi_button.h"

void ck_prt()
{
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));
}
void led_blink()
{
    static uint8_t led_on = 0;
    //led_rgb_switch(LED_R, led_on);
    led_on = !led_on;
}
int main(void)
{

    systick_config();

    stim_init();

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI0);

    uart_dbg_init();
    uart_com_init();

    pwb_frame_init();

    led_rgb_init();
    beep_init();
    beep();
 
    key_init();
    multi_button_init();
  
    ec_init();
    ec_cal_timer_init();

    lcd_spi_drv_init(&lcd_drv);
    lcd_init();
    gui_init();

    ck_prt();

    stim_loop(1000, led_blink, STIM_LOOP_FOREVER);

    //stim_loop(20, beep_loop, STIM_LOOP_FOREVER);

    stim_loop(10, frame_loop_cb, STIM_LOOP_FOREVER);

    stim_loop(10, gui_loop, STIM_LOOP_FOREVER);

    //stim_loop(10, button_ticks, STIM_LOOP_FOREVER);

    gui_logo();

    delay_1ms(500);
    
    gui_init_board();
 
    while (1)
    {   
        stim_mainloop();
    }
}
