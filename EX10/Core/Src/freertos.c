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
#include "usart.h "
#include "adc.h"
#include "dac.h"
#include "tim.h"
#include "w25qxx.h"

#include "stdio.h "
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
uint8_t recv_ok = 0;

uint8_t led_run = 1;
uint8_t led_sp = 5;
uint8_t seg[4] = {0};

#define MAX_RECV_LEN 128
uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t *pBuf;
uint8_t line_flag = 0;
uint32_t recv_tick = 0;

#define MAX_DMA_BUFF_SIZE		40000
uint16_t dma_buff1[MAX_DMA_BUFF_SIZE];    //5秒40k个采样数据
uint8_t run = 1;

uint16_t count_sec = 0;
uint16_t count_min = 0;
/* USER CODE END Variables */
/* Definitions for Task_LED */
osThreadId_t Task_LEDHandle;
const osThreadAttr_t Task_LED_attributes = {
  .name = "Task_LED",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Usart */
osThreadId_t Task_UsartHandle;
const osThreadAttr_t Task_Usart_attributes = {
  .name = "Task_Usart",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Key */
osThreadId_t Task_KeyHandle;
const osThreadAttr_t Task_Key_attributes = {
  .name = "Task_Key",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Seg */
osThreadId_t Task_SegHandle;
const osThreadAttr_t Task_Seg_attributes = {
  .name = "Task_Seg",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTaskLed(void *argument);
void StartTaskUsart(void *argument);
void StartTaskKey(void *argument);
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
  /* creation of Task_LED */
  Task_LEDHandle = osThreadNew(StartTaskLed, NULL, &Task_LED_attributes);

  /* creation of Task_Usart */
  Task_UsartHandle = osThreadNew(StartTaskUsart, NULL, &Task_Usart_attributes);

  /* creation of Task_Key */
  Task_KeyHandle = osThreadNew(StartTaskKey, NULL, &Task_Key_attributes);

  /* creation of Task_Seg */
  Task_SegHandle = osThreadNew(StartTaskSeg, NULL, &Task_Seg_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskLed */
/**
  * @brief  Function implementing the Task_LED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskLed */
void StartTaskLed(void *argument)
{
  /* USER CODE BEGIN StartTaskLed */
  /* Infinite loop */
	uint8_t led_sta = 0x01;
	SetLeds(led_sta);            //蓝牙调试助手(大作业)
  for(;;)
  {

		if(led_run)
		{
			if(led_sta < 0x80)
				led_sta <<= 1;
			else
				led_sta = 0x01;
			SetLeds(led_sta);
		}
		
		if(recv_ok)
		{
			recv_ok = 0;
			W25QXX_Write((uint8_t *)dma_buff1,0,sizeof(dma_buff1));
		}
		
    osDelay(100 * (10-led_sp));

  }
  /* USER CODE END StartTaskLed */
}

/* USER CODE BEGIN Header_StartTaskUsart */
/**
* @brief Function implementing the Task_Usart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUsart */
void StartTaskUsart(void *argument)
{
  /* USER CODE BEGIN StartTaskUsart */
  /* Infinite loop */
//	printf("Hello world!\n");
	pBuf = rx1_buff;
	HAL_UART_Receive_IT(&huart1, pBuf, 1);
  for(;;)
  {
		if(line_flag)
		{

			char *pb = (char *)rx1_buff;
			if(strstr(pb, "STOP") == pb)
			{
				led_run = 0;
				printf("OK\n\r");
			}
				
			else if(strstr(pb, "START") == pb)
			{
				led_run = 1;
				printf("OK\n\r");
			}
				
			else if(strstr(pb, "SP") == pb)
			{
				if(pb[2] >= '1' && pb[2] <= '9')
				{
					led_sp = pb[2] - '0';
					printf("OK\n\r");
				}
			}
			else 
			{
				int i;
				int bok = 1;
				int dat[4];
				for(i=0;i<strlen(pb) && i < 4;++i)
				{
					if(pb[i] >= '0' && pb[i] <= '9')
					{
						dat[i] = pb[i] - '0';						
					}
					else if(pb[i] >= 'A' && pb[i] <= 'F')
					{
						dat[i] = 10 + pb[i] - 'A';
					}
					else if(pb[i] == '\n' || pb[i] == '\r')
						break;
					else
						bok = 0;
				}
				if(bok)
				{
					int k = 0;
					for (int j = 4-i;j<4;++j)
						seg[j] = dat[k++];
				}
				else if(bok == 0)
			    printf("%s",rx1_buff);				
			}
			
			line_flag = 0;
			
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskUsart */
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
				printf("打印录音数据到串口...\r\n");
				W25QXX_Read((uint8_t *)dma_buff1,0,sizeof(dma_buff1));
				for(uint32_t i=0;i<40000;i++)
				{
					osDelay(1);
					printf("%d ",dma_buff1[i]);
				}
				printf("\r\n\r\n");
				printf("打印结束！\r\n");
				
				osDelay(200);
				break;
			case KEY2:
				run = !run;

				if(run)
					HAL_TIM_Base_Start(&htim2);
				else
					HAL_TIM_Base_Stop(&htim2);
				osDelay(200);

				break;
			case KEY3:
				printf("重新放音...\r\n");
				HAL_TIM_Base_Stop(&htim2);
				HAL_DAC_Stop_DMA(&hdac,DAC_CHANNEL_1);
			
			  W25QXX_Read((uint8_t *)dma_buff1,0,sizeof(dma_buff1));
				HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)dma_buff1,MAX_DMA_BUFF_SIZE,DAC_ALIGN_12B_R);
				HAL_TIM_Base_Start(&htim2);
			
				osDelay(200);
				
				break;
			case KEY4:

				break;
			case KEY5:
				printf("开始录音...\r\n");
				HAL_ADC_Start_DMA(&hadc1,(uint32_t *)dma_buff1,MAX_DMA_BUFF_SIZE);
				HAL_TIM_Base_Start(&htim2);
			
				osDelay(200);

				break;
			case KEY6:
				printf("开始放音...\r\n");
				W25QXX_Read((uint8_t *)dma_buff1,0,sizeof(dma_buff1));
				HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)dma_buff1,MAX_DMA_BUFF_SIZE,DAC_ALIGN_12B_R);
				HAL_TIM_Base_Start(&htim2);
			
				osDelay(200);
				break;
			default:
				break;
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskKey */
}

/* USER CODE BEGIN Header_StartTaskSeg */
/**
* @brief Function implementing the Task_Seg thread.
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
		for(int i=0; i<4; ++i)
		{
			if(i==1)
				Write595(i,seg[i],1);
			else
				Write595(i,seg[i],0);
			osDelay(5);
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskSeg */
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
		
		else if(*(pBuf - 1) == '\n')
		{
			line_flag = 1;
			*pBuf = '\0';
			pBuf = rx1_buff;
		}
		
		__HAL_UNLOCK(UartHandle);
		HAL_UART_Receive_IT(UartHandle, pBuf, 1);
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 
{

	if(hadc == &hadc1)
	{
		HAL_TIM_Base_Stop(&htim2);
		HAL_ADC_Stop_DMA(&hadc1);
		
		printf("录音结束！\r\n");
		recv_ok = 1;
		
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hadc) 
{
	HAL_TIM_Base_Stop(&htim2);
	HAL_DAC_Stop_DMA(hadc,DAC_CHANNEL_1);
		
	printf("放音结束！\r\n");

}
/* USER CODE END Application */

