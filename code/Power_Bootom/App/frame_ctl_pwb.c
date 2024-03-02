#include "board_pin_def.h"
#include "frame.h"

#include "frame_ctl_pwb.h"
#include "lwrb.h"
#include "power_data.h"

/* 与电源底板进行通讯
高位在前  低位在后   例如：0x1234  低位0x34 高位0x12

1.模式设置  指令：0x01
|指令 |模式||电压|电流|
|0x01 |1字节|2字节|2字节|

模式: 0:恒压 1:恒流
电压:2字节 mv
电流:2字节 ma

2.保护设置  指令：0x02
|指令 |电压|电流|
|0x02 |2字节|2字节|
电压:2字节 mv ,输出最大电压值
电流:2字节 ma ,输出最大电流值

3.输出控制  指令：0x03
|指令 |控制|
|0x03 |1字节|
控制: 0:关闭 1:打开

5.设置匀速上报指令 指令:0x04
|指令 |上报间隔|
|0x03 |1字节|
上报间隔:ms 10-255

4.输出数据查询 指令：0x05
|指令 |
|0x05 |

返回数据:0x05
|指令|输出状态|模式|告警|电压|电流|输入电压|
|0x05|1字节  |1字节|1字节|2字节|2字节|

告警：0 无告警   1  过压  2 过流  3 其他故障



*/
struct frame_obj frame_pwb;
void pwb_frame_pack_05()
{

    /*
    返回数据:0x05
    |指令|输出状态|模式|告警|电压|电流|输入电压|SC8815_VBUS电压|SC8815_VBUS电流|SC8815中断|NTC温度|CPU温度|
    |0x05|1字节  |1字节|1字节|2字节|2字节|


    */
    uint8_t data[17] = {0};
    data[0] = 0x05;
    data[1] = power_data.set_switch_status;
    data[2] = power_data.set_cc_cv;
    data[3] = power_data.alarm;

    // INA226 读取部分
    // vbus 电压
    data[4] = (power_data.ina_read_voltage >> 8) & 0xff;
    data[5] = power_data.ina_read_voltage & 0xff;
    // 电流
    data[6] = (power_data.ina_read_current >> 8) & 0xff;
    data[7] = power_data.ina_read_current & 0xff;

    // 输入电压
    data[8] = (power_data.input_voltage >> 8) & 0xff;
    data[9] = power_data.input_voltage & 0xff;

    // sc8815 读取部分

    // VBUS 电压
    data[10] = (power_data.sc8815_vbus_voltage >> 8) & 0xff;
    data[11] = power_data.sc8815_vbus_voltage & 0xff;

    // VBUS 电流
    data[12] = (power_data.sc8815_vbus_current >> 8) & 0xff;
    data[13] = power_data.sc8815_vbus_current & 0xff;

    // interrupt
    data[14] = power_data.sc8815_interrupt;

    // temperature
    data[15] = (uint8_t)power_data.ntc_temp;


    frame_pack(&frame_pwb, data, 16);
}
void pwb_frame_callback(uint8_t *data, uint8_t len)
{

    uint8_t cmd = data[0];
    switch (cmd)
    {
    case 0x01:
        power_data.set_cc_cv = data[1];
        power_data.set_voltage = (data[2] << 8) | data[3];
        power_data.set_current = (data[4] << 8) | data[5];
        sc8815_user_set(power_data.set_cc_cv, power_data.set_voltage, power_data.set_current);

        break;
    case 0x02:
        power_data.set_protect_voltage = (data[1] << 8) | data[2];
        power_data.set_protect_current = (data[3] << 8) | data[4];
        sc8815_protect_set(power_data.set_protect_voltage, power_data.set_protect_current);
        break;
    case 0x03:
        power_data.set_switch_status = data[1];
        sc8815_user_power_on_off(power_data.set_switch_status);
        break;
    case 0x04:
        power_data.report_interval = data[1];
        break;
    case 0x05:
        pwb_frame_pack_05();
        break;

    default:
        break;
    }
};

void pwb_frame_send_data(uint8_t ch)
{
    com_put_char(ch);
};

void pwb_frame_init()
{
    frame_pwb.frame_send_data = pwb_frame_send_data;
    frame_pwb.frame_callback = pwb_frame_callback;
};

void frame_loop_cb()
{
    static uint32_t last_time = 0;
    frame_loop(&frame_pwb);
    if (power_data.report_interval != 0)
    {
        if (get_sys_tick_diff(last_time) >= power_data.report_interval)
        {
            last_time = get_sys_tick_count();
            pwb_frame_pack_05();
        }
    }
}
