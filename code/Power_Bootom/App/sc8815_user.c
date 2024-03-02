#include "SC8815.h"
#include "sc8815_user.h"
#include "i2c_ulib.h"
#include "ina226_user.h"
#include "ina226.h"
#include "power_data.h"
#include "gd32e23x.h"
#include "board_pin_def.h"
void sc8815_scl_write(uint8_t data)
{
    gpio_bit_write(SC_SCL_GPIO_PORT, SC_SCL_PIN, data);
};
void sc8815_sda_write(uint8_t data)
{
    gpio_bit_write(SC_SDA_GPIO_PORT, SC_SDA_PIN, data);
};
void sc8815_sda_dir(uint8_t data)
{
    if (data)
    {
        gpio_mode_set(SC_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SC_SDA_PIN);
    }
    else
    {
        gpio_mode_set(SC_SDA_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SC_SDA_PIN);
    }
};
uint8_t sc8815_sda_read()
{
    return gpio_input_bit_get(SC_SDA_GPIO_PORT, SC_SDA_PIN);
};
void sc8815_i2c_delay(uint32_t x)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < 1000; j++)
            __NOP();
    }
}
struct i2c_ulib_dev_t sc8815_dev;
void sc8815_i2c_init()
{
    // SCL -> PB6
    // SDA -> PB7
    // PSTOP -> PB8
    // CE -> PB9
    // INT -> PB10
    // PWR CTL ->PB13

    gpio_mode_set(SC_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_SCL_PIN);
    gpio_output_options_set(SC_SCL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SC_SCL_PIN);

    gpio_mode_set(SC_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_SDA_PIN);
    gpio_output_options_set(SC_SDA_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SC_SDA_PIN);

    gpio_mode_set(SC_PSTOP_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_PSTOP_PIN);
    gpio_output_options_set(SC_PSTOP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SC_PSTOP_PIN);

    gpio_mode_set(SC_CE_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SC_CE_PIN);
    gpio_output_options_set(SC_CE_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SC_CE_PIN);

    gpio_mode_set(OUT_CTL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, OUT_CTL_PIN);
    gpio_output_options_set(OUT_CTL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, OUT_CTL_PIN);

    // 中断引脚
    gpio_mode_set(SC_INT_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SC_INT_PIN);

    // 关闭输出
    // PWR CTL 低
    gpio_bit_reset(OUT_CTL_GPIO_PORT, OUT_CTL_PIN);
    // PSTOP 高
    gpio_bit_set(SC_PSTOP_GPIO_PORT, SC_PSTOP_PIN);

    sc8815_dev.us_delay = sc8815_i2c_delay;
    sc8815_dev.scl_write = sc8815_scl_write;
    sc8815_dev.sda_write = sc8815_sda_write;
    sc8815_dev.sda_read = sc8815_sda_read;
    sc8815_dev.sda_dir = sc8815_sda_dir;
}
// 使用此库需要提供以下外部函数,这是SC8815库需要使用的函数
void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData) // 通过I2C向设备寄存器写一个字节
{
    // 写数据
    uint8_t data[2] = {RegAddress, ByteData};
    i2c_ulib_write(&sc8815_dev, SlaveAddress, data, 2, 1);
}
uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress) // 通过I2C从设备寄存器读一个字节
{
    uint8_t data;
    // 读数据
    i2c_ulib_write(&sc8815_dev, SlaveAddress, &RegAddress, 1, 0);
    i2c_ulib_read(&sc8815_dev, SlaveAddress, &data, 1);
    return data;
}
void SoftwareDelay(uint8_t ms) // 软件延时毫秒
{
    for (uint8_t i = 0; i < ms; i++)
    {
        for (uint16_t j = 0; j < 1000; j++)
        {
            __NOP();
        }
    }
}

// PSTOP 引脚
void SC8815_PSTOP_Set(uint8_t data)
{
    gpio_bit_write(SC_PSTOP_GPIO_PORT, SC_PSTOP_PIN, data);
}

// CE 引脚
void SC8815_CE_Set(uint8_t data)
{
    gpio_bit_write(SC_CE_GPIO_PORT, SC_CE_PIN, data);
}

// PWR_CTL
void SC8815_PWR_CTL_Set(uint8_t data)
{
    gpio_bit_write(OUT_CTL_GPIO_PORT, OUT_CTL_PIN, data);
}

