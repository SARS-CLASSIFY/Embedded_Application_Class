/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define K2_Pin GPIO_PIN_2
#define K2_GPIO_Port GPIOE
#define K3_Pin GPIO_PIN_3
#define K3_GPIO_Port GPIOE
#define K5_Pin GPIO_PIN_5
#define K5_GPIO_Port GPIOE
#define K6_Pin GPIO_PIN_6
#define K6_GPIO_Port GPIOE
#define L1_Pin GPIO_PIN_8
#define L1_GPIO_Port GPIOE
#define L2_Pin GPIO_PIN_9
#define L2_GPIO_Port GPIOE
#define L3_Pin GPIO_PIN_10
#define L3_GPIO_Port GPIOE
#define L4_Pin GPIO_PIN_11
#define L4_GPIO_Port GPIOE
#define L5_Pin GPIO_PIN_12
#define L5_GPIO_Port GPIOE
#define L6_Pin GPIO_PIN_13
#define L6_GPIO_Port GPIOE
#define L7_Pin GPIO_PIN_14
#define L7_GPIO_Port GPIOE
#define L8_Pin GPIO_PIN_15
#define L8_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
