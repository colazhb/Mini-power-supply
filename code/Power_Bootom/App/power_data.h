#ifndef __POWER_DATA_H
#define __POWER_DATA_H
#include "gd32e23x.h"

#define MAX_OUT_VOLTAGE 35000      // 35V
#define MAX_OUT_CURRENT 6000       // 6A
#define MAX_OUT_TEMP     90        // 90C
#define MAX_OUT_POWER    65        // 65W

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

enum
{
    INTERR_ACIN  = 0x01,
    INTERR_INDET = 0x02,
    INTERR_VBUS_SHORT = 0x04,
    INTERR_OTP = 0x08,
    INTERR_EOC = 0x10,
};


struct power_data_t
{  // 开关状态 bit0 PSTOP bit1 MOS_CTL
    uint8_t set_switch_status;

    // 模式 0:CC 1:CV
    uint8_t set_cc_cv;

    // 设置电压
    uint16_t set_voltage;

    // 设置电流
    uint16_t set_current;

    // 设置保护电压
    uint16_t set_protect_voltage;

    // 设置保护电流
    uint16_t set_protect_current;

    // 报警状态 
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


    // 上报间隔时间 ms
    uint16_t report_interval;
};
extern struct power_data_t power_data;
#endif