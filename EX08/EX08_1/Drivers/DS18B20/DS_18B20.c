#include "DS_18B20.h"
#define EnableINT()
#define DisableINT()

#define DS_PRECISION 		0x7f   //�������üĴ��� 1f=9λ; 3f=10λ; 5f=11λ; 7f=12λ;
#define DS_AlarmTH  		0x64
#define DS_AlarmTL  		0x8a
#define DS_CONVERT_TICK 1000

#define ResetDQ() 	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_RESET)
#define SetDQ()  	HAL_GPIO_WritePin(DQ_GPIO_Port, DQ_Pin, GPIO_PIN_SET)
#define GetDQ()  	HAL_GPIO_ReadPin(DQ_GPIO_Port, DQ_Pin) 

void delayus(uint32_t nus)
{
	volatile uint32_t i;
	while (nus-- > 0)
	{
		i = 26;
		while (i-- > 0);
	}
}

unsigned char ResetDS18B20(void)
{
	unsigned char resport;
	SetDQ();
	delayus(50);

	ResetDQ();
	delayus(500);  //500us ����ʱ���ʱ�䷶Χ���Դ�480��960΢�룩
	SetDQ();
	delayus(40);  //40us
	//resport = GetDQ();
	uint16_t cnt = 0;
	while(GetDQ() && cnt < 500)
	{
		++cnt;
		delayus(1);  //40us
	}
	if (cnt >= 500)
		resport = 1;
	else
		resport = 0;
	
	delayus(500);  //500us
	SetDQ();
	return resport;
}

void DS18B20WriteByte(unsigned char Dat)
{
	unsigned char i;
	for(i = 8; i > 0; i--)
	{
		ResetDQ();     		//��15u���������������ϣ�DS18B20��15-60u����
		delayus(5);   //5us
		if(Dat & 0x01)
			SetDQ();
		else
			ResetDQ();
		delayus(65);  //65us
		SetDQ();
		delayus(2);  	//������λ��Ӧ����1us
		Dat >>= 1;
	} 
}


unsigned char DS18B20ReadByte(void)
{
	unsigned char i,Dat;
	SetDQ();
	delayus(5);
	for(i = 8; i > 0; i--)
	{
		Dat >>= 1;
		ResetDQ();     //�Ӷ�ʱ��ʼ�������ź��߱�����15u�ڣ��Ҳ�������������15u�����
		delayus(5);   //5us
		SetDQ();
		delayus(5);   //5us
		if(GetDQ())
			Dat |= 0x80;
		else
			Dat &= 0x7f;  
		delayus(65);   //65us
		SetDQ();
	}
	return Dat;
}

void ReadRom(unsigned char *Read_Addr)
{
	unsigned char i;

	DS18B20WriteByte(ReadROM);
	for(i = 8; i > 0; i--)
	{
		*Read_Addr = DS18B20ReadByte();
		Read_Addr++;
	}
}

void DS18B20Init(unsigned char Precision,unsigned char AlarmTH,unsigned char AlarmTL)
{
	DisableINT();
	ResetDS18B20();
	
	DS18B20WriteByte(SkipROM); 
	DS18B20WriteByte(WriteScratchpad);
	DS18B20WriteByte(AlarmTL);
	DS18B20WriteByte(AlarmTH);
	DS18B20WriteByte(Precision);

	ResetDS18B20();
	DS18B20WriteByte(SkipROM); 
	DS18B20WriteByte(CopyScratchpad);
	EnableINT();

	while(!GetDQ());  //�ȴ�������� ///////////
}

void DS18B20StartConvert(void)
{
	DisableINT();
	ResetDS18B20();
	DS18B20WriteByte(SkipROM); 
	DS18B20WriteByte(StartConvert); 
	EnableINT();
}

void DS18B20_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  GPIO_InitStruct.Pin = DQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DQ_GPIO_Port, &GPIO_InitStruct);
}


void ds18b20_init(void)
{
	DS18B20_Configuration();
	ResetDS18B20();
	DS18B20Init(DS_PRECISION, DS_AlarmTH, DS_AlarmTL);
	DS18B20StartConvert();
}


float ds18b20_read(void)
{
	unsigned char DL, DH;
	unsigned short TemperatureData;
	float Temperature;

	DisableINT();
	DS18B20StartConvert();
	ResetDS18B20();
	DS18B20WriteByte(SkipROM); 
	DS18B20WriteByte(ReadScratchpad);
	DL = DS18B20ReadByte();
	DH = DS18B20ReadByte(); 
	EnableINT();

	TemperatureData = DH;
	TemperatureData <<= 8;
	TemperatureData |= DL;

	Temperature = (float)((float)TemperatureData * 0.0625); //�ֱ���Ϊ0.0625��

	return  Temperature;
}
