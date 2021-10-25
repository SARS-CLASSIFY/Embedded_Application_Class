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
#include "usart.h"
#include "stdio.h"
#include "string.h"
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

uint8_t seg[4] = {0};

#define MAX_RECV_LEN 128
uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t *pBuf;
uint8_t line_flag = 0;

uint32_t recv_tick = 0;


/* USER CODE END Variables */
/* Definitions for TaskLED */
osThreadId_t TaskLEDHandle;
const osThreadAttr_t TaskLED_attributes = {
  .name = "TaskLED",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskUART */
osThreadId_t TaskUARTHandle;
const osThreadAttr_t TaskUART_attributes = {
  .name = "TaskUART",
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
void data_analyse();
/* USER CODE END FunctionPrototypes */

void StartTaskLED(void *argument);
void StartTaskUART(void *argument);
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

  /* creation of TaskUART */
  TaskUARTHandle = osThreadNew(StartTaskUART, NULL, &TaskUART_attributes);

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

/* USER CODE BEGIN Header_StartTaskUART */
/**
* @brief Function implementing the TaskUART thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUART */
void StartTaskUART(void *argument)
{
  /* USER CODE BEGIN StartTaskUART */
//	printf("Hello Word!\r\n");
	pBuf = rx1_buff;
	HAL_UART_Receive_IT(&huart1,pBuf,1);
  /* Infinite loop */
  for(;;)
  {
//		if(line_flag)
//		{
//			printf("%s",rx1_buff);
//			memset(rx1_buff,0,MAX_RECV_LEN);
//			line_flag = 0;
//		}
		if(recv_tick > 0 && osKernelGetTickCount() >= recv_tick + 20)
		{
			*pBuf = '\0';
			data_analyse();
			pBuf = rx1_buff;
			recv_tick = 0;
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskUART */
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
				led_run=1;
				break;
			case KEY2:
				seg[0]=2;
				led_run=0;
				break;
			case KEY3:
				seg[0]=3;
				if(led_sp<9)
					++led_sp;
				break;
			case KEY4:
				seg[0]=4;
				if(led_sp>1)
					--led_sp;			
				break;
			case KEY5:
				seg[0]=5;				
				break;
			case KEY6:
				seg[0]=6;				
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART1)
	{
		++ pBuf;
		if(pBuf == rx1_buff + MAX_RECV_LEN)
			pBuf = rx1_buff;
		
		recv_tick = osKernelGetTickCount();
//		else if(*(pBuf - 1) == '\n')
//		{
//			line_flag = 1;
//			*pBuf = '\0';
//			pBuf = rx1_buff;
//		}
		
		__HAL_UNLOCK(UartHandle);
		HAL_UART_Receive_IT(UartHandle,pBuf,1);
	}

}



/*------------------------------------------------------------------------------
* brief   : 串口接收逻辑判断函数
-------------------------------------------------------------------------------*/
void data_analyse()
{
	if(strcmp((char *)rx1_buff,"START") == 0)
	{
			led_run=1;
			printf("OK\r\n");
	}
	else if(strcmp((char *)rx1_buff,"STOP") == 0)
	{
			led_run=0;
			printf("OK\r\n");
	}
	else if(strcmp((char *)rx1_buff,"SP1") == 0)
	{
			led_sp = 1;
			printf("OK\r\n");
	}
	else if(strcmp((char *)rx1_buff,"SP2") == 0)
	{
			led_sp = 2;
			printf("OK\r\n");
	}
		else if(strcmp((char *)rx1_buff,"SP3") == 0)
	{
			led_sp = 3;
			printf("OK\r\n");
	}
	
	else if(strcmp((char *)rx1_buff,"SP4") == 0)
	{
			led_sp = 4;
			printf("OK\r\n");
	}
	else if(strcmp((char *)rx1_buff,"SP5") == 0)
	{
			led_sp = 5;
			printf("OK\r\n");
	}
		else if(strcmp((char *)rx1_buff,"SP6") == 0)
	{
			led_sp = 6;
			printf("OK\r\n");
	}

		else if(strcmp((char *)rx1_buff,"SP7") == 0)
	{
			led_sp = 7;
			printf("OK\r\n");
	}
	else if(strcmp((char *)rx1_buff,"SP8") == 0)
	{
			led_sp = 8;
			printf("OK\r\n");
	}
		else if(strcmp((char *)rx1_buff,"SP9") == 0)
	{
			led_sp = 9;
			printf("OK\r\n");
	}
	else
	{
			printf("%s",rx1_buff);
	}

}




/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
