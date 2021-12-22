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
#include "tim.h"
#include "rtc.h"
#include "w25qxx.h"

#include "stdio.h "
#include "string.h"

#include "MPU6050.h"
#include "math.h"
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
uint8_t seg[4] = {0};

#define MAX_RECV_LEN 128
uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t *pBuf;
uint8_t line_flag = 0;
uint32_t recv_tick = 0;

extern uint8_t count_10ms;
extern uint8_t up_flag;

uint8_t start_count_flag = 0;
uint8_t run_flag = 0xFF;
uint8_t show_flag = 0;
uint8_t save_flag = 0;
uint8_t read_flag = 0;
uint8_t read_ad_flag = 0;
uint8_t BT_ok = 0;
uint8_t ad_rec_flag = 0;

short count_sec = 0;
short count_min = 0;
short g_number = 0;
short ad_number = 0;

#define MX_SAVE_DATA	100*10+2 + 500 + 6*5400   //最多采集6分钟，不然超出flash范围
short flash_data[MX_SAVE_DATA] = {0};
short ad_data[6] = {0};
short Amax[720] = {-1};

short max_data_gx=0;
short max_data_gy=0;
short max_data_gz=0;
short max_data[100] = {0};
short Amax_data_x = 0;
short Amax_data_y = 0;
short Amax_data_z = 0;
short time[100] = {0};



uint8_t rx1_buf[128];  // 串口1接收缓冲
uint8_t rx2_buf[512];  // 串口2接收缓冲

