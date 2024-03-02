#include "protect.h"
#include "board_pin_def.h"
#include "gd32e23x_libopt.h"
#include "power_data.h"


void protect_pwr(uint8_t alarm)
{
    // 关闭输出
    sc8815_user_power_on_off(0);

    // 设置保护值
    power_data.alarm = alarm;
};

// 保护检测(最大电压，最大电流,最大温度)
void power_protect_check()
{
    // 基本保护检测(保护底线)
    // 过最大输出电压
    if((power_data.ina_read_voltage >= MAX_OUT_VOLTAGE))
    {
        // 关闭输出
        printf("over max voltage: ina:%d sc8815:%d\r\n", power_data.ina_read_voltage, power_data.sc8815_vbus_voltage);
        protect_pwr(ALARM_OVER_MAX_VOLTAGE);
    }
    else if ((power_data.ina_read_current >= MAX_OUT_CURRENT))
    {
        // 关闭输出
        printf("over max current: ina:%d sc8815:%d\r\n", power_data.ina_read_current, power_data.sc8815_vbus_current);
        protect_pwr(ALARM_OVER_MAX_CURRENT);
    }
    else if (power_data.ntc_temp >= MAX_OUT_TEMP )
    {
        // 关闭输出
        //printf("over max temp: ntc:%d cpu:%d\r\n", power_data.ntc_temp, power_data.cpu_temp);
       protect_pwr(ALARM_OVER_MAX_TEMP);
    }
    else if (power_data.ina_read_power  >= (MAX_OUT_POWER*1000) )
    {
        // 关闭输出
        printf("over max power: %d\r\n", power_data.ina_read_power);
        protect_pwr(ALARM_OVER_MAX_POWER);
    }
    else
    {
        power_data.alarm = ALARM_NULL;
    }

};
