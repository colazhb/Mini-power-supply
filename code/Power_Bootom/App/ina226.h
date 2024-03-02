
#ifndef INA226_H
#define INA226_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//包含标准类型定义头文件
#include <stdint.h>


#define INA226_ADDRESS              (0x40)  //7bit address  A0:0  A1:0

#define INA226_REG_CONFIG           (0x00)             // 配置寄存器（00h）（读/写）
#define INA226_REG_SHUNTVOLTAGE     (0x01)             // 分流电压寄存器（01h）（只读）
#define INA226_REG_BUSVOLTAGE       (0x02)             // 总线电压寄存器（02h）（只读）
#define INA226_REG_POWER            (0x03)             // 功率寄存器（03h）（只读）

//如果启用了平均，该寄存器将显示平均值。根据等式3，通过将并联电压寄存器中的十进制值与校准寄存器的十进制数值相乘来计算电流寄存器的值。

#define INA226_REG_CURRENT          (0x04)             // 平均寄存器（04h）（只读）

#define INA226_REG_CALIBRATION      (0x05)             // 校准寄存器
#define INA226_REG_MASKENABLE       (0x06)             // 中断使能寄存器
#define INA226_REG_ALERTLIMIT       (0x07)             // 中断上下限寄存器

#define INA226_BIT_SOL              (0x8000)           // 电源过压限制 并联电压上限（SOL）
#define INA226_BIT_SUL              (0x4000)           // 电源欠压限制 并联电压下限（SUL）
#define INA226_BIT_BOL              (0x2000)           // 总线过压限制 母线电压上限（BOL）
#define INA226_BIT_BUL              (0x1000)           // 总线欠压限制 总线电压下限（BUL）
#define INA226_BIT_POL              (0x0800)           // 功率过载限制 功率上限（POL）
#define INA226_BIT_CNVR             (0x0400)           // 转换就绪限制
#define INA226_BIT_AFF              (0x0010)           // 警报标志位
#define INA226_BIT_CVRF             (0x0008)           // 转换就绪标志位
#define INA226_BIT_OVF              (0x0004)           // 数学溢出标志位
#define INA226_BIT_APOL             (0x0002)           // 警报极性
#define INA226_BIT_LEN              (0x0001)           // 警报锁存,是用于设置报警引脚和报警标志位在报警消失后是否保持有效的功能

typedef enum
{
    INA226_AVERAGES_1             = 0b000,             // 采样次数
    INA226_AVERAGES_4             = 0b001,            
    INA226_AVERAGES_16            = 0b010,
    INA226_AVERAGES_64            = 0b011,
    INA226_AVERAGES_128           = 0b100,
    INA226_AVERAGES_256           = 0b101,
    INA226_AVERAGES_512           = 0b110,
    INA226_AVERAGES_1024          = 0b111
} ina226_averages_t;


// 母线电压转换时间
typedef enum
{
    INA226_BUS_CONV_TIME_140US    = 0b000,
    INA226_BUS_CONV_TIME_204US    = 0b001,
    INA226_BUS_CONV_TIME_332US    = 0b010,
    INA226_BUS_CONV_TIME_588US    = 0b011,
    INA226_BUS_CONV_TIME_1100US   = 0b100,
    INA226_BUS_CONV_TIME_2116US   = 0b101,
    INA226_BUS_CONV_TIME_4156US   = 0b110,
    INA226_BUS_CONV_TIME_8244US   = 0b111
} ina226_busConvTime_t;

// 分流电压转换时间
typedef enum
{
    INA226_SHUNT_CONV_TIME_140US   = 0b000,
    INA226_SHUNT_CONV_TIME_204US   = 0b001,
    INA226_SHUNT_CONV_TIME_332US   = 0b010,
    INA226_SHUNT_CONV_TIME_588US   = 0b011,
    INA226_SHUNT_CONV_TIME_1100US  = 0b100,
    INA226_SHUNT_CONV_TIME_2116US  = 0b101,
    INA226_SHUNT_CONV_TIME_4156US  = 0b110,
    INA226_SHUNT_CONV_TIME_8244US  = 0b111
} ina226_shuntConvTime_t;

typedef enum
{
    INA226_MODE_POWER_DOWN      = 0b000,              // 关闭
    INA226_MODE_SHUNT_TRIG      = 0b001,              // 分流触发
    INA226_MODE_BUS_TRIG        = 0b010,              // 母线触发
    INA226_MODE_SHUNT_BUS_TRIG  = 0b011,              // 分流母线触发
    INA226_MODE_ADC_OFF         = 0b100,              // ADC关闭
    INA226_MODE_SHUNT_CONT      = 0b101,              // 分流连续
    INA226_MODE_BUS_CONT        = 0b110,              // 母线连续
    INA226_MODE_SHUNT_BUS_CONT  = 0b111,              // 分流母线连续
} ina226_mode_t;

struct ina226_t
{
    uint8_t addr;
    float currentLSB, powerLSB;
	float vShuntMax, vBusMax, rShunt;

    // 回调函数
    // i2c 写16位数据
    uint8_t (*write16)(uint8_t addr, uint8_t reg, uint16_t data);
    // i2c 读16位数据
    uint8_t (*read16)(uint8_t addr, uint8_t reg, uint16_t *data);
};
/* 
 * @brief 初始化ina226
 * @param dev ina226设备\
 * @avg 采样次数
 * @busConvTime 母线电压转换时间
 * @shuntConvTime 分流电压转换时间
 * @mode 工作模式
 * @return 无
 * @note 无
 */
void ina226_config(struct ina226_t *dev, ina226_averages_t avg, ina226_busConvTime_t busConvTime, ina226_shuntConvTime_t shuntConvTime, ina226_mode_t mode);
uint16_t ina226_config_read(struct ina226_t *dev);

/* 
 * @brief 校准ina226
 * @param dev ina226设备
 * @param rShunt 分流电阻 单位 Ω
 * @param iMaxExpected 最大电流 单位 A
 * @return 无
 * @note 无
 */
void ina226_calibrate(struct ina226_t *dev, float rShunt, float iMaxExpected);


float ina226_get_max_possible_current(struct ina226_t *dev);
float ina226_get_max_current(struct ina226_t *dev);
float ina226_get_max_power(struct ina226_t *dev);
float ina226_get_max_shunt_voltage(struct ina226_t *dev);

float ina226_read_bus_voltage(struct ina226_t *dev);
float ina226_read_bus_power(struct ina226_t *dev);
float ina226_read_shunt_voltage(struct ina226_t *dev);
float ina226_read_shunt_current(struct ina226_t *dev);
uint16_t ina226_get_manufacturer_id(struct ina226_t *dev);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // INA226_H