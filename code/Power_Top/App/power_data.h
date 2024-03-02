#ifndef __POWER_DATA_H
#define __POWER_DATA_H
#include "gd32e23x.h"

enum
{
    ALARM_NULL,
    ALARM_OVER_MAX_VOLTAGE,         // 过压,最大输出电压
    ALARM_OVER_MAX_CURRENT,         // 过流,最大输出电流
    ALARM_OVER_MAX_TEMP,            // 过温,最大温度
    ALARM_OVER_MAX_POWER,           // 过功率,最大输出功率
    ALARM_OVER_PROTECT_VOLTAGE,     // 过保护电压
    ALARM_OVER_PROTECT_CURRENT,     // 过保护电流
    ALARM_SC8815_OTP,               // 发生 OTP(过温) 故障中断
    ALARM_SC8815_SHORT,             // 发生短路故障中断
};

struct power_data_t
{
    // 开关状态 0:关 1:开
    uint8_t switch_status;

    // 模式 0:CC 1:CV
    uint8_t cc_cv;

    // 设置电压
    uint16_t set_voltage;
    int8_t  set_v_h;
    int8_t  set_v_l;

    // 设置电流
    uint16_t set_current;
    int8_t  set_i_h;
    int8_t  set_i_l;

    
    uint8_t alarm;    

    // INA226 读取部分
    // 读取电压
    uint16_t ina_read_voltage;
    // 读取电流
    uint16_t ina_read_current;
    // 读取功率
    uint16_t ina_read_power;

    // SC8815 读取部分
    uint16_t sc8815_vbus_voltage;
    uint16_t sc8815_vbus_current;

    uint16_t sc8815_vbat_voltage;
    uint16_t sc8815_vbat_current;

    uint8_t  sc8815_interrupt;
    
    // 输入电压
    uint16_t input_voltage;

    // NTC 温度ADC
    int8_t ntc_temp;
    // CPU 温度
    int8_t cpu_temp;

    // 上报间隔时间 ms
    uint16_t report_interval;


    // GUI 当前模式 0：初始化 1：电流电压显示模式 :2:设置模式
    uint8_t gui_mode;

    // 设置模式当前在设置哪个条目: 0:CC,CV  1:电压整数部分  2:电压小数部分,  3:电流整数, 4:电流小数部分
    uint8_t gui_set_mode_id;

    uint8_t set_mode_change_flag;


};
extern struct power_data_t power_data;
#endif