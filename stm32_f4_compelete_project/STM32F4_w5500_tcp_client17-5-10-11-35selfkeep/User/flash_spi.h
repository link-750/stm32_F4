#ifndef __SPI_H
#define __SPI_H
//#include "sys.h"
#include "stm32f4xx_conf.h"
#define MOSI_H GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define MOSI_L GPIO_ResetBits(GPIOC, GPIO_Pin_6)
#define SCLK_H GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define SCLK_L GPIO_ResetBits(GPIOC, GPIO_Pin_7)
#define MISO GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)	   													  
u8 SPI1_ReadWriteByte(u8 TxData);//SPI1总线读写一个字节	 
#endif

