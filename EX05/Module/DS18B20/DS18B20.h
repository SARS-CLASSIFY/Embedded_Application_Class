#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "main.h"

void ds18b20_init(void);	// 初始化DS18B20模块
float ds18b20_read(void);	// 开启转换并等待读取模块返回温度数据

#endif