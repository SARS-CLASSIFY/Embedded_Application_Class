#include "led.h"



void Set_leds(uint8_t sta)
{
	HAL_GPIO_WritePin(L1_GPIO_Port,L1_Pin,(sta&0x01)? GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L2_GPIO_Port,L2_Pin,(sta&0x02)? GPIO_PIN_RESET:GPIO_PIN_SET);	
	HAL_GPIO_WritePin(L3_GPIO_Port,L3_Pin,(sta&0x04)? GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L4_GPIO_Port,L4_Pin,(sta&0x08)? GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L5_GPIO_Port,L5_Pin,(sta&0x10)? GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L6_GPIO_Port,L6_Pin,(sta&0x20)? GPIO_PIN_RESET:GPIO_PIN_SET);	
	HAL_GPIO_WritePin(L7_GPIO_Port,L7_Pin,(sta&0x40)? GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L8_GPIO_Port,L8_Pin,(sta&0x80)? GPIO_PIN_RESET:GPIO_PIN_SET);
}




uint8_t ScanKey(void)
{
	static uint8_t oldkey = 0;
	static uint8_t keycnt = 0;
	uint8_t key = 0;
	
	if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin) == GPIO_PIN_RESET) key= key|KEY1;
	if(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin) == GPIO_PIN_RESET) key= key|KEY2;	
	if(HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin) == GPIO_PIN_RESET) key= key|KEY3;
	if(HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin) == GPIO_PIN_RESET) key= key|KEY4;
	if(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin) == GPIO_PIN_SET) key= key|KEY5;	
	if(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin) == GPIO_PIN_SET) key= key|KEY6;	
	
	if(key!=oldkey){
		keycnt=0;
		oldkey=key;
	}
	else{
		++keycnt;
	}
	
	if(keycnt==2){
		return key;
	}
	
	else{
		return 0;
	}
}




