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
#include "rtc.h"
#include "seg.h"
#include "led.h"
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
uint8_t seg[4] = {0};

uint8_t able_set = 0;
uint8_t bset = 0;//0 disp 1 set min 2 set sec
uint8_t bfree = 0;

uint16_t beep_time = 0;
uint8_t beep_tune;

/* USER CODE END Variables */
/* Definitions for TaskLED */
osThreadId_t TaskLEDHandle;
const osThreadAttr_t TaskLED_attributes = {
  .name = "TaskLED",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskRTC */
osThreadId_t TaskRTCHandle;
const osThreadAttr_t TaskRTC_attributes = {
  .name = "TaskRTC",
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


void EnterSleepMode(void);
void EnterStopMode(void);
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin);
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
void HAL_RTC_AlarmAEventCallback (RTC_HandleTypeDef *hrtc);

/* USER CODE END FunctionPrototypes */

void StartTaskLED(void *argument);
void StartTaskRTC(void *argument);
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

  /* creation of TaskRTC */
  TaskRTCHandle = osThreadNew(StartTaskRTC, NULL, &TaskRTC_attributes);

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
  for(;;)
  {
		if(beep_time>0)
		{
			-- beep_time;
			HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin);
			for(volatile uint8_t i = 0;i<beep_tune;++i)
				for(volatile uint16_t j=0;j<3000;++j);
		}
			
		
    osDelay(1);
  }
  /* USER CODE END StartTaskLED */
}

/* USER CODE BEGIN Header_StartTaskRTC */
/**
* @brief Function implementing the TaskRTC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskRTC */
void StartTaskRTC(void *argument)
{
  /* USER CODE BEGIN StartTaskRTC */
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A);
	
	uint32_t tick = osKernelGetTickCount();
	
  /* Infinite loop */
  for(;;)
  {
		if(osKernelGetTickCount()>tick+1000)
		{
			tick = osKernelGetTickCount();
			if(ReadRTCDateTime() == HAL_OK)
			{
				seg[0] = RTC_Min/10;
				seg[1] = RTC_Min%10;
				seg[2] = RTC_Sec/10;
				seg[3] = RTC_Sec%10;
				
			}
		
		}
		
    osDelay(1);
  }
  /* USER CODE END StartTaskRTC */
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
	uint32_t press_tick;
  /* Infinite loop */
	uint8_t key;
	
  for(;;)
  {
		
		key=ScanKey();
		
		//进入停止模式
		if(key>0)
			press_tick = osKernelGetTickCount();
			
		if(bfree&&osKernelGetTickCount()>=press_tick+10*1000)
		{

			press_tick = osKernelGetTickCount();
			
			HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);
			EnterStopMode();
		}

		
		
		switch (key)
		{
			case KEY1:
					if(bset)
					{
						bset = 1;
						RTC_Min = (RTC_Min+1)%60;
						SetRTCTime(RTC_Hour,RTC_Min,RTC_Sec);
					}
			
				break;
			case KEY2:
					if(bset)
					{
						bset = 2;
						RTC_Sec = (RTC_Sec+59)%60;
						SetRTCTime(RTC_Hour,RTC_Min,RTC_Sec);
					}
				break;
			case KEY3:
					if(bset)
					{
						bset = 2;
						RTC_Sec = (RTC_Sec+1)%60;
						SetRTCTime(RTC_Hour,RTC_Min,RTC_Sec);
					}
				break;
			case KEY4:
					if(bset)
					{
						bset = 1;
						RTC_Min = (RTC_Min+59)%60;
						SetRTCTime(RTC_Hour,RTC_Min,RTC_Sec);
					}		
				break;
			case KEY5:
				bset = !bset;
				break;
			case KEY6:	
				bfree = !bfree;
//				Set_leds(0x01);
//				beep(100,20);
			
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
  /* Infinite loop */
  for(;;)
  {
		switch (bset)
    {
    	case 0:
				for(i=0;i<4;i++)
				{
					if((1 == i&&osKernelGetTickCount()%1000>500)||(i==3&&bfree))
					{
						Write595(i,seg[i],1);
					}
					else
						Write595(i,seg[i],0);
					osDelay(5);
				}
    		break;
    	case 1:
				for(i=0;i<4;i++)
				{
					if((0 == i||i==1)&&osKernelGetTickCount()%1000>500)
					{
						Write595(i,16,0);
					}
					else
						Write595(i,seg[i],0);
					osDelay(5);
				}		
				break;
    	case 2:
				for(i=0;i<4;i++)
				{
					if((2 == i||i==3)&&osKernelGetTickCount()%1000>500)
					{
						Write595(i,16,0);
					}
					else
						Write595(i,seg[i],0);
					osDelay(5);
				}				
			
    		break;
    	default:
    		break;
    }
		
    osDelay(1);
  }
  /* USER CODE END StartTaskSEG */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void beep(uint16_t time, uint8_t tune)
{
	beep_time = time; 
	beep_tune = tune;
}


void EnterSleepMode(void){ //进入睡眠模式
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,30,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
}


void EnterStopMode(void){	//开启停止模式
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,30,RTC_WAKEUPCLOCK_CK_SPRE_16BITS); 	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
}
	
void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin){
	
	
	SystemClock_Config();
	SysTick->CTRL |= ~SysTick_CTRL_ENABLE_Msk;
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc){
	SystemClock_Config();	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	
	HAL_RTCEx_DeactivateWakeUpTimer(hrtc);	
}


void HAL_RTC_AlarmAEventCallback (RTC_HandleTypeDef *hrtc){
		SystemClock_Config();	
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	
		HAL_RTC_DeactivateAlarm(hrtc,RTC_ALARM_A);
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
