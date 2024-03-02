
#ifndef SC8815_USER_H
#define SC8815_USER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//包含标准类型定义头文件
#include <stdint.h>

//使用此库需要提供以下外部函数,这是SC8815库需要使用的函数
void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData);   //通过I2C向设备寄存器写一个字节
uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress);                   //通过I2C从设备寄存器读一个字节
void SoftwareDelay(uint8_t ms);                                                      //软件延时毫秒

void sc8815_user_set(uint8_t mode, uint16_t voltage, uint16_t current);

void sc8815_user_init();

void sc8815_user_loop();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // I2C_ULIB_H