/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "InFlash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "w25qxx.h"
#include "fatfs.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
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
#define USER_APP_ADDR		ADDR_FLASH_SECTOR_6
#define MAX_RECV_LEN 10000

uint8_t rx1_buff[MAX_RECV_LEN] = {0};
uint8_t bin_buff[MAX_RECV_LEN] = {0};

uint8_t *pBuf;
uint8_t line_flag = 0;
uint32_t recv_tick = 0;

uint32_t update_tick = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);

FRESULT TestFatFs(FATFS *pfs,FIL *pfil,char *path);//文件操作测试函数
FRESULT GetAppFileFlag(FATFS *pfs,FIL *pfil,char *path);//文件操作测试函数
uint8_t APP_Compare(FATFS *pfs,FIL *pfil,char *path);
	
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_HOST_Init(void);
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
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
  /* USER CODE BEGIN StartDefaultTask */
	uint32_t tick2 = osKernelGetTickCount();
	uint8_t key = 0;
	uint8_t flag = 0;
	uint8_t flag2 = 0;

	
	osDelay(500);
	printf("Start to Search bin file...\r\n");
	retUSBH = GetAppFileFlag(&USBHFatFS,&USBHFile,USBHPath);
	osDelay(200);
//	ReadFlash(rx1_buff,(uint8_t *)ADDR_FLASH_SECTOR_6,50000);
	
	
	osDelay(200);
	
//	printf("Bootloader...PRESS K5 TO UPDATE APP.\n\r");
	

  /* Infinite loop */
  for(;;)
  {
		key = Key_scan();
		
		if(retUSBH == FR_OK && flag2)	
		{
			printf("APP.BIN EXIST...\r\n");   //说明存在BIN
			osDelay(200);
			
			flag = APP_Compare(&USBHFatFS,&USBHFile,USBHPath);
			osDelay(200);
			
			if(flag)
			{
				printf("\r\n");
				printf("NOT SAME!!!\r\n");
				printf("JUNP TO APP B\r\n");
				
				WriteFlash(bin_buff,(uint8_t *)ADDR_FLASH_SECTOR_6,50000);
				JumpAPP(ADDR_FLASH_SECTOR_6);
				
			}
			else
			{
				printf("\r\n");
				printf("SAME!!!\r\n");
				printf("JUNP TO APP A\r\n");
				JumpAPP(ADDR_FLASH_SECTOR_6);
			}
		}
		
		
//		if(key == 2)
//		{
//			printf("JUMP TO APP...\r\n");
//			JumpAPP(ADDR_FLASH_SECTOR_6);
//		}
		
//    osDelay(10000);
		
		//串口函数
//		if(0 == update_tick && HAL_GetTick() >= tick2 + 10*1000)
//		{
//			printf("Jump to app...");
//			JumpAPP(ADDR_FLASH_SECTOR_6);
//		}
//		
		if(update_tick > 0 && HAL_GetTick() >= update_tick + 10 * 1000)
		{
			printf("exit update mode.\r\n");	
			pBuf = rx1_buff;
			
			recv_tick = 0;
			update_tick = 0;
			tick2 = HAL_GetTick();
			
			HAL_UART_AbortReceive_IT(&huart1);		
		}
		
		if(recv_tick > 0 && HAL_GetTick() >= recv_tick + 500)
		{
			if(update_tick > 0)
			{
				printf("receive end.\r\n");
				HAL_UART_AbortReceive_IT(&huart1);
							  
				pBuf = rx1_buff;
//				printf("%s\r\n",rx1_buff);
				
				recv_tick = 0;						
				update_tick = 0;
				tick2 = HAL_GetTick();
				
        flag2 = 1;

//		
				//写入APP
				WriteFlash(rx1_buff,(uint8_t *)ADDR_FLASH_SECTOR_6,10000);
////				//跳转APP
//				JumpAPP(ADDR_FLASH_SECTOR_6);
				}
			}
		
		
		
		
		if(key == 5){
			update_tick = HAL_GetTick();
			recv_tick = 0;
			pBuf = rx1_buff;
			HAL_UART_Receive_IT(&huart1, pBuf, 1);
			printf("Start receive bin file...\r\n");
		}
		
		if(key == 2){
			osDelay(200);
			WriteFlash(rx1_buff,(uint8_t *)ADDR_FLASH_SECTOR_6,10000);
			printf("JUMP TO APPA\r\n");
			JumpAPP(ADDR_FLASH_SECTOR_6);
		}
		
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART1)
	{
		pBuf++;
		if(pBuf == rx1_buff + MAX_RECV_LEN)
			pBuf = rx1_buff;

		recv_tick = HAL_GetTick();		
		if(update_tick > 0)
			update_tick = recv_tick;	

		
		__HAL_UNLOCK(UartHandle);
		HAL_UART_Receive_IT(UartHandle, pBuf, 1);
	}
}

