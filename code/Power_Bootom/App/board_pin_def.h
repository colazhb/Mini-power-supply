
#ifndef __BOARD_PIN_DEF_H__
#define __BOARD_PIN_DEF_H__

#include "gd32e23x.h"
#include <stdio.h>
/*
    // 通讯口
    PA2 COM_TX
    PA3 COM_RX
   
    // 温度采样
    PA4 TEMP_ADC

    // 电源输入
    PA5 VIN_ADC

    // INA226 电压电流采样
    PA7 INA_ALERT
    PB0 INA_SCL
    PB1 INA_SDA

    // SC8815 升降压控制
    PB7 SC_SDA
    PB6 SC_SCL
    PB5 SC_PSTOP
    PB4 SC_CE
    PA6 SC_INT

    // 电源输出控制
    PA8 OUT_CTL
    
    // 调试
    PA15 MCU_RUN
    PA10 USART0_RX
    PA9 USART0_TX
   
*/

// 温度采样
#define TEMP_ADC_PIN                GPIO_PIN_4
#define TEMP_ADC_GPIO_PORT          GPIOA

// 电源输入电压采样
#define VIN_ADC_PIN                 GPIO_PIN_5
#define VIN_ADC_GPIO_PORT           GPIOA

// INA226 电压电流采样
#define INA_ALERT_PIN               GPIO_PIN_7
#define INA_ALERT_GPIO_PORT         GPIOA

#define INA_SCL_PIN                 GPIO_PIN_0
#define INA_SCL_GPIO_PORT           GPIOB

#define INA_SDA_PIN                 GPIO_PIN_1
#define INA_SDA_GPIO_PORT           GPIOB

// SC8815 升降压控制
#define SC_SDA_PIN                  GPIO_PIN_7
#define SC_SDA_GPIO_PORT            GPIOB

#define SC_SCL_PIN                  GPIO_PIN_6
#define SC_SCL_GPIO_PORT            GPIOB

#define SC_PSTOP_PIN                GPIO_PIN_5
#define SC_PSTOP_GPIO_PORT          GPIOB

#define SC_CE_PIN                   GPIO_PIN_4
#define SC_CE_GPIO_PORT             GPIOB

#define SC_INT_PIN                  GPIO_PIN_6
#define SC_INT_GPIO_PORT            GPIOA

// 电源输出控制
#define OUT_CTL_PIN                 GPIO_PIN_8
#define OUT_CTL_GPIO_PORT           GPIOA

// 调试
#define MCU_RUN_PIN                 GPIO_PIN_15
#define MCU_RUN_GPIO_PORT           GPIOA

#define USART0_RX_PIN               GPIO_PIN_10
#define USART0_RX_GPIO_PORT         GPIOA

#define USART0_TX_PIN               GPIO_PIN_9
#define USART0_TX_GPIO_PORT         GPIOA

// 通讯口
#define COM_TX_PIN                  GPIO_PIN_2
#define COM_TX_GPIO_PORT            GPIOA

#define COM_RX_PIN                  GPIO_PIN_3
#define COM_RX_GPIO_PORT            GPIOA

#endif // __BOARD_PIN_DEF_H__