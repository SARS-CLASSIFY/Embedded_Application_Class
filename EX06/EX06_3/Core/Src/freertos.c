/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t led_run = 1;
uint8_t led_sp = 5;
uint16_t beep_time = 0;
uint8_t beep_tune = 0;
uint8_t seg[4] = {0};
extern int g_beep_time;
extern uint8_t leds_light[8];
uint8_t led_num = 8;
/* USER CODE END Variables */
/* Definitions for TaskLED */
osThreadId_t TaskLEDHandle;
const osThreadAttr_t TaskLED_attributes = {
  .name = "TaskLED",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskBeep */
osThreadId_t TaskBeepHandle;
const osThreadAttr_t TaskBeep_attributes = {
  .name = "TaskBeep",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskKey */
osThreadId_t TaskKeyHandle;
const osThreadAttr_t TaskKey_attributes = {
  .name = "TaskKey",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskMain */
osThreadId_t TaskMainHandle;
const osThreadAttr_t TaskMain_attributes = {
  .name = "TaskMain",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskSeg */
osThreadId_t TaskSegHandle;
const osThreadAttr_t TaskSeg_attributes = {
  .name = "TaskSeg",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void Beep(uint16_t time, uint8_t tune);
/* USER CODE END FunctionPrototypes */

void StartTaskLED(void *argument);
void StartTaskBeep(void *argument);
void StartTaskKey(void *argument);
void StartTaskMain(void *argument);
void StartTaskSeg(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskLED */
  TaskLEDHandle = osThreadNew(StartTaskLED, NULL, &TaskLED_attributes);

  /* creation of TaskBeep */
  TaskBeepHandle = osThreadNew(StartTaskBeep, NULL, &TaskBeep_attributes);

  /* creation of TaskKey */
  TaskKeyHandle = osThreadNew(StartTaskKey, NULL, &TaskKey_attributes);

  /* creation of TaskMain */
  TaskMainHandle = osThreadNew(StartTaskMain, NULL, &TaskMain_attributes);

  /* creation of TaskSeg */
  TaskSegHandle = osThreadNew(StartTaskSeg, NULL, &TaskSeg_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskLED */
/**
  * @brief  Function implementing the LEDTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskLED */
void StartTaskLED(void *argument)
{
  /* USER CODE BEGIN StartTaskLED */
//	uint8_t leds_dir[8] = {0};
  /* Infinite loop */
  for(;;)
  {
		
    osDelay(1);
  }
  /* USER CODE END StartTaskLED */
}

/* USER CODE BEGIN Header_StartTaskBeep */
/**
* @brief Function implementing the Task_Beep thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBeep */
void StartTaskBeep(void *argument)
{
  /* USER CODE BEGIN StartTaskBeep */
	volatile int i, j;
	HAL_TIM_Base_Start_IT(&htim4);
  /* Infinite loop */
  for(;;)
  {
		
    osDelay(1);
  }
  /* USER CODE END StartTaskBeep */
}

/* USER CODE BEGIN Header_StartTaskKey */
/**
* @brief Function implementing the Task_Key thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskKey */
void StartTaskKey(void *argument)
{
  /* USER CODE BEGIN StartTaskKey */
  /* Infinite loop */
	uint8_t key;
  for(;;)
  {
		key = KeyScan();
		switch (key)
		{
			case KEY1:
				break;
			case KEY2:
				break;
			case KEY3:
				break;
			case KEY4:
				break;
			case KEY5:
					led_num=(led_num+1)%9;
				break;
			case KEY6:
					for(uint8_t j=0;j<8;j++)
					{
							leds_light[j] = (leds_light[j]+10)%110;						
					}
				break;
			default:
				break;
		}
    osDelay(200);
  }
  /* USER CODE END StartTaskKey */
}

/* USER CODE BEGIN Header_StartTaskMain */
/**
* @brief Function implementing the TaskMain thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMain */
void StartTaskMain(void *argument)
{
  /* USER CODE BEGIN StartTaskMain */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskMain */
}

/* USER CODE BEGIN Header_StartTaskSeg */
/**
* @brief Function implementing the TaskSeg thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskSeg */
void StartTaskSeg(void *argument)
{
  /* USER CODE BEGIN StartTaskSeg */
  /* Infinite loop */
  for(;;)
  {
			Write595(3,led_num,0);
			osDelay(5);
  }
  /* USER CODE END StartTaskSeg */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Beep(uint16_t time, uint8_t tune)
{
	static const float TAB[7]  = {
		261.62f,293.67f,329.63f,349.23f,391.99f,440.00f,493.88f
	};
	if(tune >0 && tune <= 7)
	{
		g_beep_time = time;

		int arr = 1000000 / (TAB[tune - 1] * 2);
		__HAL_TIM_SET_AUTORELOAD(&htim4, arr);
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