FRESULT TestFatFs(FATFS *pfs,FIL *pfil,char *path)//文件操作测试函数
{
	BYTE work[4096];
	char filename[128];//文件路径名
	FRESULT res;//文件操作结果
	
	res=f_mount(pfs,path,1);//立即挂载文件系统
	if(res == FR_OK)
		printf("FATFS Init ok!\n");
	else{
		printf("FATFS Init error%d\n",res);//初始化失败，直接返回
		return res;
	}
	//打开TEST.TXT文件进行读写
	sprintf(filename,"%sapp.bin",path);//设置文件绝对路径
	res = f_open(pfil,filename,FA_OPEN_ALWAYS|FA_WRITE|FA_READ|FA_OPEN_APPEND);
	if(res!=FR_OK)	printf("open file error.\n");
	else{
		printf("open file ok.\n");
		f_puts("Hello,world!\n你好世界\n",pfil);//写入两行字符串
		
		printf("file size:%d Bytes.\n",(int)f_size(pfil));
		
		UINT br;	//	临时变量
		f_lseek(pfil,0);//定位到文件头，准备读取数据
		memset(work,0x0,sizeof(work));
		res = f_read(pfil,work,sizeof(work),&br);
		if(res==FR_OK)	printf("read size:%d Byte.\n%s",br,work);
		
		else	printf("read error!\r\n");
		f_close(pfil);//使用完毕关文件
	}
	res = f_mount(0,path,0);//测试结束，卸载文件系统
	return res;

}	

FRESULT GetAppFileFlag(FATFS *pfs,FIL *pfil,char *path)//文件操作测试函数
{
//	BYTE work[4096];
//	BYTE work2[4096];
	char filename[128];//文件路径名
	FRESULT res;//文件操作结果
	
	res=f_mount(pfs,path,1);//立即挂载文件系统
	if(res == FR_OK)
		printf("FATFS Init ok!\n");
	else{
		printf("FATFS Init error%d\n",res);//初始化失败，直接返回
		return res;
	}
	//打开TEST.TXT文件进行读写
	sprintf(filename,"%sapp.bin",path);//设置文件绝对路径
	res = f_open(pfil,filename,FA_WRITE|FA_READ);
	if(res!=FR_OK)	
		{
			printf("NO APP.BIN FILE!!!\r\n");
			printf("JUMP TO APP...\r\n");
			osDelay(500);
			JumpAPP(ADDR_FLASH_SECTOR_6);
		}
	else{

		
//		UINT br,br2;	//	临时变量
//		f_lseek(pfil,0);//定位到文件头，准备读取数据
//		memset(work,0x0,sizeof(work));
//		memset(work2,0x0,sizeof(work2));
//		res = f_read(pfil,work,sizeof(work),&br);
//		if(res==FR_OK)	printf("read size:%d Byte.\n%s",br,work);
//		
//		else	printf("read error!\r\n");
//		
//		res = f_read(pfil,work2,sizeof(work2),&br2);
//		if(res==FR_OK)	printf("read size:%d Byte.\n%s",br2,work2);
//		
//		else	printf("read error!\r\n");
		
		f_close(pfil);//使用完毕关文件
	}
	res = f_mount(0,path,0);//测试结束，卸载文件系统
	return res;

}	

uint8_t APP_Compare(FATFS *pfs,FIL *pfil,char *path)//文件操作测试函数
{
//	BYTE work[4096];
//	BYTE work2[4096];
	uint8_t flag = 0;
	char filename[128];//文件路径名
	FRESULT res;//文件操作结果
	
	res=f_mount(pfs,path,1);//立即挂载文件系统
	if(res == FR_OK)
		printf("FATFS Init ok!\n");
	else{
		printf("FATFS Init error%d\n",res);//初始化失败，直接返回
		return res;
	}
	//打开TEST.TXT文件进行读写
	sprintf(filename,"%sapp.bin",path);//设置文件绝对路径
	res = f_open(pfil,filename,FA_WRITE|FA_READ);
	if(res!=FR_OK)	
		{
			printf("open file error!!!\r\n");
		}
	else{
		printf("ENTER UPDATE MODE...\n\r");
		
		printf("APP.BIN file size:%d Bytes.\n\r",(int)f_size(pfil));
		
		osDelay(200);
		
		UINT br,br2;	//	临时变量
		uint16_t count = 10000;
		f_lseek(pfil,0);//定位到文件头，准备读取数据
		
//		memset(work,0x0,sizeof(work));
//		memset(work2,0x0,sizeof(work2));
		
		res = f_read(pfil,bin_buff,sizeof(bin_buff),&br);
		if(res==FR_OK)	printf("read size:%d Byte.\n\r",br);		
		else	printf("read error!\r\n");
		
//		res = f_read(pfil,work,sizeof(work),&br);
//		if(res==FR_OK)	printf("read size:%d Byte.\n%s",br,work);		
//		else	printf("read error!\r\n");
//		
//		res = f_read(pfil,work2,sizeof(work2),&br2);
//		if(res==FR_OK)	printf("read size:%d Byte.\n%s",br2,work2);		
//		else	printf("read error!\r\n");
		
		for(count = 0; count <br; count++)
		{
			if(rx1_buff[count] != bin_buff[count])	
			{
				flag = 1;
				return flag;
			}
			else	flag = 0;
//			if(count<br)
//			{		
//				if(rx1_buff[count] != work[count])	
//				{
//					flag = 1;
//					return flag;
//				}
//				
//			}
//			else	
//			{
//				if(rx1_buff[count] != work[count-br])	
//				{
//					flag = 1;
//					return flag;
//				}
//			}
		}
		
		f_close(pfil);//使用完毕关文件
	}
	res = f_mount(0,path,0);//测试结束，卸载文件系统
	return flag;

}	

/* USER CODE END Application */