SC8815_BatteryConfigTypeDef SC8815_BatteryConfigStruct = {0};
SC8815_HardwareInitTypeDef SC8815_HardwareInitStruct = {0};
SC8815_InterruptStatusTypeDef SC8815_InterruptMaskInitStruct = {0};
void sc8815_user_init()
{
    uint8_t send[] = {0x00};
    sc8815_i2c_init();
    // 读取芯片数据
    for (int i = 0; i < 0x10; i++)
    {
        uint8_t data = I2C_ReadRegByte(SC8815_ADDR, i);
        printf("sc8815 data: %x\r\n", data);
    }

    SoftwareDelay(10); // 必要的启动延时

    // 配置 SC8815 电池参数选项
    SC8815_BatteryConfigStruct.IRCOMP = SCBAT_IRCOMP_20mR;
    SC8815_BatteryConfigStruct.VBAT_SEL = SCBAT_VBAT_SEL_Internal;
    SC8815_BatteryConfigStruct.CSEL = SCBAT_CSEL_4S;
    SC8815_BatteryConfigStruct.VCELL = SCBAT_VCELL_4v25;
    SC8815_BatteryConfig(&SC8815_BatteryConfigStruct);

    // 配置 SC8815 硬件参数选项
    SC8815_HardwareInitStruct.IBAT_RATIO = SCHWI_IBAT_RATIO_12x;
    SC8815_HardwareInitStruct.IBUS_RATIO = SCHWI_IBUS_RATIO_6x;
    SC8815_HardwareInitStruct.VBAT_RATIO = SCHWI_VBAT_RATIO_12_5x;
    SC8815_HardwareInitStruct.VBUS_RATIO = SCHWI_VBUS_RATIO_12_5x;
    SC8815_HardwareInitStruct.VINREG_Ratio = SCHWI_VINREG_RATIO_100x;
    SC8815_HardwareInitStruct.SW_FREQ = SCHWI_FREQ_450KHz;
    SC8815_HardwareInitStruct.DeadTime = SCHWI_DT_20ns;
    SC8815_HardwareInitStruct.ICHAR = SCHWI_ICHAR_IBUS;
    SC8815_HardwareInitStruct.TRICKLE = SCHWI_TRICKLE_Disable;
    SC8815_HardwareInitStruct.TERM = SCHWI_TERM_Disable;
    SC8815_HardwareInitStruct.FB_Mode = SCHWI_FB_External;
    SC8815_HardwareInitStruct.TRICKLE_SET = SCHWI_TRICKLE_SET_70;
    SC8815_HardwareInitStruct.OVP = SCHWI_OVP_Disable;
    SC8815_HardwareInitStruct.DITHER = SCHWI_DITHER_Disable;
    SC8815_HardwareInitStruct.SLEW_SET = SCHWI_SLEW_1mV_us;
    SC8815_HardwareInitStruct.ADC_SCAN = SCHWI_ADC_Enable;
    SC8815_HardwareInitStruct.ILIM_BW = SCHWI_ILIM_BW_5KHz;
    SC8815_HardwareInitStruct.LOOP = SCHWI_LOOP_Normal;
    SC8815_HardwareInitStruct.ShortFoldBack = SCHWI_SFB_Enable;
    SC8815_HardwareInitStruct.EOC = SCHWI_EOC_1_25;
    SC8815_HardwareInitStruct.PFM = SCHWI_PFM_Disable;
    SC8815_HardwareInit(&SC8815_HardwareInitStruct);

    // 配置 SC8815 中断屏蔽选项
    SC8815_InterruptMaskInitStruct.AC_OK = sENABLE;
    SC8815_InterruptMaskInitStruct.INDET = sENABLE;
    SC8815_InterruptMaskInitStruct.VBUS_SHORT = sENABLE;
    SC8815_InterruptMaskInitStruct.OTP = sENABLE;
    SC8815_InterruptMaskInitStruct.EOC = sENABLE;
    SC8815_ConfigInterruptMask(&SC8815_InterruptMaskInitStruct);
    /***现在可以设置 PSTOP 引脚为低, 启动 SC8815 功率转换****/
    SC8815_ADC_Enable();

    SC8815_VINREG_SetVoltage(5000);
    // SC8815_OTG_Disable();

    /*** 示例2, 设置为反向放电模式,电池和 VBUS 限流 3A, 输出电压 设置为 12V ****/
    SC8815_SetBatteryCurrLimit(5000);
    SC8815_SetBusCurrentLimit(5000);
    SC8815_SetOutputVoltage(3300);
    SC8815_OTG_Enable();

    SC8815_PSTOP_Set(0);  // 关闭输出
    SoftwareDelay(10);
}