/* USER CODE END Variables */
/* Definitions for Task_LED */
osThreadId_t Task_LEDHandle;
const osThreadAttr_t Task_LED_attributes = {
  .name = "Task_LED",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_Usart */
osThreadId_t Task_UsartHandle;
const osThreadAttr_t Task_Usart_attributes = {
  .name = "Task_Usart",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Key */
osThreadId_t Task_KeyHandle;
const osThreadAttr_t Task_Key_attributes = {
  .name = "Task_Key",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Seg */
osThreadId_t Task_SegHandle;
const osThreadAttr_t Task_Seg_attributes = {
  .name = "Task_Seg",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_Main */
osThreadId_t Task_MainHandle;
const osThreadAttr_t Task_Main_attributes = {
  .name = "Task_Main",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_MPU6050 */
osThreadId_t Task_MPU6050Handle;
const osThreadAttr_t Task_MPU6050_attributes = {
  .name = "Task_MPU6050",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for QueueUart2 */
osMessageQueueId_t QueueUart2Handle;
const osMessageQueueAttr_t QueueUart2_attributes = {
  .name = "QueueUart2"
};
/* Definitions for QueueUart1 */
osMessageQueueId_t QueueUart1Handle;
const osMessageQueueAttr_t QueueUart1_attributes = {
  .name = "QueueUart1"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTaskLed(void *argument);
void StartTaskUsart(void *argument);
void StartTaskKey(void *argument);
void StartTaskSeg(void *argument);
void StartTaskMain(void *argument);
void StartTaskMPU6050(void *argument);

void SendATCmd(char *cmd, int waitms) { // 发送AT指令给串口2
	if (NULL != cmd){
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)cmd, strlen(cmd));	
		osDelay(waitms);  // 延时等待HC05模块应答时间
	}
}

void SendBTStr(char *str) { // 发送字符串给串口2
	if (NULL != str)
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)str, strlen(str));
}

// 发送字符串给串口2并显示在串口2
void Trans_Records2BT(short flash_dat[11]) 
{ 
		char num[10] = {0};
		char year[10] = {0};
		char month[10] = {0};
		char dat[10] = {0};
		char hour[10] = {0};
		char min[10] = {0};
		char sec[10] = {0};
		char c_min[10] = {0};
		char c_sec[10] = {0};
		char c_ms[10] = {0};



		
		num[0] = flash_dat[0]/10 + '0';	num[1] = flash_dat[0]%10 + '0';num[2] = '.';	num[3] = ' ';
		year[0] = flash_dat[1]/10 + '0';	year[1] = flash_dat[1]%10 + '0';	year[2] = flash_dat[2]/10 + '0';	year[3] = flash_dat[2]%10 + '0';	year[4] = '-';
		month[0] = flash_dat[3]/10 + '0';	month[1] = flash_dat[3]%10 + '0';	month[2] = '-';
		dat[0] = flash_dat[4]/10 + '0';	dat[1] = flash_dat[4]%10 + '0';	dat[2] = ' ';
		hour[0] = flash_dat[5]/10 + '0';	hour[1] = flash_dat[5]%10 + '0';	hour[2] = ':';
		min[0] = flash_dat[6]/10 + '0';	min[1] = flash_dat[6]%10 + '0';	min[2] = ':';
		sec[0] = flash_dat[7]/10 + '0';	sec[1] = flash_dat[7]%10 + '0';	sec[2] = ' ';	sec[3] = ' ';
		c_min[0] = flash_dat[8]%10 + '0'; c_min[1] = 'm' ; c_min[2] = 'i';  c_min[3] = 'n';
		c_sec[0] = flash_dat[9]/10 + '0'; c_sec[1] = flash_dat[9]%10 + '0'; c_sec[2] = 's'; c_sec[3] = 'e'; c_sec[4] = 'c';
		c_ms[0] = flash_dat[10]/10 + '0'; c_ms[1] = flash_dat[10]%10 + '0';
				
		osDelay(20);	SendBTStr(num);
		osDelay(20);	SendBTStr(year);
		osDelay(20);	SendBTStr(month);	
		osDelay(20);	SendBTStr(dat);	
		osDelay(20);	SendBTStr(hour);	
		osDelay(20);	SendBTStr(min);	
		osDelay(20);	SendBTStr(sec);	
		osDelay(20);	SendBTStr(c_min);	
		osDelay(20);	SendBTStr(c_sec);	
		osDelay(20);	SendBTStr(c_ms);	
		osDelay(20);	SendBTStr("\n");
}

void Trans_adDATA2BT(short flash_dat[6]) 
{ 
	char Ax[10] = {0};
	char Ay[10] = {0};
	char Az[10] = {0};
	char gx[10] = {0};
	char gy[10] = {0};
	char gz[10] = {0};
	
	Ax[0] = (flash_dat[0] > 0) ? '+' : '-';	if(Ax[0] == '+')	{}	else if(Ax[0] == '-')	flash_dat[0] = 0 - flash_dat[0];
	Ax[1] =	(flash_dat[0]/1000  + '0'); Ax[2] =	((flash_dat[0]%1000)/100  + '0'); Ax[3] =	((flash_dat[0]%100)/10  + '0'); Ax[4] =	(flash_dat[0]%10  + '0');
		
	Ay[0] = (flash_dat[1] > 0) ? '+' : '-'; if(Ay[0] == '+')	{}	else if(Ay[0] == '-')	flash_dat[1] = 0 - flash_dat[1];
	Ay[1] =	(flash_dat[1]/1000  + '0'); Ay[2] =	(flash_dat[1]%1000/100  + '0'); Ay[3] =	(flash_dat[1]%100/10  + '0'); Ay[4] =	(flash_dat[1]%10  + '0');
		
	Az[0] = (flash_dat[2] > 0) ? '+' : '-'; if(Az[0] == '+')	{}	else if(Az[0] == '-')	flash_dat[2] = 0 - flash_dat[2];
	Az[1] =	(flash_dat[2]/1000  + '0'); Az[2] =	(flash_dat[2]%1000/100  + '0'); Az[3] =	(flash_dat[2]%100/10  + '0'); Az[4] =	(flash_dat[2]%10  + '0');
		
	gx[0] = (flash_dat[3] > 0) ? '+' : '-';	if(gx[0] == '+')	{}	else if(gx[0] == '-')	flash_dat[3] = 0 - flash_dat[3];
	gx[1] =	(flash_dat[3]/1000  + '0'); gx[2] =	(flash_dat[3]%1000/100  + '0'); gx[3] =	(flash_dat[3]%100/10  + '0'); gx[4] =	(flash_dat[3]%10  + '0');
		
	gy[0] = (flash_dat[4] > 0) ? '+' : '-';	if(gy[0] == '+')	{}	else if(gy[0] == '-')	flash_dat[4] = 0 - flash_dat[4];
	gy[1] =	(flash_dat[4]/1000  + '0'); gy[2] =	(flash_dat[4]%1000/100  + '0'); gy[3] =	(flash_dat[4]%100/10  + '0'); gy[4] =	(flash_dat[4]%10  + '0');
		
	gz[0] = (flash_dat[5] > 0) ? '+' : '-';	if(gz[0] == '+')	{}	else if(gz[0] == '-')	flash_dat[5] = 0 - flash_dat[5];
	gz[1] =	(flash_dat[5]/1000  + '0'); gz[2] =	(flash_dat[5]%1000/100  + '0'); gz[3] =	(flash_dat[5]%100/10  + '0'); gz[4] =	(flash_dat[5]%10  + '0');
	
	osDelay(20);	SendBTStr("wx:");
	osDelay(20);	SendBTStr(Ax);
	osDelay(20);	SendBTStr("wy:");	
	osDelay(20);	SendBTStr(Ay);	
	osDelay(20);	SendBTStr("wz:");	
	osDelay(20);	SendBTStr(Az);
	osDelay(20);	SendBTStr("    d/s");
	osDelay(20);	SendBTStr("\n");	
	osDelay(20);	SendBTStr("ax:");	
	osDelay(20);	SendBTStr(gx);	
	osDelay(20);	SendBTStr("ay");	
	osDelay(20);	SendBTStr(gy);	
	osDelay(20);	SendBTStr("az");	
	osDelay(20);	SendBTStr(gz);	
	osDelay(20);	SendBTStr("    mg");
	osDelay(20);	SendBTStr("\n\n");	
	
}

//返回平均最大加速度
short maxA(void)
{
	short max = 0;
	for(int i = 0; Amax[i] != -1; i++)
		if(Amax[i] > max)
			max = Amax[i];
	return max;
}

//返回单个最大加速度
short maxA_single(short x, short y, short z)
{
	short min0;
	x = x > 0 ? x : 0 - x;
	y = y > 0 ? y : 0 - y;
	z = z > 0 ? z : 0 - z;
	min0 = (x < y ? x : y) < z ? (x < y ? x : y) : z;
	if(x == min0)
		return (short)(pow((double)(y * y + z * z), 0.5));
	else if(y == min0)
		return (short)(pow((double)(x * x + z * z), 0.5));
	else
		return (short)(pow((double)(x * x + y * y), 0.5));
}
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

  /* Create the queue(s) */
  /* creation of QueueUart2 */
  QueueUart2Handle = osMessageQueueNew (8, 512, &QueueUart2_attributes);

  /* creation of QueueUart1 */
  QueueUart1Handle = osMessageQueueNew (8, 512, &QueueUart1_attributes);

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

  /* creation of Task_Main */
  Task_MainHandle = osThreadNew(StartTaskMain, NULL, &Task_Main_attributes);

  /* creation of Task_MPU6050 */
  Task_MPU6050Handle = osThreadNew(StartTaskMPU6050, NULL, &Task_MPU6050_attributes);

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
	 osDelay(4000);
	 printf("进入AT模式！\n");
	 HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_SET);	
	 osDelay(500);
	 printf("测试HC05模块是否存在...\n");
	 SendATCmd("AT\r\n", 500);
	 printf("读取HC05模块名称...\n");
	 SendATCmd("AT+NAME?\r\n", 500);
	 printf("读取HC05模块蓝牙地址...\n");
	 SendATCmd("AT+ADDR?\r\n", 500);
//	printf("设置HC05模块为主机工作模式\n");
//	SendATCmd("AT+ROLE=1\r\n", 500);
	 printf("设置HC05模块为从机工作模式\n");
	 SendATCmd("AT+ROLE=0\r\n", 500);
	 printf("查询HC05模块工作模式...\n");
	 SendATCmd("AT+ROLE?\r\n", 500);
	 printf("设置HC05模块串口参数...\n");
	 SendATCmd("AT+UART=38400,0,0\r\n", 500);
	 printf("查询HC05模块串口参数...\n");
	 SendATCmd("AT+UART?\r\n", 500);
	 printf("退出AT模式！\n");
	 HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, GPIO_PIN_RESET);	
	 printf("重启模块！\n");
	 SendATCmd("AT+RESET\r\n", 500);
	 osDelay(100);	BT_ok = 1;
	 osDelay(100);	SendBTStr("AT SET OK!\n");
	 osDelay(100);	SendBTStr("Please Input:READ RECORD to read records!\n");
	 osDelay(100);	SendBTStr("Please Input:READ AD DATA to read ad data!\n");
	 osDelay(100);	SendBTStr("Please Input:CLEAR ALL DATA to clear all data!\n");
	
	uint8_t led_sta = 0x01;
	SetLeds(led_sta);
  for(;;)
  {
//		SendBTStr("HELLO HC05\r\n");  // 每隔1秒发送一条字符串
//		osDelay(1000);
		if(led_run)
		{
			if(led_sta < 0x80)
				led_sta <<= 1;
			else
				led_sta = 0x01;
			SetLeds(led_sta);
		}
    osDelay(1000);
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
	
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buf, sizeof(rx1_buf));
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2_buf, sizeof(rx2_buf));
	
  uint8_t dat[512];  // 临时数组
