/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "GUI.h"
#include "usart.h"
#include "ds_18b20.h"
#include "adc.h"
#include "w25qxx.h"
#include "fatfs.h"
#include "spi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define UI_LOGO				0
#define UI_MAIN				1
#define UI_DATA				2
#define UI_TEMP_LINE	3
#define UI_MIC_LINE		4
#define UI_TOUCH_LINE	5
#define UI_ALARM_SET	6

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_12;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_16;
extern GUI_CONST_STORAGE GUI_BITMAP bmPKQ;

uint8_t g_ui_index = UI_ALARM_SET;
uint8_t g_ui_select = UI_DATA;

#define MAX_RECV_LEN 	256
#define MAX_LINE_SIZE	22
#define MAX_PAGE_LINE	6
uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t recv_buff[MAX_RECV_LEN] = {0};
uint8_t *pBuf;
uint32_t recv_tick = 0;

#define MAX_DMA_BUFF_SIZE	4
uint16_t dma_adval[MAX_DMA_BUFF_SIZE];
uint16_t adval1;	// 麦克风输入
uint16_t adval2;	// 触摸按键输入

float temp_tab[128];
int temp_idx = 0;
uint16_t mic_tab[128];
int mic_idx = 0;
uint16_t touch_tab[128];
int touch_idx = 0;

float temp_alarm_max = 30;
uint8_t g_balarm = 0;

int temp_alarm_time = 5;

