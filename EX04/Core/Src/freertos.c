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
#include "GUI.h"
#include "led.h"
//usart
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
#define UI_LOGO 0
#define UI_MAIN 1
#define UI_LSD  2
#define UI_KEY  3
#define UI_USART 4
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_16;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_12;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_16;
extern GUI_CONST_STORAGE GUI_BITMAP bm2;

uint8_t g_ui_indx = UI_LOGO;
uint8_t g_ui_select = UI_LSD;

static uint8_t flag = 0;
static uint8_t logo_flag = 0;
uint16_t beep_time;
uint8_t  beep_tune;

//led
uint8_t led_run=1; //运行
uint8_t led_sta=0x01;
//key
extern uint8_t key_sta;


//uart
#define MAX_RECV_LEN 256
uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t *pBuf;
uint8_t line_flag = 0;
uint32_t recv_tick = 0;

char gui_uart_buff[21*8*10] = {0};
char str_show[MAX_RECV_LEN] = {0};
uint8_t gui_uart_index = 0;
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
/* Definitions for TaskGUI */
osThreadId_t TaskGUIHandle;
const osThreadAttr_t TaskGUI_attributes = {
  .name = "TaskGUI",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for TaskBEEP */
osThreadId_t TaskBEEPHandle;
const osThreadAttr_t TaskBEEP_attributes = {
  .name = "TaskBEEP",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void bmp_show(void);
void DrawUILOGO();
void DrawUIMAIN();
void DrawUIKEY();
void DrawUIUSART();
void DrawUILSD();
void beep(uint16_t time,uint8_t tune);

//key
void Key1Done();
void Key2Done();
void Key3Done();
void Key4Done();

//uart
void data_analyse(char old_str[MAX_RECV_LEN],char str_show[MAX_RECV_LEN],int len);
/* USER CODE END FunctionPrototypes */

void StartTaskLED(void *argument);
void StartTaskUART(void *argument);
void StartTaskKEY(void *argument);
void StartTaskGUI(void *argument);
void StartTaskBEEP(void *argument);

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

  /* creation of TaskGUI */
  TaskGUIHandle = osThreadNew(StartTaskGUI, NULL, &TaskGUI_attributes);

  /* creation of TaskBEEP */
  TaskBEEPHandle = osThreadNew(StartTaskBEEP, NULL, &TaskBEEP_attributes);

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

	Set_leds(led_sta);
  /* Infinite loop */
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
		
    osDelay(50);
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
  /* Infinite loop */
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
			memset(str_show,0,sizeof(MAX_RECV_LEN));
			*pBuf = '\0';
			/*-----------------接收逻辑函数调用处---------------------*/
			printf("%s\r\n",rx1_buff);
			data_analyse((char*)rx1_buff,str_show,pBuf-rx1_buff+1);
			
			
			/*--------------------------------------------------------*/
			
			
			pBuf = rx1_buff;
			HAL_UART_Receive_IT(&huart1,pBuf,1);
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
	uint8_t key;
  /* Infinite loop */
  for(;;)
  {		
		key=ScanKey();
		
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
				if(g_ui_indx == UI_USART){
					printf("KEY5\r\n");
				}
				g_ui_indx = UI_LOGO;
				break;
			case KEY6:
				if(g_ui_indx == UI_USART){
					printf("KEY6\r\n");
				}
				g_ui_indx = UI_MAIN;
				break;				
			default:
				break;
		}	
    osDelay(50);
  }
  /* USER CODE END StartTaskKEY */
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
	
	//question 1
//	bmp_show();
//	GUI_Update();
  /* Infinite loop */
  for(;;)
  {
		//question 2
		switch (g_ui_indx)
    {
    	case UI_LOGO:
				DrawUILOGO();
    		break;
    	case UI_MAIN:
				DrawUIMAIN();
    		break;
			case UI_LSD:
				DrawUILSD();
    		break;
    	case UI_KEY:
				DrawUIKEY();
    		break;
    	case UI_USART:
				DrawUIUSART();
    		break;			
    	default:
    		break;
    }

    osDelay(1);
  }
  /* USER CODE END StartTaskGUI */
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

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void bmp_show(void)
{
	if(!flag){
		GUI_DrawBitmap(&bm2,(128-bm2.XSize)/2,0);
		GUI_Update();
		osDelay(3000);
		GUI_Clear();
		
		
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_DispStringAt("嵌入式系统实验\n 19042223\n 孙若松 ",0,0);
		
		GUI_Update();
		flag = 1;
	}


}

void DrawUILOGO()
{
	
		if(logo_flag == 0){
			GUI_Clear();
			GUI_DrawBitmap(&bm2,(128-bm2.XSize)/2,0);
			GUI_Update();
			osDelay(2000);
			GUI_Clear();
			logo_flag = 1;
		}
		
		
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_DispStringAt("19042223\n 孙若松",0,0);
		GUI_Update();
		osDelay(2000);
	
		
		g_ui_indx = UI_MAIN;
	
	
	
	
}

void DrawUIMAIN()
{
	
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("主菜单",64,0);
	
		GUI_SetFont(&GUI_FontHZ_KaiTi_12);
		GUI_DispStringAt("流水灯",64-18,20);
		GUI_DispStringAt((g_ui_select == UI_LSD)? "*" : " ",10,20);
		GUI_DispStringAt("按键检测",64-18,34);	
		GUI_DispStringAt((g_ui_select == UI_KEY)? "*" : " ",10,34);	
		GUI_DispStringAt("串口通信",64-18,48);	
		GUI_DispStringAt((g_ui_select == UI_USART)? "*" : " ",10,48);	
		GUI_Update();
		
}

void DrawUIKEY()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("按键检测",64,0);
	
		GUI_SetFont(&GUI_FontHZ_SimSun_16);
		GUI_DispStringAt((key_sta & KEY1)? "↓":"↑",2,20);
		GUI_DispStringAt((key_sta & KEY2)? "↓":"↑",20,20);
		GUI_DispStringAt((key_sta & KEY3)? "↓":"↑",38,20);
		GUI_DispStringAt((key_sta & KEY4)? "↓":"↑",56,20);	
		GUI_DispStringAt((key_sta & KEY5)? "↓":"↑",74,20);
		GUI_DispStringAt((key_sta & KEY6)? "↓":"↑",92,20);

	
		GUI_Update();

}

