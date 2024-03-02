
#ifndef __BOARD_PIN_DEF_H__
#define __BOARD_PIN_DEF_H__

#include "gd32e23x.h"
#include <stdio.h>
// 按键 BT1 PA0 ,BT2 PA1

#define KEY1_PIN                    GPIO_PIN_0
#define KEY1_GPIO_PORT              GPIOA

#define KEY2_PIN                    GPIO_PIN_1
#define KEY2_GPIO_PORT              GPIOA

// 旋转编码器 EC_A PB5,EC_B PB4,EC_SW PB3
#define EC_A_PIN                    GPIO_PIN_5
#define EC_A_GPIO_PORT              GPIOB

#define EC_B_PIN                    GPIO_PIN_4
#define EC_B_GPIO_PORT              GPIOB

#define EC_SW_PIN                   GPIO_PIN_3
#define EC_SW_GPIO_PORT             GPIOB

// 通讯串口 COM_TX PA1 ,COM_RX PA2
#define COM_TX_PIN                  GPIO_PIN_2
#define COM_TX_GPIO_PORT            GPIOA

#define COM_RX_PIN                  GPIO_PIN_3
#define COM_RX_GPIO_PORT            GPIOA

// 调试-上位机串口与CH340X连接  TX PA9 ,RX PA10

#define DEBUG_TX_PIN                GPIO_PIN_9
#define DEBUG_TX_GPIO_PORT          GPIOA

#define DEBUG_RX_PIN                GPIO_PIN_10
#define DEBUG_RX_GPIO_PORT          GPIOA

// RGB LED LED_R PB7,LED_G PB6,LED_B PA15
#define LED_R_PIN                   GPIO_PIN_7
#define LED_R_GPIO_PORT             GPIOB

#define LED_G_PIN                   GPIO_PIN_6
#define LED_G_GPIO_PORT             GPIOB

#define LED_B_PIN                   GPIO_PIN_15
#define LED_B_GPIO_PORT             GPIOA

// BEEP PA8 TIME0 CH0
#define BEEP_PIN                    GPIO_PIN_8
#define BEEP_GPIO_PORT              GPIOA

// LCD 屏幕 LCD_CS PA4,LCD_SCL PA5, LCD_RS(DC) PA6,LCD_SDA PA7,LCD_RST PB0,LCD_PWM PB1
#define LCD_CS_PIN                  GPIO_PIN_4
#define LCD_CS_GPIO_PORT            GPIOA



#define LCD_RS_PIN                  GPIO_PIN_6
#define LCD_RS_GPIO_PORT            GPIOA


#define LCD_SCL_PIN                 GPIO_PIN_5
#define LCD_SCL_GPIO_PORT           GPIOA

#define LCD_SDA_PIN                 GPIO_PIN_7
#define LCD_SDA_GPIO_PORT           GPIOA

#define LCD_RST_PIN                 GPIO_PIN_0
#define LCD_RST_GPIO_PORT           GPIOB

#define LCD_PWM_PIN                 GPIO_PIN_1
#define LCD_PWM_GPIO_PORT           GPIOB

#endif // __BOARD_PIN_DEF_H__