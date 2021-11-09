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
#include "DS18B20.h"
#include "usart.h"
#include "stdio.h"
#include "adc.h"
#include "util.h"
#include "GUI.h"

//usart
#include "string.h"

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
#define UI_LOGO 0
#define UI_MAIN 1
#define UI_DATA  2
#define UI_TEMP_LINE  3
#define UI_MIC_LINE 4
#define UI_TOUCH_LINE 5

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_24;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_16;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_KaiTi_12;
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_16;
extern GUI_CONST_STORAGE GUI_BITMAP bm2;


uint16_t val[16];
uint16_t adval1;
uint16_t adval2;

float temp;
char temp_data[10] = {0};
char mic_data[10] = {0};

float temp_tab[128];
int temp_idx=0;
uint16_t mic_tab[128];
int mic_idx=0;

uint8_t g_ui_indx = UI_LOGO;
uint8_t g_ui_select = UI_DATA;



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
#define MAX_LINE_SIZE 22
#define MAX_PAGE_LINE 6

uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t recv_buff[MAX_RECV_LEN] = {0};//接收字符串缓存
uint8_t *pBuf;
uint8_t line_flag = 0;
uint32_t recv_tick = 0;

char gui_uart_buff[MAX_LINE_SIZE*8*10] = {0};
char str_show[MAX_RECV_LEN] = {0};
uint8_t gui_uart_index = 0;



uint8_t sense = 0;


//报警延时
int cnt = 0;
int led_flag = 0;