/* USER CODE END Variables */
/* Definitions for TaskMain */
osThreadId_t TaskMainHandle;
const osThreadAttr_t TaskMain_attributes = {
  .name = "TaskMain",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for TaskKEY */
osThreadId_t TaskKEYHandle;
const osThreadAttr_t TaskKEY_attributes = {
  .name = "TaskKEY",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskUsart */
osThreadId_t TaskUsartHandle;
const osThreadAttr_t TaskUsart_attributes = {
  .name = "TaskUsart",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskGUI */
osThreadId_t TaskGUIHandle;
const osThreadAttr_t TaskGUI_attributes = {
  .name = "TaskGUI",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DrawUILogo(void);
void DrawUIMain(void);
void DrawUIData(void);
void DrawUITempLine(void);
void DrawUIMicLine(void);
void DrawUITouchLine(void);
void DrawUIAlarmSet(void);

void Key1Done(void);
void Key2Done(void);
void Key3Done(void);
void Key4Done(void);

#define SET_ADDR 10 * 4096
void SaveAlarmSet(uint32_t addr, float setval);
float LoadAlarmSet(uint32_t addr);

void LoadPara(void);
void SavePara(void);
void PrintfAlarm(void);
void SaveAlarm(float temp);

/* USER CODE END FunctionPrototypes */

void StartTaskMain(void *argument);
void StartTaskKEY(void *argument);
void StartTaskUsart(void *argument);
void StartTaskGUI(void *argument);

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
  /* creation of TaskMain */
  TaskMainHandle = osThreadNew(StartTaskMain, NULL, &TaskMain_attributes);

  /* creation of TaskKEY */
  TaskKEYHandle = osThreadNew(StartTaskKEY, NULL, &TaskKEY_attributes);

  /* creation of TaskUsart */
  TaskUsartHandle = osThreadNew(StartTaskUsart, NULL, &TaskUsart_attributes);

  /* creation of TaskGUI */
  TaskGUIHandle = osThreadNew(StartTaskGUI, NULL, &TaskGUI_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskMain */
/**
  * @brief  Function implementing the TaskMain thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskMain */
void StartTaskMain(void *argument)
{
  /* USER CODE BEGIN StartTaskMain */
	ds18b20_init();
	LoadPara();
	HAL_Delay(3000);
//	W25QXX_Init();
//	printf("W25QXX FLASH ID:%0x\n\r",W25QXX_ReadID());
	float val = LoadAlarmSet(SET_ADDR);
	if(val > -40 && val < 150)
	{
		temp_alarm_max = val;
		printf("temp alarm max:%lf\n\r",temp_alarm_max);
	}
	
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)dma_adval, MAX_DMA_BUFF_SIZE);
	
  /* Infinite loop */
	uint32_t tick = osKernelGetTickCount() + 100;
	uint32_t alarm_tick = 0;
	
	float temp = 0;
	int step = 100;
  for(;;)
  {
		if (osKernelGetTickCount() >= tick)
		{
			tick = osKernelGetTickCount() + step;
			
			if (g_ui_index == UI_MIC_LINE || 
				g_ui_index == UI_TOUCH_LINE)
			{
				step = 0;
				if (mic_idx < 128)
					mic_tab[mic_idx++] = adval1;
				else
				{
					memcpy((void *)mic_tab, (void *)(mic_tab + 1), sizeof(uint16_t) * 127);
					mic_tab[127] = adval1;
				}
				if (touch_idx < 128)
					touch_tab[touch_idx++] = adval2;
				else
				{
					memcpy((void *)touch_tab, (void *)(touch_tab + 1), sizeof(uint16_t) * 127);
					touch_tab[127] = adval2;
				}

//				printf("adval1:%5d, adval2:%5d\n", adval1, adval2);
			}
			else
			{
				step = 100;
				temp = ds18b20_read();
				if (temp > temp_alarm_max)
				{
					g_balarm = 1;
					if(osKernelGetTickCount() >= alarm_tick)
					{
//						printf("%4.1f temp warnning !!!\n\r", temp);
						SaveAlarm(temp);
						alarm_tick = osKernelGetTickCount() + temp_alarm_time * 1000;
					}
				}
				else
				{
					g_balarm = 0;
					alarm_tick = 0;
//					printf("temp:%4.1f, adval1:%5d, adval2:%5d\n", temp, adval1, adval2);
				}
				if (temp_idx < 128)
					temp_tab[temp_idx++] = temp;
				else
				{
					memcpy((void *)temp_tab, (void *)(temp_tab + 1), sizeof(float) * 127);
					temp_tab[127] = temp;
				}
			}			
		}
		
    osDelay(1);
  }
  /* USER CODE END StartTaskMain */
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
	uint8_t key = 0;
	int beep_time = 100;
	int beep_cnt = 3;
	uint32_t beep_tick = 0;
  for(;;)
  {
		key = KeyScan();
		switch (key)
		{
			case KEY1:
				Key1Done();
				break;
			case KEY2:
				Key2Done();
				break;
			case KEY3:
				Key3Done();
				break;
			case KEY4:
				Key4Done();
				break;
			case KEY5:
				g_ui_index = UI_LOGO;
				break;
			case KEY6:
				g_ui_index = UI_MAIN;
				break;
			default:
				break;
		}
    
		if (g_balarm && osKernelGetTickCount() > beep_tick)
		{
			if (beep_cnt)
			{
//				Beep(&beep_time);
				if (0 == beep_time)
				{
					osDelay(100);
					--beep_cnt;
					if (beep_cnt)
						beep_time = 100;
					else
					{
						beep_cnt = 3;
						beep_time = 100;
						beep_tick = osKernelGetTickCount() + temp_alarm_time*1000;
					}
				}
			}
		}
		osDelay(1);
  }
  /* USER CODE END StartTaskKEY */
}

/* USER CODE BEGIN Header_StartTaskUsart */
/**
* @brief Function implementing the TaskUsart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUsart */
void StartTaskUsart(void *argument)
{
  /* USER CODE BEGIN StartTaskUsart */
	pBuf = rx1_buff;
	HAL_UART_Receive_IT(&huart1, pBuf, 1); // 重新开启接收中断
	
  /* Infinite loop */
  for(;;)
  {
		if (recv_tick > 0 && osKernelGetTickCount() >= recv_tick + 20)
		{
			*pBuf = '\0';			// 添加字符串末尾结束符
			memcpy(recv_buff, rx1_buff, pBuf - rx1_buff + 1);
			recv_tick = 0;
			
			pBuf = rx1_buff;	// 重新指向数组开头
			printf("%s", recv_buff);
			
			if (strstr((char *)recv_buff, "TEMP_ALARM_MAX:") == (char *)recv_buff)
			{
				float temp = atof((char *)recv_buff + 15);
				if (temp >= -10 && temp <= 100)
				{
					temp_alarm_max = temp;
					printf("temp alarm max set ok!\n");
					SavePara();
				}
				else
					printf("temp alarm max set error!\n");
			}
			if (strstr((char *)recv_buff, "TEMP_ALARM_TIME:") == (char *)recv_buff)
			{
				int time = atoi((char *) recv_buff + 16);
				if(time > 0 && time <= 60)
				{
					temp_alarm_time = time;
					printf("temp alarm time set ok!\n");
					SavePara();
				}
				else
					printf("temp alarm time set error!\n");
			}
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskUsart */
}

/* USER CODE BEGIN Header_StartTaskGUI */
/**
* @brief Function implementing the TaskGUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskGUI */
void StartTaskGUI(void *argument)
{
  /* USER CODE BEGIN StartTaskGUI */
	GUI_Init();
		
  /* Infinite loop */
  for(;;)
  {
		switch (g_ui_index)
		{
			default:
			case UI_LOGO:
				DrawUILogo();
				break;
			case UI_MAIN:
				DrawUIMain();
				break;
			case UI_DATA:
				DrawUIData();
				break;
			case UI_TEMP_LINE:
				DrawUITempLine();
				break;
			case UI_MIC_LINE:
				DrawUIMicLine();
				break;
			case UI_TOUCH_LINE:
				DrawUITouchLine();
				break;
			case UI_ALARM_SET:
				DrawUIAlarmSet();
				break;
		}
    osDelay(1);
  }
  /* USER CODE END StartTaskGUI */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void DrawUILogo(void)
{
	GUI_Clear();
	GUI_DrawBitmap(&bmPKQ, (128 - bmPKQ.XSize) / 2, 0);
	GUI_Update();
	osDelay(2000);
	
	GUI_Clear();
	
	GUI_SetFont(&GUI_FontHZ_KaiTi_24);
	GUI_DispStringHCenterAt("1801234", 64, 0);
	GUI_DispStringHCenterAt("曾毓", 64, 30);
	GUI_Update();
	osDelay(2000);
	
	g_ui_index = UI_MAIN;
}

void DrawUIMain(void)
{
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ_SimSun_16);
	GUI_DispStringHCenterAt("主菜单", 64, 0);
	
	GUI_SetFont(&GUI_FontHZ_SimSun_12);
	
	if (g_ui_select > UI_TEMP_LINE)
	{
		GUI_DispStringAt("麦克风数据曲线", 20, 20);
		GUI_DispStringAt((g_ui_select == UI_MIC_LINE) ? "*" : "", 10, 20);
		GUI_DispStringAt("触摸按键数据曲线", 20, 34);
		GUI_DispStringAt((g_ui_select == UI_TOUCH_LINE) ? "*" : "", 10, 34);
		GUI_DispStringAt("温度报警设置", 20, 48);
		GUI_DispStringAt((g_ui_select == UI_ALARM_SET) ? "*" : "", 10, 48);
	}
	else
	{
		GUI_DispStringAt("即时数据", 20, 20);
		GUI_DispStringAt((g_ui_select == UI_DATA) ? "*" : "", 10, 20);
		GUI_DispStringAt("温度曲线", 20, 34);
		GUI_DispStringAt((g_ui_select == UI_TEMP_LINE) ? "*" : "", 10, 34);
		GUI_DispStringAt("麦克风数据曲线", 20, 48);
		GUI_DispStringAt((g_ui_select == UI_MIC_LINE) ? "*" : "", 10, 48);
	}
	
	GUI_Update();
}

void DrawUIData(void)
{
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ_SimSun_16);
	GUI_DispStringHCenterAt("即时数据", 64, 0);
	
	
	GUI_Update();
}

void DrawUITempLine(void)
{
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ_SimSun_16);
	GUI_DispStringHCenterAt("温度曲线", 64, 0);
	
	float temp_min = 20;
	float temp_max = 30;
	float temp_mid = (temp_max + temp_min) / 2;
	float temp_height = (temp_max - temp_min) / 2;
	for (int i = 1; i < temp_idx; ++i)
	{
		int h1 = 16 + 24 - (temp_tab[i] - temp_mid) * 24 / temp_height;
		int h0 = 16 + 24 - (temp_tab[i - 1] - temp_mid) * 24 / temp_height;
		GUI_DrawLine(i - 1, h0, i, h1);
	}
	
	GUI_Update();
}

void DrawUIMicLine(void)
{
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ_SimSun_16);
	GUI_DispStringHCenterAt("麦克风数据曲线", 64, 0);
	
	uint16_t mic_min = 0;
	uint16_t mic_max = 4095;
	uint16_t mic_mid = (mic_max + mic_min) / 2;
	uint16_t mic_height = (mic_max - mic_min) / 2;
	for (int i = 1; i < mic_idx; ++i)
	{
		int h1 = 16 + 24 - (mic_tab[i] - mic_mid) * 24 / mic_height;
		int h0 = 16 + 24 - (mic_tab[i - 1] - mic_mid) * 24 / mic_height;
		GUI_DrawLine(i - 1, h0, i, h1);
	}
	GUI_Update();
}

void DrawUITouchLine(void)
{
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ_SimSun_16);
	GUI_DispStringAt("触摸按键数据曲线", 10, 0);

	uint16_t touch_min = 0;
	uint16_t touch_max = 4095;
	uint16_t touch_mid = (touch_max + touch_min) / 2;
	uint16_t touch_height = (touch_max - touch_min) / 2;
	for (int i = 1; i < touch_idx; ++i)
	{
		int h1 = 16 + 24 - (touch_tab[i] - touch_mid) * 24 / touch_height;
		int h0 = 16 + 24 - (touch_tab[i - 1] - touch_mid) * 24 / touch_height;
		GUI_DrawLine(i - 1, h0, i, h1);
	}
	GUI_Update();
}

void DrawUIAlarmSet(void)
{
	GUI_Clear();
	GUI_SetFont(&GUI_FontHZ_SimSun_16);
	GUI_DispStringAt("温度报警设置", 10, 0);
	
	char str[20];
	sprintf(str, "温度上限：%.0f℃", temp_alarm_max);
	GUI_DispStringAt(str, 10, 20);
	
	GUI_DispStringAt("↑K1  ↓K4", 10, 40);
	GUI_Update();
}

void Key1Done(void)
{
	switch (g_ui_index)
	{
		default:
			break;
		case UI_MAIN:
			if (g_ui_select > UI_DATA)
				-- g_ui_select;
			break;
		case UI_ALARM_SET:
			if (temp_alarm_max < 100)
			{
				temp_alarm_max += 1;
				SaveAlarmSet(SET_ADDR,temp_alarm_max);
				SavePara();
			}
			break;
	}
}

void Key2Done(void)
{
	switch (g_ui_index)
	{
		default:
			break;
	}
}

void Key3Done(void)
{
	switch (g_ui_index)
	{
		default:
			break;
		case UI_MAIN:
			g_ui_index = g_ui_select;
			break;
		case UI_ALARM_SET:
			PrintfAlarm();
			break;
	}
}

void Key4Done(void)
{
	switch (g_ui_index)
	{
		default:
			break;
		case UI_MAIN:
			if (g_ui_select < UI_ALARM_SET)
				++ g_ui_select;
			break;
		case UI_ALARM_SET:
			if (temp_alarm_max > -10)
			{
				temp_alarm_max -= 1;
				SaveAlarmSet(SET_ADDR,temp_alarm_max);
				SavePara();
			}
			break;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) 
{
	if (UartHandle->Instance == USART1) 
	{
		++ pBuf; // 已接收一个字节数据，当前存储位置指针后移 
		if(pBuf == rx1_buff + MAX_RECV_LEN) // 如果指针已移出数组边界 
			pBuf = rx1_buff; 									// 重新指向数组开头 
		
		recv_tick = osKernelGetTickCount();
		
		__HAL_UNLOCK(UartHandle); 								// 解锁串口
		HAL_UART_Receive_IT(UartHandle, pBuf, 1); // 重新开启接收中断
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) 
{
//	static uint8_t idx = 0;
	if (hadc == &hadc1)
	{
		int sum1, sum2;
		sum1 = sum2 = 0;
		for (int i = 0; i < MAX_DMA_BUFF_SIZE; i += 2)
		{
			sum1 += dma_adval[i];			// 麦克风
			sum2 += dma_adval[i + 1];	// 触摸按键
		}
		adval1 = (sum1 << 1) / MAX_DMA_BUFF_SIZE;
		adval2 = (sum2 << 1) / MAX_DMA_BUFF_SIZE;
	}
}

void SaveAlarmSet(uint32_t addr, float setval)
{
	W25QXX_Write((uint8_t *)(&setval), addr, sizeof(setval));
}

float LoadAlarmSet(uint32_t addr)
{
	float val = 0;
	W25QXX_Read((uint8_t *)(&val), addr, sizeof(val));
	
	return val;
}

void SavePara(void)
{
	FIL fil;
	FRESULT res;	
	
		res = f_open(&fil,"0:/para.txt",FA_WRITE| FA_OPEN_ALWAYS);
	if(res != FR_OK)
		printf("open file error.\n\r");
	else
	{

		f_lseek(&fil,0);
		f_printf(&fil,"ALARM_MAX:%.1f\n",temp_alarm_max);
		f_printf(&fil,"ALARM_TIME:%d\n",temp_alarm_time);		
		f_close(&fil);
	}

}

void LoadPara(void)
{
	FIL fil;
	FRESULT res;	
	char buf[50];
	
		res = f_open(&fil,"0:/para.txt",FA_READ| FA_OPEN_ALWAYS );
	if(res != FR_OK)
		printf("open file error.\n\r");
	else
	{
		f_lseek(&fil,0);
		while (f_gets(buf,50,&fil))
		{
			if (strstr(buf, "ALARM_MAX:") == buf)
			{
				float temp = atof(buf + 10);
				if (temp >= -10 && temp <= 100)
				{
					temp_alarm_max = temp;
					printf("temp alarm max load ok!\n");
				}
				else
					printf("temp alarm max load error!\n");
			}
			if (strstr(buf, "ALARM_TIME:") == buf)
			{
				int time = atoi( buf + 11);
				if(time > 0 && time <= 60)
				{
					temp_alarm_time = time;
					printf("temp alarm time load ok!\n");
				}
				else
					printf("temp alarm time load error!\n");
			}
		}
		f_close(&fil);
	}

}

void SaveAlarm(float temp)
{
	FIL fil;
	FRESULT res;	
	char buf[50];
	
		res = f_open(&fil,"0:/alarm.txt",FA_WRITE | FA_OPEN_APPEND);
	if(res != FR_OK)
		printf("open file error.\n\r");
	else
	{
		sprintf(buf,"%8d,%5.1f\r\n",osKernelGetTickCount(),temp);
		f_puts(buf,&fil);
		f_close(&fil);
	}

}

void PrintfAlarm(void)
{
	FIL fil;
	FRESULT res;	
	char buf[50];
	
		res = f_open(&fil,"0:/alarm.txt",FA_READ| FA_OPEN_ALWAYS );
	if(res != FR_OK)
		printf("open file error.\n\r");
	else
	{
		f_lseek(&fil,0);
		while (f_gets(buf,50,&fil))
		{
			printf("%s",buf);
			printf("\n\r");
//			puts(buf);
			
		}
		f_close(&fil);
	}

}
/* USER CODE END Application */

