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
uint8_t led_run=1; //运行
uint8_t led_sp=5;	 //速度等级

uint16_t beep_time;
uint8_t  beep_tune;

uint8_t seg[4] = {0};
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
  /* Infinite loop */
	uint8_t led_sta=0x01;
	Set_leds(led_sta);
	
  for(;;)
  {
		if(led_run){
			if(led_sta<0x80){
				led_sta<<=1;
			}
			else{
				led_sta=0x01;
			}
			
			Set_leds(led_sta);	
		}
		
    osDelay(50*(10-led_sp));
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
  /* Infinite loop */
  for(;;)
  {
		if(osKernelGetTickCount()<beep_time){
			HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin);
			for(volatile uint8_t i = 0;i<beep_tune;++i)
				for(volatile uint16_t j = 0;j<10000;++j);
		}

    osDelay(1);
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
	
  for(;;)
  {
		
		key=ScanKey();
		
		switch (key)
		{
			case KEY1:
				seg[0]=1;
				beep(500,1);
				led_run=1;
				break;
			case KEY2:
				seg[0]=2;
				beep(500,2);
				led_run=0;
				break;
			case KEY3:
				seg[0]=3;
				beep(500,3);
				if(led_sp<9)
					++led_sp;
				break;
			case KEY4:
				seg[0]=4;
				beep(500,4);
				if(led_sp>1)
					--led_sp;			
				break;
			case KEY5:
				seg[0]=5;
				beep(500,5);					
				break;
			case KEY6:
				seg[0]=6;
				beep(500,6);					
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
  /* Infinite loop */
  for(;;)
  {
		int i;
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

void beep(uint16_t time,uint8_t tune)
{
	beep_time = time+osKernelGetTickCount();
	beep_tune = tune;
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