/* USER CODE END Variables */
/* Definitions for TaskMAIN */
osThreadId_t TaskMAINHandle;
const osThreadAttr_t TaskMAIN_attributes = {
  .name = "TaskMAIN",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskLED */
osThreadId_t TaskLEDHandle;
const osThreadAttr_t TaskLED_attributes = {
  .name = "TaskLED",
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
void DrawUILOGO();
void DrawUIMAIN();
void DrawUIDATA();
void DrawUITEMPLINE();
void DrawUIMICLINE();
void DrawUITOUCHLINE();

//key
void Key1Done();
void Key2Done();
void Key3Done();
void Key4Done();

//uart
void data_analyse(char old_str[MAX_RECV_LEN],char str_show[MAX_RECV_LEN],int len);
void StrFormat(char oldstr[MAX_RECV_LEN],char fmtstr[MAX_LINE_SIZE*8*10],int len);


void start_alarm(void);

/* USER CODE END FunctionPrototypes */

void StartTaskMAIN(void *argument);
void StartTaskLED(void *argument);
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
  /* creation of TaskMAIN */
  TaskMAINHandle = osThreadNew(StartTaskMAIN, NULL, &TaskMAIN_attributes);

  /* creation of TaskLED */
  TaskLEDHandle = osThreadNew(StartTaskLED, NULL, &TaskLED_attributes);

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

/* USER CODE BEGIN Header_StartTaskMAIN */
/**
  * @brief  Function implementing the TaskMAIN thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskMAIN */
void StartTaskMAIN(void *argument)
{
  /* USER CODE BEGIN StartTaskMAIN */
	ds18b20_init();
	

  /* Infinite loop */
	uint32_t tick = osKernelGetTickCount()+100;
	
	HAL_ADC_Start_DMA(&hadc1,(uint32_t*)val,16);
  for(;;)
  {
		if(osKernelGetTickCount() >= tick){
			tick = osKernelGetTickCount()+100;
			temp = ds18b20_read();
			printf("temp = %5.1f adcval1=%5d adcval2=%5d\r\n",temp,adval1,adval2);
			
			if(temp_idx<128){
				temp_tab[temp_idx++]=temp;
			}
			else{
				memcpy((void*)temp_tab,(void*)(temp_tab+1),sizeof(float*)*127);
				temp_tab[127]=temp;
			}
			if(mic_idx<128){
				mic_tab[mic_idx++]=temp;
			}
			else{
				memcpy((void*)mic_tab,(void*)(mic_tab+1),sizeof(uint16_t)*127);
				mic_tab[127]=adval2;
			}			
		}
		
    osDelay(1);
  }
  /* USER CODE END StartTaskMAIN */
}

/* USER CODE BEGIN Header_StartTaskLED */
/**
* @brief Function implementing the TaskLED thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLED */
void StartTaskLED(void *argument)
{
  /* USER CODE BEGIN StartTaskLED */
  /* Infinite loop */
  for(;;)
  {
		if(led_flag == 1){
			cnt++;
			if(cnt>=20){
				cnt = 0;
				led_flag = 0;
				Set_leds(0x00);
			}
		
		}
		
    osDelay(500);
  }
  /* USER CODE END StartTaskLED */
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
				g_ui_indx = UI_MAIN;
				break;
			case KEY3:
				Key3Done();
				break;
			case KEY4:
				Key4Done();

				break;
			case KEY5:
				g_ui_indx = UI_LOGO;
				break;
			case KEY6:
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
	
  /* Infinite loop */
  for(;;)
  {
		switch (g_ui_indx)
    {
    	case UI_LOGO:
				DrawUILOGO();
    		break;
    	case UI_MAIN:
				DrawUIMAIN();
    		break;
			case UI_DATA:
				DrawUIDATA();
    		break;
    	case UI_TEMP_LINE:
				DrawUITEMPLINE();
    		break;
    	case UI_MIC_LINE:
				DrawUIMICLINE();
				break;
			case UI_TOUCH_LINE:
				DrawUITOUCHLINE();
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
    osDelay(1);
  }
  /* USER CODE END StartTaskBEEP */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	static uint8_t idx = 0;
	if(hadc == &hadc1)
	{
		int sum1,sum2;
		sum1 = sum2 = 0;
		for(int i = 0;i<16;i+=2){
			sum1 += val[i];
			sum2 += val[i+1];
		}
		
		adval1 = sum1/8;
		adval2 = sum2/8;
		
	}
	
}


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
	
	
		if(g_ui_select<=UI_DATA){
			GUI_SetFont(&GUI_FontHZ_KaiTi_12);
			GUI_DispStringAt("即时数据",20,20);
			GUI_DispStringAt((g_ui_select == UI_DATA)? "*" : " ",10,20);
			GUI_DispStringAt("温度曲线",20,34);	
			GUI_DispStringAt((g_ui_select == UI_TEMP_LINE)? "*" : " ",10,34);	
			GUI_DispStringAt("麦克风曲线",20,48);	
			GUI_DispStringAt((g_ui_select == UI_MIC_LINE)? "*" : " ",10,48);	
		}
		else{
			GUI_SetFont(&GUI_FontHZ_KaiTi_12);
			GUI_DispStringAt("温度曲线",20,20);
			GUI_DispStringAt((g_ui_select == UI_TEMP_LINE)? "*" : " ",10,20);
			GUI_DispStringAt("麦克风曲线",20,34);	
			GUI_DispStringAt((g_ui_select == UI_MIC_LINE)? "*" : " ",10,34);	
			GUI_DispStringAt("灵敏度设置",20,48);	
			GUI_DispStringAt((g_ui_select == UI_TOUCH_LINE)? "*" : " ",10,48);	
		}
		GUI_Update();
		
}

void DrawUIDATA()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("即时数据",64,0);
	
		GUI_SetFont(&GUI_FontHZ_KaiTi_12);
		GUI_DispStringAt("温度",20,20);
		myftoa_FD(temp,temp_data,2);
		GUI_DispStringAt("度",90,20);	
		GUI_DispStringAt(temp_data,50,20);
	
		GUI_DispStringAt("麦克风",20,40);
		myftoa_FD(adval1*3.3/4096,mic_data,2);
		GUI_DispStringAt("v",90,40);	
		GUI_DispStringAt(mic_data,60,40);	
	
		memset(mic_data,0,sizeof(mic_data));	
		memset(mic_data,0,sizeof(mic_data));
		GUI_Update();

}

void DrawUITEMPLINE()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
//		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("温度数据曲线",0,0);

	
		float temp_min=22;
		float temp_max=25;
		float temp_mid = (temp_min+temp_max)/2;
		float temp_height = (temp_max-temp_min)/2;
	
		for(int i=1;i<temp_idx;++i){
			int h1 = 16+24-(temp_tab[i]-temp_mid)*24/temp_height;
			int h0 = 16+24-(temp_tab[i-1]-temp_mid)*24/temp_height;
			GUI_DrawLine(i-1,h0,i,h1);
//			GUI_DrawPixel(i,h1);
		}

	
	
		GUI_Update();
}

