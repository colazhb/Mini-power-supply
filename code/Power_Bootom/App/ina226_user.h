
#ifndef INA226_USER_H
#define INA226_USER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//包含标准类型定义头文件
#include <stdint.h>

#include "ina226.h"

void ina226_user_int();
void ina226_user_loop();
extern struct ina226_t ina226_obj;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // INA226_USER_H