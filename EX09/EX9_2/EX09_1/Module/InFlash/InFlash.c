#include "InFlash.h"
#include <stdio.h>

/**
  * STM32F407VET6		512KB		SECTOR_0到SECTOR7有效
  * STM32F407VGT6		1MB			SECTOR_0到SECTOR11有效
  * 返回给定地址所在扇区
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    sector = FLASH_SECTOR_0;  
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    sector = FLASH_SECTOR_1;  
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    sector = FLASH_SECTOR_2;  
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    sector = FLASH_SECTOR_3;  
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    sector = FLASH_SECTOR_4;  
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    sector = FLASH_SECTOR_5;  
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    sector = FLASH_SECTOR_6;  
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    sector = FLASH_SECTOR_7;  
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    sector = FLASH_SECTOR_8;  
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    sector = FLASH_SECTOR_9;  
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    sector = FLASH_SECTOR_10;  
  else
    sector = FLASH_SECTOR_11;  
 
  return sector;
}

uint8_t EraseFlash(uint32_t start_Add, uint32_t end_Add)
{
    uint32_t UserStartSector;
    uint32_t SectorError;
    FLASH_EraseInitTypeDef pEraseInit;
 
    // 解锁内部FLASH
    HAL_FLASH_Unlock(); 
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
 
    // 获取起始地址扇区号
    UserStartSector = GetSector(start_Add);
    pEraseInit.TypeErase = TYPEERASE_SECTORS;
    pEraseInit.Sector = UserStartSector;
    pEraseInit.NbSectors = GetSector(end_Add) - UserStartSector + 1 ;
    pEraseInit.VoltageRange = VOLTAGE_RANGE_3;
 
    if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK)
        return (HAL_ERROR);

    return (HAL_OK);
}

uint8_t WriteFlash(uint8_t *src, uint8_t *dest, uint32_t Len)
{
		uint32_t i = 0;

		if (EraseFlash((uint32_t)dest, (uint32_t)(dest + Len)) == HAL_OK)
		{
			for(i = 0; i < Len; i+=4)
			{
					// 电源电压在2.7-3.6V范围时支持FLASH写入操作
					if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)(dest+i), *(uint32_t*)(src+i)) == HAL_OK)
					{
							// 检查写入是否有效
							if(*(uint32_t *)(src + i) != *(uint32_t*)(dest+i))
							{
									// 校验失败，返回错误代码2
									return 2;
							}
					}
					else
					{
							// 写入失败，返回错误代码1
							return HAL_ERROR;
					}
			}
			return HAL_OK;
		}
		else
			return HAL_ERROR;
}

uint8_t ReadFlash (uint8_t *src, uint8_t *dest, uint32_t Len)
{
    uint32_t i = 0;
    uint8_t *psrc = src;
 
    for(i = 0; i < Len; i++)
        dest[i] = *psrc++;
    return HAL_OK;
}

void JumpAPP(uint32_t JumpAddress) {
  typedef void (*Iapfun)(void);
  if(((*(__IO uint32_t*)(JumpAddress + 4)) & 0xFF000000) == 0x08000000) 
  { // 程序地址有效
    if (((*(__IO uint32_t*) JumpAddress) & 0x2FFE0000) == 0x20000000) 
    { // 栈顶地址合法
      // printf("Jump to app...\n"); // 打印信息需要添加串口支持，参见前述章节
      Iapfun App = (Iapfun)*(volatile uint32_t *)(JumpAddress + 4);
      for(int i = 0; i < 8; i++)  {
        NVIC->ICER[i] = 0xFFFFFFFF; // 关闭所有中断
        NVIC->ICPR[i] = 0xFFFFFFFF; // 清除所有中断标志位
      }
      
      // 重设主堆栈栈顶地址
      __set_MSP(*(volatile uint32_t *) JumpAddress);
      __set_PSP(*(volatile uint32_t *) JumpAddress);
      __set_CONTROL(0);		// 任务堆栈改为主堆栈

      App();// 执行App
    }
    else printf("No App...\n");
  }
  else printf("No App...\n");
}
