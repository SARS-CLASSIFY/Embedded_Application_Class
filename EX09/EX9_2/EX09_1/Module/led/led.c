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



uint8_t Key_scan(void)
{
	uint8_t key = 0;
	//sw5 detect
	if(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin)== GPIO_PIN_SET){
		key=5;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin)== GPIO_PIN_SET){
			return  key;
		}
		else{
			return 0;
		}	
	}
	//sw6 detect
	if(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin)== GPIO_PIN_SET){
		key=6;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin)== GPIO_PIN_SET){
			return  key;
		}
		else{
			return 0;
		}	
	}
	//SW2
	if(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin)== GPIO_PIN_RESET){
		key=2;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin)== GPIO_PIN_RESET){
			return  key;
		}
		else{
			return 0;
		}	
	}
	//SW3
	if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin)== GPIO_PIN_RESET){
		key=1;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin)== GPIO_PIN_RESET){
			return  key;
		}
		else{
			return 0;
		}	
	}
	
	return 0;
}



//uint8_t KEY_Scan(uint8_t mode)
//{	 
//	static uint8_t key_up=1;//按键按松开标志
//	if(mode)key_up=1;  //支持连按		  
//	if(key_up&&(KEY5==0||KEY6==0))
//	{
//		delay_ms(10);//去抖动 
//		key_up=0;
//		if(KEY5==0)return 1;
//		else if(KEY6==0)return 2;
//	}else if(KEY5==1&&KEY6==1)key_up=1; 	    
// 	return 0;// 无按键按下
//}


