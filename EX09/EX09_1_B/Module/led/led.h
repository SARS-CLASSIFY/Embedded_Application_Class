#ifndef _LED_H
#define _LED_H
#include "main.h"
#include "gpio.h"

#define KEY5 HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin);
#define KEY6 HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin);


void Set_leds(uint8_t sta);
uint8_t Key_scan(void);

#endif