//	pBuf = rx1_buff;
	HAL_UART_Receive_IT(&huart1, pBuf, 1);
  for(;;)
  {
   // 串口1接收数据处理
    if (osMessageQueueGet(QueueUart1Handle, dat, NULL, 10) == osOK) {
//      printf("UART1: %s \r\n", dat);  // 串口1打印回显
			char *pb = (char *)dat;
			
			if(strstr(pb, "READ RECORD") == pb)
			{
				read_flag = 1;
			}
			else if(strstr(pb, "READ AD DATA") == pb)
			{
				read_ad_flag = 1;
			}
			
			else if(strstr(pb, "CHANGE DATA") == pb)
			{
				read_ad_flag = 1;
				ad_rec_flag = 1;
			}
			else if(strstr(pb, "READ A MAX") == pb)
			{
				W25QXX_Read((uint8_t *)flash_data,0,sizeof(flash_data));
				osDelay(100);
				Amax_data_x = flash_data[1200];
				Amax_data_y = flash_data[1201];
				Amax_data_z = flash_data[1202];
				printf("a(mg)x:%d y:%d z:%d\r\n",  Amax_data_x, Amax_data_y, Amax_data_z);
				osDelay(100);

			}	
			
			else if(strstr(pb, "READ TIME ALL") == pb)
			{
				W25QXX_Read((uint8_t *)flash_data,0,sizeof(flash_data));
				osDelay(200);
				for(int i=0;i<flash_data[1000];i++){
					time[i] = flash_data[7+10*(i)] * 600 + flash_data[8+10*(i)] * 10 + (5 + flash_data[9+10*(i)])/10;
					osDelay(100);
					printf("%d: time(100ms):%d \r\n", i, time[i]);
					osDelay(100);
				}
			}		
			
			
			else if(strstr(pb, "CLEAR ALL DATA") == pb)
			{
				printf("CLEAR ALL DATA Finshed!\r\n");
				SendBTStr("CLEAR ALL DATA Finshed!\n");
				g_number = 0;
				memset(flash_data,0,sizeof(flash_data));
				osDelay(200);
				W25QXX_Write((uint8_t *)flash_data,0,sizeof(flash_data));
			}		
			
			else 
			{
				printf("Wrong Command!\r\n Please Input: READ AD DATA or READ RECORD\r\n");
			}
			
//      HAL_UART_Transmit_DMA(&huart2, dat, strlen((char *)dat));	// 转发串口2
	   }

    // 串口2接收数据处理
	   if (osMessageQueueGet(QueueUart2Handle, dat, NULL, 10) == osOK)
		 {
//				printf("UART2: %s", dat);	// 串口1打印显示
				 
				char *pb = (char *)dat;
				
				if(strstr(pb, "READ RECORD") == pb)
				{
					read_flag = 1;
				}
				else if(strstr(pb, "READ AD DATA") == pb)
				{
					read_ad_flag = 1;
				}
				
				else if(strstr(pb, "CLEAR ALL DATA") == pb)
				{
					printf("CLEAR ALL DATA Finshed!\r\n");
					SendBTStr("CLEAR ALL DATA Finshed!\n");
					g_number = 0;
					memset(flash_data,0,sizeof(flash_data));
					osDelay(200);
					W25QXX_Write((uint8_t *)flash_data,0,sizeof(flash_data));
				}		
				
				
				else 
				{
					if(BT_ok)
						printf("Wrong Command!\r\n Please Input: READ AD DATA or READ RECORD\r\n");
				}
				

		 }

	   osDelay(1);
		
		
//		if(line_flag)
//		{

//			char *pb = (char *)rx1_buff;
//			if(strstr(pb, "READ RECORD") == pb)
//			{
//				read_flag = 1;
//			}

//			else if(strstr(pb, "READ AD DATA") == pb)
//			{
//				read_ad_flag = 1;
//			}
//			
//			else if(strstr(pb, "CLEAR ALL DATA") == pb)
//			{
//				printf("CLEAR ALL DATA Finshed!\r\n");
//				g_number = 0;
//				memset(flash_data,0,sizeof(flash_data));
//				osDelay(200);
//				W25QXX_Write((uint8_t *)flash_data,0,sizeof(flash_data));
//			}		
//			
//			else 
//			{
//				printf("Wrong Command!\r\n Please Input: READ AD DATA or READ RECORD\r\n");
//			}
//			
//			line_flag = 0;
//			
//		}
//    osDelay(1);
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
	uint8_t key;
	W25QXX_Read((uint8_t *)flash_data,0,sizeof(flash_data));
	g_number = flash_data[1000];
  /* Infinite loop */

  for(;;)
  {
		key = KeyScan();
		switch (key)
		{
			case KEY1:
				read_flag = 1;

				break;
			case KEY2:
				show_flag = 1;
			
				break;
			case KEY3:
				show_flag = 0;

				break;
			case KEY4:
				read_ad_flag = 1;
				break;
			case KEY5:				
				printf("开始计时和采集...\r\n");
				ad_number = 0;
				start_count_flag = 1;
				HAL_TIM_Base_Start_IT(&htim4);
				count_10ms = 0;
				count_sec = 0;
				count_min = 0;
				max_data_gx = 0 ;max_data_gy = 0 ;max_data_gz = 0 ;
				break;
			case KEY6:
//				run_flag = !run_flag;
//				if(!run_flag)
					HAL_TIM_Base_Stop_IT(&htim4);
					start_count_flag = 0;
					save_flag = 1;
					g_number++;
					printf("Please Input:READ RECORD or Press K1 to Read Rcord!\r\n");
					printf("Please Input:READ AD DATA or Press K4 to Read ad data!\r\n");

					 flash_data[1200]= max_data_gx;
					 flash_data[1201]= max_data_gy;
					 flash_data[1202]= max_data_gz;
			
					

			
//				else if(run_flag)
//					HAL_TIM_Base_Start_IT(&htim4);

				break;
			default:
				break;
		}
    osDelay(200);
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
//		W25QXX_Read((uint8_t *)flash_data,0,sizeof(flash_data));
//		osDelay(100);
//		short i = flash_data[1000];
//		count_min = flash_data[7+10*(i)];
//		count_sec = flash_data[8+10*(i)];
//		count_10ms = flash_data[9+10*(i)];
//		osDelay(1000);
  /* Infinite loop */
  for(;;)
  {

		if(!show_flag)
		{
			seg[0] = count_sec / 10;
			seg[1] = count_sec % 10;
			seg[2] = count_10ms / 10;
			seg[3] = count_10ms % 10;			
		}
		
		if(show_flag)
		{
			seg[0] = count_min / 10;
			seg[1] = count_min % 10;
			seg[2] = count_sec / 10;
			seg[3] = count_sec % 10;			
		}


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

/* USER CODE BEGIN Header_StartTaskMain */
/**
* @brief Function implementing the Task_Main thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMain */
void StartTaskMain(void *argument)
{
  /* USER CODE BEGIN StartTaskMain */
//	RTC_Year = 2021;
//	RTC_Mon  = 12;
//	RTC_Dat  = 13;
//	RTC_Hour = 19;
//	RTC_Min  = 17;
//	RTC_Sec  = 50;
//	
//	SetRTCDate(RTC_Year,RTC_Mon,RTC_Dat);
//	SetRTCTime(RTC_Hour,RTC_Min,RTC_Sec);

	short flash_dat[11] = {0};
	short flash_ad_dat[6] = {0};
	int ax0 = 0,ay0 = 0, az0 = 0;
	int ax1,ay1, az1;
	int j = 0;
	short Ax, Ay, Az;
  /* Infinite loop */
  for(;;)
  {
		
		if(start_count_flag)
		{			
			if(up_flag)
			{
				up_flag = 0;
				count_sec++;
				ReadRTCDateTime();
				if(count_sec == 60)	
				{
					count_sec = 0;
					count_min++;
					if(count_min == 60)		count_min = 0;
					else if(count_min == 6) 
					{
							HAL_TIM_Base_Stop_IT(&htim4);
							start_count_flag = 0;
							save_flag = 1;
							g_number++;
							printf("超时！！！\r\n");
							osDelay(1000);
							printf("Please Input:READ RECORD or Press K1 to Read Rcord!\r\n");
							printf("Please Input:READ AD DATA or Press K4 to Read ad data!\r\n");
					}
				}
			}			
		}
		
		if(save_flag)
		{
			save_flag = 0;
			flash_data[1000] = g_number;
			for(int i=0;i<10;i++)
			{
				if(i == 0)	flash_data[i+10*(g_number-1)] = (RTC_Year/100);
				if(i == 1)	flash_data[i+10*(g_number-1)] = (RTC_Year%100);
				if(i == 2)	flash_data[i+10*(g_number-1)] = (RTC_Mon);
				if(i == 3)	flash_data[i+10*(g_number-1)] = (RTC_Dat);
				if(i == 4)	flash_data[i+10*(g_number-1)] = (RTC_Hour);
				if(i == 5)	flash_data[i+10*(g_number-1)] = (RTC_Min);
				if(i == 6)	flash_data[i+10*(g_number-1)] = (RTC_Sec);
				if(i == 7)	flash_data[i+10*(g_number-1)] = (count_min);
				if(i == 8)	flash_data[i+10*(g_number-1)] = (count_sec);
				if(i == 9)	flash_data[i+10*(g_number-1)] = (count_10ms);
			}
			osDelay(200);
			W25QXX_Write((uint8_t *)flash_data,0,sizeof(flash_data));
			osDelay(200);
		}
		
		if(read_flag)
		{
			read_flag = 0;
			printf("reading records...\n\r");
			SendBTStr("reading records...\n");
			osDelay(200);
			W25QXX_Read((uint8_t *)flash_data,0,sizeof(flash_data));
//			flash_data[2] = -10222;
			osDelay(200);
			
			if(flash_data[1000] == 0)
			{
				printf("no records!\r\n");
			}
			else
			{
				for(int i=0;i<flash_data[1000];i++)
				{
					
					printf("%d. %d%d年%d月%d日%d时%d分%d秒 : %d分%d秒%d\r\n",i,flash_data[0+10*(i)],flash_data[1+10*(i)],flash_data[2+10*(i)],flash_data[3+10*(i)],flash_data[4+10*(i)],
																															flash_data[5+10*(i)],flash_data[6+10*(i)],flash_data[7+10*(i)],flash_data[8+10*(i)],flash_data[9+10*(i)]);
//					printf("time(100ms):%d  a(mg)%d", time[i], max_data[i]);
					flash_dat[0] = i;	
					flash_dat[1] = flash_data[0+10*(i)];	flash_dat[2] = flash_data[1+10*(i)];	flash_dat[3] = flash_data[2+10*(i)];	flash_dat[4] = flash_data[3+10*(i)];	flash_dat[5] = flash_data[4+10*(i)];
					flash_dat[6] = flash_data[5+10*(i)];	flash_dat[7] = flash_data[6+10*(i)];	flash_dat[8] = flash_data[7+10*(i)];	flash_dat[9] = flash_data[8+10*(i)];	flash_dat[10] = flash_data[9+10*(i)];
					osDelay(100);
					Trans_Records2BT(flash_dat);

				}
				count_min = flash_data[7+10*(flash_data[1000]-1)];
				count_sec = flash_data[8+10*(flash_data[1000]-1)];
				count_10ms = flash_data[9+10*(flash_data[1000]-1)];
				osDelay(100);
				printf("reading records ends!\r\n");
				osDelay(100);
				SendBTStr("reading records ends!\n");
			}
			
			
		}
		
		if(read_ad_flag)
		{
			read_ad_flag = 0;
			
			uint8_t c_ms = 0;
			uint8_t c_sec = 0;
			uint8_t c_min = 0;
			
			printf("reading ad data...\r\n");
			SendBTStr("reading ad data...\n");
			osDelay(200);
			W25QXX_Read((uint8_t *)flash_data,0,sizeof(flash_data));
			osDelay(200);
			
			if(flash_data[1001] == 0)
			{
				printf("no data!\r\n");
			}
			else
			{
				for(int i=1;i<flash_data[1001];i++)
				{					
					if(c_ms == 20)
					{
						c_ms = 0;
						c_sec++;
					}
					if(c_sec == 60)
					{
						c_sec = 0;
						c_min++;
					}

					ax = flash_data[1002 + 6*(i)]; ay = flash_data[1003 + 6*(i)]; az = flash_data[1004 + 6*(i)]; gx = flash_data[1005 + 6*(i)]; gy = flash_data[1006 + 6*(i)]; gz = flash_data[1007 + 6*(i)];
					ax1 = ax;ay1 = ay;az1 = az;
					Ax = (ax1 - ax0) / 3.7;Ay = (ay1 - ay0) / 3.7;Az = (az1 - az0) / 3.7;
					ax0 = ax1;ay0 = ay1;az0 = az1;
					
					if(ad_rec_flag == 0)
						printf("%d分%d秒%d: axyz%6d,%6d,%6d,gxyz%6d,%6d,%6d\r\n",c_min,c_sec,c_ms*5,Ax,Ay,Az,gx,gy,gz);	

					else if(ad_rec_flag == 1)
						//配合上位机接收格式
						printf("A%dBC%dDE%dFG%dHI%dJK%dL\r\n",Ax,Ay,Az,gx,gy,gz);	
					
					osDelay(50);//延时30ms
					
					flash_ad_dat[0] = Ax; flash_ad_dat[1] = Ay; flash_ad_dat[2] = Az; flash_ad_dat[3] = gx; flash_ad_dat[4] = gy; flash_ad_dat[5] = gz;
					Trans_adDATA2BT(flash_ad_dat);
					
					c_ms++;
				}
				printf("共采集%d次！\r\n",flash_data[1001]);
				printf("reading ad data ends!\r\n");
				osDelay(20);
				SendBTStr("reading ad data ends!\n");
//				printf("read ends\r\n");
			}
			ad_rec_flag = 0;
		}
		
		
    osDelay(1);
  }
  /* USER CODE END StartTaskMain */
}

/* USER CODE BEGIN Header_StartTaskMPU6050 */
/**
* @brief Function implementing the Task_MPU6050 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskMPU6050 */
void StartTaskMPU6050(void *argument)
{
  /* USER CODE BEGIN StartTaskMPU6050 */
  /* Infinite loop */
	
	int cnt=0,mpuok;	

	short Ax, Ay, Az;
	int ax0 = 0,ay0 = 0, az0 = 0;
	int ax1,ay1, az1;
	
	mpuok=MPU_init();
	while(!mpuok && cnt<3)
	{
		osDelay(500);
		mpuok=MPU_init();
		++cnt;
	}	
	if(mpuok)	
	{
		printf("\r\n 六轴传感器初始化成功！ \r\n");
		HAL_Delay(1000);
		printf("Please Input:READ RECORD or Press K1 to Read Rcord!\r\n");
		printf("Please Input:READ AD DATA or Press K4 to Read ad data!\r\n");
		printf("Please Input:CLEAR ALL DATA to Clear all data!\r\n");
	}
	else	
	{
		printf("\r\n 六轴传感器初始化失败！请重新启动程序！！ \r\n");
	}
  for(;;)
  {
		if(mpuok && start_count_flag)
		{
			ad_number++;
			MPU_getdata();//读取传感器数据

			ax1 = ax;ay1 = ay;az1 = az;
			Ax = (ax1 - ax0) / 3.7;Ay = (ay1 - ay0) / 3.7;Az = (az1 - az0) / 3.7;
			ax0 = ax1;ay0 = ay1;az0 = az1;
//			printf("A%dBC%dDE%dFG%dHI%dJK%dL\r\n",Ax,Ay,Az,gx,gy,gz);	
			
			ad_data[0] = ax;	ad_data[1] = ay;	ad_data[2] = az;	ad_data[3] = gx;	ad_data[4] = gy;	ad_data[5] = gz;

			//取最大值
			if(max_data_gx<=gx)
			{
				max_data_gx=gx;
			}
			
			if(max_data_gy<=gy)
			{
				max_data_gy=gy;
			}
			
			
			if(max_data_gz<=gz)
			{
				max_data_gz=gz;
			}
			
			
			osDelay(44);
			flash_data[1001] = ad_number;   //存储共采集多少次
			for(int n=0;n<6;n++)
			{
				flash_data[1002 + n + 6*(ad_number-1)] = ad_data[n];
			}			
		}
			
    osDelay(1);
	
	
//	int cnt=0,mpuok;
//	mpuok=MPU_init();
//	while(!mpuok && cnt<3)
//	{
//		osDelay(500);
//		mpuok=MPU_init();
//		++cnt;
//	}	
//	if(mpuok)	
//	{
//		printf("\r\n 六轴传感器初始化成功！ \r\n");
//		HAL_Delay(1000);
//		printf("Please Input:READ RECORD or Press K1 to Read Rcord!\r\n");
//		printf("Please Input:READ AD DATA or Press K4 to Read ad data!\r\n");
//		printf("Please Input:CLEAR ALL DATA to Clear all data!\r\n");
//	}
//	else	
//	{
//		printf("\r\n 六轴传感器初始化失败！请重新启动程序！！ \r\n");
//	}
//  for(;;)
//  {
//		if(mpuok && start_count_flag)
//		{
//			ad_number++;
//			MPU_getdata();//读取传感器数据
////			printf("A%dBC%dDE%dFG%dHI%dJK%dL\r\n",ax,ay,az,gx,gy,gz);	

//			ad_data[0] = ax;
//			ad_data[1] = ay;
//			ad_data[2] = az;
//			ad_data[3] = gx;
//			ad_data[4] = gy;
//			ad_data[5] = gz;
//				
//			osDelay(46);
//			flash_data[1001] = ad_number;   //存储共采集多少次
//			for(int n=0;n<6;n++)
//			{
//				flash_data[1002 + n + 6*(ad_number-1)] = ad_data[n];
//			}			
//		}
//			
//    osDelay(1);
  }
  /* USER CODE END StartTaskMPU6050 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart) {
	huart->gState = HAL_UART_STATE_READY;  // 复位串口状态
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart == &huart1) { // 串口1接收数据处理
	  HAL_UART_DMAStop(huart);  // 停止DMA请求
	  rx1_buf[Size] = '\0';      // 末尾加字符串结束符
	  osMessageQueuePut(QueueUart1Handle, rx1_buf, NULL, 0);  // 发送消息到队列1
	  // 重启串口1的DMA接收
	  __HAL_UNLOCK(huart);
	  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buf, sizeof(rx1_buf));
	}
	if (huart == &huart2) { // 串口2接收数据处理
	 HAL_UART_DMAStop(huart);  // 停止DMA请求
	 rx2_buf[Size] = '\0';      // 末尾加字符串结束符
	 osMessageQueuePut(QueueUart2Handle, rx2_buf, NULL, 0); // 发送消息到队列2
	 // 重启串口2的DMA接收
	 __HAL_UNLOCK(huart);
	 HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2_buf, sizeof(rx2_buf));
	}
}







//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
//{
//	if(UartHandle->Instance == USART1)
//	{
//		++ pBuf;
//		if(pBuf == rx1_buff + MAX_RECV_LEN)
//			pBuf = rx1_buff;
//		
//		else if(*(pBuf - 1) == '\n')
//		{
//			line_flag = 1;
//			*pBuf = '\0';
//			pBuf = rx1_buff;
//		}
//		
//		__HAL_UNLOCK(UartHandle);
//		HAL_UART_Receive_IT(UartHandle, pBuf, 1);
//	}
//}
/* USER CODE END Application */

