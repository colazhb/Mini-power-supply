#include "gd32e23x.h"
#include "ina226.h"
#include "ina226_user.h"
#include "i2c_ulib.h"
#include "board_pin_def.h"
#include "power_data.h"
struct ina226_t ina226_obj;

void ina226_scl_write(uint8_t data)
{
     gpio_bit_write(INA_SCL_GPIO_PORT, INA_SCL_PIN, data);
};
void ina226_sda_write(uint8_t data)
{
    gpio_bit_write(INA_SDA_GPIO_PORT, INA_SDA_PIN, data);

};
void ina226_sda_dir(uint8_t data)
{
   if(data)
    {
       gpio_mode_set(INA_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, INA_SDA_PIN);
    }
    else
    {
        gpio_mode_set(INA_SDA_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, INA_SDA_PIN);
    }

};
uint8_t ina226_sda_read()
{
    return gpio_input_bit_get(INA_SDA_GPIO_PORT, INA_SDA_PIN);
};
void ina226_i2c_delay(uint32_t x)
{
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < 1000; j++)
            __NOP();
    }
}
struct i2c_ulib_dev_t ina226_i2c_dev;
void ina226_i2c_init()
{
    // SCL -> PB11
    // SDA -> PB12
    gpio_mode_set(INA_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, INA_SCL_PIN);
    gpio_output_options_set(INA_SCL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, INA_SCL_PIN);

    gpio_mode_set(INA_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, INA_SDA_PIN);
    gpio_output_options_set(INA_SDA_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, INA_SDA_PIN);


    ina226_i2c_dev.us_delay = ina226_i2c_delay;
    ina226_i2c_dev.scl_write = ina226_scl_write;
    ina226_i2c_dev.sda_write = ina226_sda_write;
    ina226_i2c_dev.sda_read = ina226_sda_read;
    ina226_i2c_dev.sda_dir = ina226_sda_dir;
    i2c_ulib_feq(&ina226_i2c_dev, 100);
    
}




uint8_t ina226_write16(uint8_t addr, uint8_t reg, uint16_t data)
{
    uint8_t buf[3];
    buf[0] = reg;
    buf[1] = data >> 8;
    buf[2] = data & 0xff;
   // i2c_ulib_write(&ina226_i2c_dev, addr, buf, 1 , 0);
    i2c_ulib_write(&ina226_i2c_dev, addr, buf, 3 , 1);
};
uint8_t ina226_read16(uint8_t addr, uint8_t reg, uint16_t *data)
{
    uint8_t buf[2];
    buf[0] = reg;
    i2c_ulib_write(&ina226_i2c_dev, addr, buf, 1 , 0);
    i2c_ulib_read(&ina226_i2c_dev, addr, buf, 2);
    *data = buf[0] << 8 | buf[1];
};

void ina226_user_int()
{
    ina226_i2c_init();
    ina226_obj.addr = 0x40;
    ina226_obj.write16 = ina226_write16;
    ina226_obj.read16 = ina226_read16;

    ina226_init(&ina226_obj);
    int16_t defcfg = ina226_config_read(&ina226_obj);
    printf("ina226 def config 0x4127 : read: %x\r\n", defcfg);


    ina226_config(&ina226_obj, INA226_AVERAGES_64, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    uint16_t cfg = ina226_config_read(&ina226_obj);
    printf("ina226 set config: %x\r\n", cfg);

    
    uint16_t xx = ina226_get_manufacturer_id(&ina226_obj);
    printf("ina226_get_manufacturer_id: %x\r\n", xx);

    // 05 READ
    uint16_t cal = ina226_calibrate_read(&ina226_obj);
    printf("ina226_calibrate_read: %x\r\n", cal);

    // 10欧姆采样电阻 5A 最大电流
    ina226_calibrate(&ina226_obj, 0.01, 5);
    cal = ina226_calibrate_read(&ina226_obj);
    printf("ina226_calibrate_read: %x\r\n", cal);

}

uint8_t ina226_dbg_flag = 0;

void ina226_user_loop()
{
    float bus_voltage = ina226_read_bus_voltage(&ina226_obj);
 
    float shunt_voltage = ina226_read_shunt_voltage(&ina226_obj);
 
    float current = ina226_read_shunt_current(&ina226_obj);
 
    float power = ina226_read_bus_power(&ina226_obj);

    power_data.ina_read_voltage =  (int32_t)(bus_voltage*1000);
    power_data.ina_read_current = (int32_t)(current*1000);
    power_data.ina_read_power = (int32_t)(power*1000);


    if(ina226_dbg_flag)
    {
        printf("bus_voltage: %d  current: %d power: %d\r\n", power_data.ina_read_voltage, power_data.ina_read_current,  power_data.ina_read_power );
    };
}

void ina226_user_print()
{
    ina226_dbg_flag =  !ina226_dbg_flag;
}