
#ifndef I2C_ULIB_H
#define I2C_ULIB_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//包含标准类型定义头文件
#include <stdint.h>


struct i2c_ulib_dev_t
{
    uint8_t delay;
    // 延时回调
    void (*us_delay)(uint32_t);

    // SCL 写回调函数
    void (*scl_write)(uint8_t);
    // SDA 写回调函数
    void (*sda_write)(uint8_t);
    // SDA 读回调函数
    uint8_t (*sda_read)(void);
    // SDA 方向设置回调函数 0：输入 1：输出
    void (*sda_dir)(uint8_t);

};

void i2c_ulib_feq(struct i2c_ulib_dev_t *dev , uint8_t feq);
uint8_t i2c_ulib_write(struct i2c_ulib_dev_t *dev, uint8_t addr, uint8_t *data , uint16_t len , uint8_t stop);
uint8_t i2c_ulib_read(struct i2c_ulib_dev_t *dev, uint8_t addr, uint8_t *data , uint16_t len);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // I2C_ULIB_H