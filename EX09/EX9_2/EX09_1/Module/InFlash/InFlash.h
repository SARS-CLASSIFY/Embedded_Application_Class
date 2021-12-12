#ifndef __INFLASH_H
#define __INFLASH_H

#include "stm32f4xx_hal.h"

/**
  * STM32F407VET6		512KB		SECTOR_0到SECTOR7有效
  * STM32F407VGT6		1MB			SECTOR_0到SECTOR11有效
  */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

uint8_t EraseFlash(uint32_t start_Add, uint32_t end_Add);				// FLASH擦除
uint8_t WriteFlash(uint8_t *src, uint8_t *dest, uint32_t Len);	// FLASH写入
uint8_t ReadFlash (uint8_t *src, uint8_t *dest, uint32_t Len);	// FLASH读取

void JumpAPP(uint32_t JumpAddress);															// 跳转执行APP应用程序


#endif /* __INFLASH_H */
