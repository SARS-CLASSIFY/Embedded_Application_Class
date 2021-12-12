#include "InFlash.h"
#include <stdio.h>

/**
  * STM32F407VET6		512KB		SECTOR_0��SECTOR7��Ч
  * STM32F407VGT6		1MB			SECTOR_0��SECTOR11��Ч
  * ���ظ�����ַ��������
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
 
    // �����ڲ�FLASH
    HAL_FLASH_Unlock(); 
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                          FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
 
    // ��ȡ��ʼ��ַ������
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
					// ��Դ��ѹ��2.7-3.6V��Χʱ֧��FLASHд�����
					if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)(dest+i), *(uint32_t*)(src+i)) == HAL_OK)
					{
							// ���д���Ƿ���Ч
							if(*(uint32_t *)(src + i) != *(uint32_t*)(dest+i))
							{
									// У��ʧ�ܣ����ش������2
									return 2;
							}
					}
					else
					{
							// д��ʧ�ܣ����ش������1
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
  { // �����ַ��Ч
    if (((*(__IO uint32_t*) JumpAddress) & 0x2FFE0000) == 0x20000000) 
    { // ջ����ַ�Ϸ�
      // printf("Jump to app...\n"); // ��ӡ��Ϣ��Ҫ��Ӵ���֧�֣��μ�ǰ���½�
      Iapfun App = (Iapfun)*(volatile uint32_t *)(JumpAddress + 4);
      for(int i = 0; i < 8; i++)  {
        NVIC->ICER[i] = 0xFFFFFFFF; // �ر������ж�
        NVIC->ICPR[i] = 0xFFFFFFFF; // ��������жϱ�־λ
      }
      
      // ��������ջջ����ַ
      __set_MSP(*(volatile uint32_t *) JumpAddress);
      __set_PSP(*(volatile uint32_t *) JumpAddress);
      __set_CONTROL(0);		// �����ջ��Ϊ����ջ

      App();// ִ��App
    }
    else printf("No App...\n");
  }
  else printf("No App...\n");
}