void DrawUIMICLINE()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
//		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("麦克风曲线",0,0);
		GUI_Update();
		float mic_min=0;
		float mic_max=4095;
		float mic_mid = (mic_min+mic_max)/2;
		float mic_height = (mic_max-mic_min)/2;
	
		for(int i=1;i<temp_idx;++i){
			int h1 = 16+24-(mic_tab[i]-mic_mid)*24/mic_height;
			int h0 = 16+24-(mic_tab[i-1]-mic_mid)*24/mic_height;
			GUI_DrawLine(i-1,h0,i,h1);
		}
	
	
	
		GUI_Update();
	
}



void DrawUITOUCHLINE()
{
		GUI_Clear();
		GUI_SetFont(&GUI_FontHZ_KaiTi_16);
//		GUI_SetTextAlign(GUI_TA_CENTER);
		GUI_DispStringAt("灵敏度设置",40,0);
	
		
		GUI_DispStringAt("灵敏度等级",40,20);
	
		GUI_DispCharAt(sense+'0',50,40);	
		
		if((4000-sense*160)<=adval2&&sense!=0){
			start_alarm();
		}
	
	
		GUI_Update();
		




}


void beep(uint16_t time,uint8_t tune)
{
	beep_time = time+osKernelGetTickCount();
	beep_tune = tune;
}

//按键响应函数
void Key1Done()
{
	switch (g_ui_indx)
	{
		case UI_MAIN:
			if(g_ui_select>UI_DATA)
				-- g_ui_select;
			break;
			
		case UI_TOUCH_LINE:
			if(sense>0)
				sense-=1;
			break;			
			
		default:
			break;
	}
}

void Key2Done()
{
	switch (g_ui_indx)
	{
		case UI_DATA:
			printf("KEY2\r\n");
			gui_uart_index = (gui_uart_index+1)%10;
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
		
	
		default:
			break;
	}
}

void Key4Done()
{
	switch (g_ui_indx)
	{
		case UI_MAIN:
			if(g_ui_select<UI_TOUCH_LINE)
				++ g_ui_select;		
			break;
		case UI_TOUCH_LINE:
			if(sense<5)
				sense+=1;
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
	
}

//strFormat格式化函数
void StrFormat(char oldstr[MAX_RECV_LEN],char fmtstr[MAX_LINE_SIZE*8*10],int len)
{
	int i,j,row = 0,col = 0;
	memset(fmtstr,0,MAX_LINE_SIZE*8*10);
	
	
	for(i = 0;i<len;++i)
	{
		if(oldstr[i]!='\n' && oldstr[i] != '\r')
		{
			fmtstr[row*MAX_LINE_SIZE+col] = oldstr[i];
			++col;
			
			if(col >= MAX_LINE_SIZE-1)
			{
				row++;
				col = 0;	
			}
		
		}
		else 
		{
			if(i>0&&oldstr[i-1]!='\r'&&oldstr[i-1]!='\n')
				row++;
			col = 0;
		}
		
		if(row >= 10)
		{
			break;
		}
	}
}


void start_alarm(void)
{
		Set_leds(0xff);	
		cnt = 0;
		led_flag = 1;
		
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
