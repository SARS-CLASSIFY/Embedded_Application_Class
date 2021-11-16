#ifndef _LED_H
#define _LED_H
#include "main.h"
#include "gpio.h"

#define KEY1 0x01
#define KEY2 0x02
#define KEY3 0x04
#define KEY4 0x08
#define KEY5 0x10
#define KEY6 0x20


#define KEY1_READ HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin)
#define KEY2_READ HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin)
#define KEY3_READ HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin)
#define KEY4_READ HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin)
#define KEY5_READ HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin)
#define KEY6_READ HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin)


void Set_leds(uint8_t sta);
uint8_t ScanKey(void);

#endif

