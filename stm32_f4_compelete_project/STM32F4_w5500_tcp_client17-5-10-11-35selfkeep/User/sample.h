#ifndef __SAMPLE_H
#define __SAMPLE_H
#include <stm32f4xx_conf.h>
//#include "modbusH.h"
void NVIC_Configration(void);
void data_transmition(int);//���ݴ��� ����		
void light_Configration(void);//LED��������
void TIM3_Configration(uint16_t);	
void eMBPoll_TCP(unsigned char,unsigned char);
int eMBPoll_USART(unsigned char);
void ADC_Configration(int);
void DMA_Configration(uint16_t);
#endif
