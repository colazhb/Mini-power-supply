#include "gd32e23x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "frame_ctl_pwb.h"
#include "smarttimer.h"
#include "led.h"
#include "adc_conv.h"
#include "board_pin_def.h"
#include "ina226_user.h"
#include "power_data.h"
#include "sc8815_user.h"
#include "protect.h"

void ck_prt()
{
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));
    printf("\r\nCK_AHB is %d", rcu_clock_freq_get(CK_AHB));
    printf("\r\nCK_APB1 is %d", rcu_clock_freq_get(CK_APB1));
    printf("\r\nCK_APB2 is %d", rcu_clock_freq_get(CK_APB2));
}

int main(void)
{

    systick_config();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    // led 初始化
    led_init();

    // 帧初始化
    pwb_frame_init();

    // 串口初始化
    uart_dbg_init();    // 调试串口 
    uart_com_init();    // 通讯串口

    // adc 初始化
    adc_conv_init();

    // ina226 初始化
    ina226_user_int();

    // sc8815 初始化
    sc8815_user_init();

    // 定时器功能初始化
    stim_init();

    // shell 初始化
    shell_user_init();

    // led 闪烁
    stim_loop(1000, led_blink, STIM_LOOP_FOREVER);

    //  帧解析,数据上报
    stim_loop(10, frame_loop_cb, STIM_LOOP_FOREVER);

    // ina 读取
    stim_loop(50, ina226_user_loop, STIM_LOOP_FOREVER);

    // sc8815
    stim_loop(50, sc8815_user_loop, STIM_LOOP_FOREVER);

    // adc
    stim_loop(100, adc_conv_loop, STIM_LOOP_FOREVER);

    // 保护检测
    stim_loop(10, power_protect_check, STIM_LOOP_FOREVER);

    ck_prt();

    printf("Max OTG Volt: %d\n", SC8815_GetMaxOutputVoltage());

    // 数据上报时间间隔 ms
    power_data.report_interval = 200;
    power_data.set_protect_voltage = 5000;
    power_data.set_protect_current = 1000;

    while (1)
    {
        stim_mainloop();
    }
}
