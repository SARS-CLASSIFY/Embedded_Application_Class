/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CS_Pin|SER_Pin|DISEN_Pin|A1_Pin
                          |A2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, L1_Pin|L2_Pin|L3_Pin|L4_Pin
                          |L5_Pin|L6_Pin|L7_Pin|L8_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DISLK_Pin|SCK_Pin|A3_Pin|A0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = K2_Pin|K3_Pin|K4_Pin|K1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = K5_Pin|K6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin
                           PCPin */
  GPIO_InitStruct.Pin = CS_Pin|SER_Pin|DISEN_Pin|A1_Pin
                          |A2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin
                           PEPin PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = L1_Pin|L2_Pin|L3_Pin|L4_Pin
                          |L5_Pin|L6_Pin|L7_Pin|L8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = DISLK_Pin|SCK_Pin|A3_Pin|A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BEEP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
void SetLeds(uint8_t sta)
{
	HAL_GPIO_WritePin(L1_GPIO_Port,L1_Pin,(sta&0x01)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L2_GPIO_Port,L2_Pin,(sta&0x02)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L3_GPIO_Port,L3_Pin,(sta&0x04)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L4_GPIO_Port,L4_Pin,(sta&0x08)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L5_GPIO_Port,L5_Pin,(sta&0x10)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L6_GPIO_Port,L6_Pin,(sta&0x20)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L7_GPIO_Port,L7_Pin,(sta&0x40)?GPIO_PIN_RESET:GPIO_PIN_SET);
	HAL_GPIO_WritePin(L8_GPIO_Port,L8_Pin,(sta&0x80)?GPIO_PIN_RESET:GPIO_PIN_SET);
}


uint8_t KeyScan(void)
{
	static uint8_t oldkey = 0;
	static uint8_t keycnt = 0;
	uint8_t key = 0;
	
	if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin) == GPIO_PIN_RESET)	key |= KEY1;
	if(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin) == GPIO_PIN_RESET)	key |= KEY2;
	if(HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin) == GPIO_PIN_RESET)	key |= KEY3;
	if(HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin) == GPIO_PIN_RESET)	key |= KEY4;
	if(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin) == GPIO_PIN_SET)	key |= KEY5;
	if(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin) == GPIO_PIN_SET)	key |= KEY6;
	
	if(key != oldkey)	
	{
		keycnt = 0;
		oldkey = key;
	}
	else ++keycnt;
	
	if(keycnt > 2) return key;
	else return oldkey;
}

void Write595(uint8_t sel, uint8_t num, uint8_t bdot)
{
	// 共阴数码管，';0';～';7';，';8';～';F';
	static const uint8_t TAB[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 
																	0x7F,	0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	
	// 74HC138输出高电平，关数码管显示
	HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);
																	
	uint8_t dat = TAB[num & 0x0F] | (bdot ? 0x80 : 0x00);
	uint8_t i;
	for (i = 0; i < 8; ++i)
	{
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, (dat & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		dat <<= 1;
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(DISLK_GPIO_Port, DISLK_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DISLK_GPIO_Port, DISLK_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, (sel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, (sel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);
}

/* USER CODE END 2 */
