/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "w25qxx.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void TestW25Q128(void)
{
	uint8_t dat[4096] = "1234567890-1234567890-1234567890-1234567890-1234567890-\0";
	uint8_t read_buf[4096] = {0};
	uint16_t i;
	uint16_t *pDat = (uint16_t *)dat;
	for(i=0;i<sizeof(dat)/2;++i)
	{
		pDat[i] = i*i;
	}
	
	W25QXX_Init();
	uint16_t device_id = W25QXX_ReadID();
	printf("device_id = 0x%04X\n\r",device_id);
	
	HAL_Delay(500);
	
	int j=0;
	uint32_t addr = 0;
	for(j=0;j<1;j++)
	{
		addr = 0x1000 * j;
		// ������
		printf("Read data before write\n\r");
		memset(read_buf,0,sizeof(read_buf));
		W25QXX_Read(read_buf,addr,sizeof(read_buf));
		printf("read data if:\n\r");
		for(i=0;i<sizeof(read_buf);++i)
		{
			printf("%02X ",read_buf[i]);
		}
		printf("\n\r");
		HAL_Delay(20);
		
		//����������
		printf("Erase sector %d\n\r",addr / 0x1000);
		W25QXX_Erase_Sector(addr);
		HAL_Delay(20);
		
		//д����
		printf("Write data\n\r");
		W25QXX_Write(dat,addr,sizeof(dat));
		HAL_Delay(20);
		
		//�ٴζ�����
		printf("Read data after write\n\r");
		memset(read_buf,0,sizeof(dat));
		W25QXX_Read(read_buf,addr,sizeof(read_buf));
		printf("read data is:\n\r");
		for(i=0;i<sizeof(read_buf);++i)
		{
			printf("%02X ",read_buf[i]);
		}
		printf("\n\r");
		HAL_Delay(20);
		
		uint8_t chkok = 1;
		for(i=0;i<sizeof(dat);++i)
		{
			if(dat[i] != read_buf[i])
			{
				chkok = 0;
				break;
			}
		}
		printf("sector %d write %s!\n\r", addr/0x1000,chkok?"ok":"error");
		if(!chkok)
			break;
	}
}

void fs_test(void)
{
	BYTE work[4096];
	FATFS fs;
	FIL fil;
	FRESULT res;
	UINT bw,i;
	BYTE mm[500];
	
	res = f_mount(&fs,"0:",1);
	if(res == 0x0D)
	{
		printf("Flash Disk Formatting...\n\r");  //��ʽ��FLASH
		res = f_mkfs("0:",FM_ANY,0,work,sizeof(work));
		if(res != FR_OK)
			printf("mkfs error.\n\r");
	}
	if(res == FR_OK)
		printf("FATFS Init ok!\n\r");
	
	res = f_open(&fil,"0:/test.txt",FA_CREATE_NEW);
	if(res != FR_OK && res != FR_EXIST)
		printf("create file error.\n\r");
	if(res == FR_EXIST)
		res = f_open(&fil,"0:/test.txt",FA_WRITE|FA_READ|FA_OPEN_APPEND);
	if(res != FR_OK)
		printf("open file error.\n\r");
	else
	{
		printf("open file ok.\n\r");
		f_puts("Hello,World\n\r�������\n\r",&fil);
		printf("file size:%ld Bytes.\n\r",f_size(&fil));
		
		memset(mm,0x0,500);
		f_lseek(&fil,0);
		res = f_read(&fil,mm,500,&i);
		if(res == FR_OK)
			printf("read size:%d Bytes.\n\r%s",i,mm);
		else  
			printf("read error!\n\r");
		
		f_close(&fil);
	}
	/*ж���ļ�ϵͳ*/
	f_mount(0,"0:",0);
}

void InitFS(uint8_t breset)
{
	BYTE work[4096];
	FATFS fs;
	FIL fil;
	FRESULT res;

	
	res = f_mount(&fs,"0:",1);
	if(res == 0x0D)
	{
		printf("Flash Disk Formatting...\n\r");  //��ʽ��FLASH
		res = f_mkfs("0:",FM_ANY,0,work,sizeof(work));
		if(res != FR_OK)
			printf("mkfs error.\n\r");
	}
	if(res == FR_OK)
		printf("FATFS Init ok!\n\r");
	if(breset)
		res = f_open(&fil,"0:/alarm.txt",FA_CREATE_ALWAYS);	
	else
		res = f_open(&fil,"0:/alarm.txt",FA_CREATE_NEW);	
	if(res != FR_OK && res != FR_EXIST)
		printf("create file error.\n\r");
	else
	{
		printf("create alarm.txt ok\n\r");
		f_close(&fil);
	}
	
	if(breset)
		res = f_open(&fil,"0:/para.txt",FA_CREATE_ALWAYS);
	else	
		res = f_open(&fil,"0:/para.txt",FA_CREATE_NEW);
	if(res != FR_OK && res != FR_EXIST)
		printf("create file error.\n\r");
	else
	{
		printf("create para.txt ok\n\r");
		f_close(&fil);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A);
	
	
	
	HAL_Delay(1000);
//	TestW25Q128();
//	fs_test();
//	
//	HAL_Delay(10000);
//	InitFS(0);
	
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();
  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