void sc8815_user_set(uint8_t mode, uint16_t voltage, uint16_t current)
{
    printf("set %d %d %d", mode, voltage, current);
    SC8815_SetOutputVoltage(voltage);
}
void sc8815_protect_set(uint16_t voltage, uint16_t current)
{
    SC8815_SetBatteryCurrLimit(current);
    SC8815_SetBusCurrentLimit(current);
};

// bit0 PSTOP 引脚, 1:打开 0:关闭
// bit1 MOS_CTL 引脚, 1:打开 0:关闭
void sc8815_user_power_on_off(uint8_t data)
{
    printf("power on off %d\r\n", data);
    uint8_t pstop = 0;
    uint8_t pwr_ctl = 0;

    pstop = (data & 0x01) ? 0 : 1;
    pwr_ctl = (data >> 1) & 0x01;

    SC8815_PSTOP_Set(pstop);
    SC8815_PWR_CTL_Set(pwr_ctl);
}

uint8_t dbg_sc8815_print_flag = 0;
void sc8815_user_debug(uint8_t data)
{
    dbg_sc8815_print_flag = data;
};
uint16_t VbusVolt = 0;
uint16_t VbusCurr = 0;
uint16_t BattVolt = 0;
uint16_t BattCurr = 0;

void sc8815_user_loop()
{
    // SC8815中断引脚
    if (gpio_input_bit_get(SC_INT_GPIO_PORT, SC_INT_PIN) == 1)
    {
        SC8815_ReadInterrupStatus(&SC8815_InterruptMaskInitStruct); // MCU 收到 SC8815 中断后调用此函数读 SC8815 中断 (读中断状态后将清除中断状态位)

        if (SC8815_InterruptMaskInitStruct.AC_OK == 1) // 检查 AC_OK 中断是否触发
        {
           // printf("AC_OK\n");
            // AC_OK 中断处理代码
            power_data.sc8815_interrupt |= INTERR_ACIN;
        }
        else if (SC8815_InterruptMaskInitStruct.EOC == 1)
        {
            printf("EOC\n");
            // EOC 中断处理代码
            power_data.sc8815_interrupt |= INTERR_EOC;
        }
        else if (SC8815_InterruptMaskInitStruct.INDET == 1)
        {
            printf("INDET\n");
            // INDET 中断处理代码
            power_data.sc8815_interrupt |= INTERR_INDET;
        }
        else if (SC8815_InterruptMaskInitStruct.VBUS_SHORT == 1)
        {
            printf("VBUS_SHORT\n");
            // VBUS_SHORT 中断处理代码
            power_data.sc8815_interrupt |= INTERR_VBUS_SHORT;
            sc8815_user_power_on_off(0);
        }
        else if (SC8815_InterruptMaskInitStruct.OTP == 1)
        {
            printf("OTP\n");
            // OTP 中断处理代码
            power_data.sc8815_interrupt |= INTERR_OTP;
            sc8815_user_power_on_off(0);
        }
        else
        {
            /* code */
        }
    }

    VbusVolt = SC8815_Read_VBUS_Voltage();
    VbusCurr = SC8815_Read_VBUS_Current();
    BattVolt = SC8815_Read_BATT_Voltage();
    BattCurr = SC8815_Read_BATT_Current();

    power_data.sc8815_vbus_voltage = VbusVolt;
    power_data.sc8815_vbus_current = VbusCurr;
    power_data.sc8815_vbat_voltage = BattVolt;
    power_data.sc8815_vbat_current = BattCurr;

    if (dbg_sc8815_print_flag)
    {
        printf("SC8815 VbusVolt:%d VbusCurr:%d BattVolt:%d BattCurr:%d\r\n", VbusVolt, VbusCurr, BattVolt, BattCurr);
    }
}