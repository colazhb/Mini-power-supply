#include "shell_user.h"
#include "board_pin_def.h"
#include "shell.h"
#include "mcu_flash.h"
#include "sc8815_user.h"
#include "power_data.h"
SHELL_TypeDef shell;
void user_shellWrite(const char ch)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE));
};

void shell_user_put_char(uint8_t ch)
{
    shellInput(&shell, ch);
};

void shell_user_init()
{
    //shell.read = shellRead;  //shellInput
    shell.write = user_shellWrite;
    shellInit(&shell);
};

// -------------------- 命令 -------------------------------


// 设置输出电压
void set_v(int argc, char *agrv[])
{
    
    if (argc != 2)
    {
        printf(&shell, "参数错误\r\n");
        return;
    }
    uint16_t v = atoi(agrv[1]);
    power_data.set_switch_status = 3;
    power_data.set_voltage = v;
    sc8815_user_set(power_data.set_cc_cv, power_data.set_voltage, power_data.set_current);
    sc8815_user_power_on_off(power_data.set_switch_status);

    printf("set power %d\r\n", v);
}
SHELL_EXPORT_CMD(set_v, set_v, set_v);

//打印 INA226 电压电流功率
void ina226_print()
{
    ina226_user_print();
}
SHELL_EXPORT_CMD(ina226_print, ina226_print, ina226_print);
/*
 校准电压值,输入3.3v时显示的电压，测量到的电压，30v时显示的电压，测量到的电压，计算出斜率
 输入 3v3_dis,3v3_mes,30v_dis,30v_mes
 
*/
void mes_v(int argc, char *agrv[])
{
    if (argc != 5)
    {
        printf(&shell, "参数错误\r\n");
        return;
    }
    float v1_dis = atof(agrv[1]);
    float v1_mes = atof(agrv[2]);
    float v2_dis = atof(agrv[3]);
    float v2_mes = atof(agrv[4]);
    float k = (v1_dis - v2_dis) / (v1_mes - v2_mes);
    char buf[32];
    sprintf(buf, "k = %f\r\n", k);
  
}
SHELL_EXPORT_CMD(mes_v, mes_v, mes_v);

// FLASH 读写测试
void flash_test()
{
    uint8_t data[]= "hello";
    uint8_t buff[10] = {0};
    // 1.写入 hello
    mcu_flash_write(data, 5);
    printf("flash write: %s\r\n", data);

    // 2.读取 hello
    mcu_flash_read(buff, 5);
    printf("flash read: %s\r\n", buff);

    // 3.擦擦
    mcu_flash_erase();
    printf("flash erase\r\n");

    // 4.再读取
    memset(buff, 0, 10);
    mcu_flash_read(buff, 5);
    printf("flash read: %s\r\n", buff);

}
SHELL_EXPORT_CMD(flash_test, flash_test, flash_test);


// adc 测试
void adc_test()
{
    adc_conv_print();
}
SHELL_EXPORT_CMD(adc_test, adc_test, adc_test);