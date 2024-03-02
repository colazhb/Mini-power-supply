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
|指令|输出状态|模式|告警|电压|电流|输入电压|SC8815_VBUS电压|SC8815_VBUS电流|SC8815中断|NTC温度|CPU温度|
|0x05|1字节  |1字节|1字节|2字节|2字节|




*/
struct frame_obj frame_pwb;
lwrb_t buff;
uint8_t buff_data[128];

void pwb_set_mode(uint8_t mode, uint16_t voltage, uint16_t current)
{
    uint8_t data[6] = {0};
    data[0] = 0x01;
    data[1] = mode;
    data[2] = (voltage >> 8) & 0xff;
    data[3] = voltage & 0xff;
    data[4] = (current >> 8) & 0xff;
    data[5] = current & 0xff;

    frame_pack(&frame_pwb, data, 6);
};

void pwb_set_protect(uint16_t voltage, uint16_t current)
{
    uint8_t data[5] = {0};
    data[0] = 0x02;
    data[1] = (voltage >> 8) & 0xff;
    data[2] = voltage & 0xff;
    data[3] = (current >> 8) & 0xff;
    data[4] = current & 0xff;

    frame_pack(&frame_pwb, data, 5);
};
 // bit0 PSTOP 引脚, 1:打开 0:关闭
// bit1 MOS_CTL 引脚, 1:打开 0:关闭
void pwb_set_output(uint8_t output)
{
    uint8_t send_val =0;
    if (output ==1)
    {
        send_val = 0x03;  //
        /* code */
    }
    
    uint8_t data[2] = {0};
    data[0] = 0x03;
    data[1] = send_val;

    frame_pack(&frame_pwb, data, 2);
};
void pwb_set_interval(uint8_t interval)
{
    uint8_t data[2] = {0};
    data[0] = 0x04;
    data[1] = interval;

    frame_pack(&frame_pwb, data, 2);
};


void pwb_get_data()
{
    uint8_t data[1] = {0};
    data[0] = 0x05;
    frame_pack(&frame_pwb, data, 1);
};

void pwb_frame_callback(uint8_t *data, uint8_t len)
{
    printf("pwb_frame_callback: ");
    for (int i = 0; i < len; i++)
    {
        printf("%02x ", data[i]);
    }
    // 解码数据
    if(data[0] == 0x05)
    {

        power_data.switch_status = data[1];
        power_data.cc_cv = data[2];
        power_data.alarm = data[3];

        // INA
        power_data.ina_read_voltage = (data[4]<<8)|data[5];
        power_data.ina_read_current = (data[6]<<8)|data[7];
        power_data.input_voltage = (data[8]<<8)|data[9];

        // sc8815 读取部分
        power_data.sc8815_vbus_voltage = (data[10]<<8)|data[11];
        power_data.sc8815_vbus_current = (data[12]<<8)|data[13];

        power_data.sc8815_interrupt = data[14];

        power_data.ntc_temp =  data[15];



    }
    printf("\r\n");
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
    frame_loop(&frame_pwb);   
}
