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
uint8_t g_key_sta = 0;

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
  HAL_GPIO_WritePin(GPIOB, BEEP_Pin|OLED_SCL_Pin|OLED_SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET);

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
                           PEPin PEPin PEPin PEPin
                           PEPin */
  GPIO_InitStruct.Pin = L1_Pin|L2_Pin|L3_Pin|L4_Pin
                          |L5_Pin|L6_Pin|L7_Pin|L8_Pin
                          |DQ_Pin;
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

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = BEEP_Pin|OLED_SCL_Pin|OLED_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
uint8_t KeyScan(void)
{
	static uint8_t oldkey = 0x00;
	static uint8_t keycnt = 0;
	uint8_t key = 0;
	key |= (HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin) == GPIO_PIN_RESET) ? KEY1 : 0;
	key |= (HAL_GPIO_ReadPin(K2_GPIO_Port, K2_Pin) == GPIO_PIN_RESET) ? KEY2 : 0;
	key |= (HAL_GPIO_ReadPin(K3_GPIO_Port, K3_Pin) == GPIO_PIN_RESET) ? KEY3 : 0;
	key |= (HAL_GPIO_ReadPin(K4_GPIO_Port, K4_Pin) == GPIO_PIN_RESET) ? KEY4 : 0;
	key |= (HAL_GPIO_ReadPin(K5_GPIO_Port, K5_Pin) == GPIO_PIN_SET) ? KEY5 : 0;
	key |= (HAL_GPIO_ReadPin(K6_GPIO_Port, K6_Pin) == GPIO_PIN_SET) ? KEY6 : 0;
	
	if (key == oldkey)
		++keycnt;
	else
		keycnt = 0;
	oldkey = key;
	
	if (1 == keycnt)
	{
		g_key_sta = key;
		return key;
	}
	return 0;
}

void SetLeds(uint8_t sta)
{
	HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, (sta & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, (sta & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L3_GPIO_Port, L3_Pin, (sta & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L4_GPIO_Port, L4_Pin, (sta & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L5_GPIO_Port, L5_Pin, (sta & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L6_GPIO_Port, L6_Pin, (sta & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L7_GPIO_Port, L7_Pin, (sta & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L8_GPIO_Port, L8_Pin, (sta & 0x80) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

void Beep(int *time)
{
	if (*time > 0)
	{
		HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
		*time = *time - 1;
	}
}

/* USER CODE END 2 */