void DrawUIUSART()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("串口通信",64,0);
	
		//display
//		printf("%s\r\n",str_show);
		GUI_SetFont(&GUI_FontHZ_KaiTi_12);
		GUI_DispStringAt(str_show,10,20);
	
		GUI_Update();
}

void DrawUILSD()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("流水灯",64,0);
		GUI_Update();
		
		//显示状态
		GUI_SetFont(&GUI_FontHZ_KaiTi_12);
		for(int i=0;i<8;++i)
		{
			GUI_DispStringAt((led_sta&(0x01<<i)) ? "*":" ",10+i*10,20);
		}
		GUI_Update();
	
}


void beep(uint16_t time,uint8_t tune)
{
	beep_time = time+osKernelGetTickCount();
	beep_tune = tune;
}


void Key1Done()
{
	switch (g_ui_indx)
	{
		case UI_MAIN:
			if(g_ui_select>UI_LSD)
				-- g_ui_select;
			break;
		case UI_USART:
			printf("KEY1\r\n");
			break;
		default:
			break;
	}
}

void Key2Done()
{
	switch (g_ui_indx)
	{
		case UI_USART:
			printf("KEY2\r\n");
			break;
		default:
			break;
	}
}

void Key3Done()
{
	switch (g_ui_indx)
	{
		case UI_MAIN:
			g_ui_indx = g_ui_select;
			break;
		case UI_USART:
			printf("KEY3\r\n");
			break;
		default:
			break;
	}
}

void Key4Done()
{
	switch (g_ui_indx)
	{
		case UI_MAIN:
			if(g_ui_select<UI_USART)
				++ g_ui_select;
		case UI_USART:
			printf("KEY4\r\n");
			break;			
			break;
		default:
			break;
	}
}
//uart接收部分

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART1)
	{
		++pBuf;
		if(pBuf == rx1_buff + MAX_RECV_LEN)
			pBuf = rx1_buff;
		
		recv_tick = osKernelGetTickCount();

		
		__HAL_UNLOCK(UartHandle);
		HAL_UART_Receive_IT(UartHandle,pBuf,1);
	}
	
	

}

//串口接收处理逻辑,用于显示较短字符串
void data_analyse(char old_str[MAX_RECV_LEN],char str_show[MAX_RECV_LEN],int len)
{
	int row = 0,num = 0;//每行12字符
	for(int i=0;i<len;i++)
	{
		str_show[i] = old_str[i+row];
		num ++;
		if(num == 12){
			num = 0;
			row++;
			str_show[i+1] = '\n';
		}
	}
	
//	//display
//	GUI_SetFont(&GUI_FontHZ_KaiTi_12);
//	GUI_DispStringAt(str_show,10,20);
//	memset(str_show,0,MAX_RECV_LEN);
	
	
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
