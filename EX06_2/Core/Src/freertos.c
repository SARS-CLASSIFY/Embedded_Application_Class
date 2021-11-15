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
#include "led.h"
#include "seg.h"
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
uint8_t auto_lights = 0;
int light = 0;


uint8_t seg[4] = {0};

extern int g_beep_time;
//小星星曲谱 8为空拍
const uint8_t star_tab[47] = {1,1,5,5,6,6,5,8,4,4,3,3,2,2,1,8,5,5,4,4,3,3,2,8,5,5,4,4,3,3,2,8,
1,1,5,5,6,6,5,8,4,4,3,3,2,2,1
};
uint8_t start_play = 0;
uint8_t already_play = 0;
/* USER CODE END Variables */
/* Definitions for TaskLED */
osThreadId_t TaskLEDHandle;
const osThreadAttr_t TaskLED_attributes = {
  .name = "TaskLED",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskBEEP */
osThreadId_t TaskBEEPHandle;
const osThreadAttr_t TaskBEEP_attributes = {
  .name = "TaskBEEP",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskKEY */
osThreadId_t TaskKEYHandle;
const osThreadAttr_t TaskKEY_attributes = {
  .name = "TaskKEY",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskSEG */
osThreadId_t TaskSEGHandle;
const osThreadAttr_t TaskSEG_attributes = {
  .name = "TaskSEG",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void beep(uint16_t time,uint8_t tune);
/* USER CODE END FunctionPrototypes */

void StartTaskLED(void *argument);
void StartTaskBEEP(void *argument);
void StartTaskKEY(void *argument);
void StartTaskSEG(void *argument);

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

  /* creation of TaskBEEP */
  TaskBEEPHandle = osThreadNew(StartTaskBEEP, NULL, &TaskBEEP_attributes);

  /* creation of TaskKEY */
  TaskKEYHandle = osThreadNew(StartTaskKEY, NULL, &TaskKEY_attributes);

  /* creation of TaskSEG */
  TaskSEGHandle = osThreadNew(StartTaskSEG, NULL, &TaskSEG_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskLED */
/**
  * @brief  Function implementing the TaskLED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskLED */
void StartTaskLED(void *argument)
{
  /* USER CODE BEGIN StartTaskLED */
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);

  /* Infinite loop */
  for(;;)
  {
		
		

		if(light>100)
		{
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,200-light);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,200-light);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,200-light);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,200-light);
			
		}
		else
		{
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,light);		
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,light);				
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_3,light);				
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_4,light);				
		}			
		
		
		if(auto_lights == 1)
		{
			++light;
		}
		
		if(light>=200)
		{
			light=0;
		}
	
		
		
	
		osDelay(5);
	}
  /* USER CODE END StartTaskLED */
}

/* USER CODE BEGIN Header_StartTaskBEEP */
/**
* @brief Function implementing the TaskBEEP thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBEEP */
void StartTaskBEEP(void *argument)
{
  /* USER CODE BEGIN StartTaskBEEP */
	volatile int i,j=0;
	HAL_TIM_Base_Start_IT(&htim4);
	__HAL_TIM_SetAutoreload(&htim4,100-1);
  /* Infinite loop */
  for(;;)
  {
		if(start_play == 1)
		{
				beep(100,star_tab[j]);
				j++;
				if(j>=47){
					j = 0;
					start_play = 0;//stop
				}
		}

    osDelay(600);
  }
  /* USER CODE END StartTaskBEEP */
}

/* USER CODE BEGIN Header_StartTaskKEY */
/**
* @brief Function implementing the TaskKEY thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskKEY */
void StartTaskKEY(void *argument)
{
  /* USER CODE BEGIN StartTaskKEY */
  /* Infinite loop */
	uint8_t key;
	uint8_t time_cnt = 0;
	
  for(;;)
  {
//		time_cnt++;				//开始计时一秒后程序播放音乐
//		if(time_cnt == 20&&already_play == 0)
//		{
//			time_cnt = 0;
//			already_play = 1;
//			start_play = 1;
//		}
		
		
		
		
		key=ScanKey();
		switch (key)
		{
			case KEY1:
				beep(100,1);
				break;
			case KEY2:
				beep(100,2);
				break;
			case KEY3:
				beep(100,3);
				break;
			case KEY4:
				beep(100,4);	
				break;
			case KEY5:
//				beep(100,5);		
				auto_lights = 1-auto_lights;
				light = 0;
				break;
			case KEY6:
				if(!auto_lights)
				{
					light = (light+10)%200;
					
				}
//				beep(100,6);				
				break;				
			default:
				break;
		}	
    osDelay(50);
  }
  /* USER CODE END StartTaskKEY */
}

/* USER CODE BEGIN Header_StartTaskSEG */
/**
* @brief Function implementing the TaskSEG thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskSEG */
void StartTaskSEG(void *argument)
{
  /* USER CODE BEGIN StartTaskSEG */
	int i;
	int show_num;
  /* Infinite loop */
  for(;;)
  {

		if(light<100)
			show_num=light;
		else
			show_num=200-light;
		
		seg[1] = show_num/100;
		seg[2] = (show_num%100)/10;
		seg[3] = show_num%10;
		
		
		for(i=0;i<4;i++){
			Write595(i,seg[i],0);
			osDelay(5);
		
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskSEG */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/*------------------------------------------------------------------------------
* brief   : 蜂鸣器音调变换函数
-------------------------------------------------------------------------------*/
void beep(uint16_t time,uint8_t tune)
{
	
	static const float TAB[7] = {
		261.62f,293.67f,329.63f,349.23f,391.99f,440.00f,493.88f
	};
	
	if(tune>=0 && tune<7)
	{
		g_beep_time = time;
		int arr = 1000000/TAB[tune-1];
		__HAL_TIM_SET_AUTORELOAD(&htim4,arr);	
	}

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
