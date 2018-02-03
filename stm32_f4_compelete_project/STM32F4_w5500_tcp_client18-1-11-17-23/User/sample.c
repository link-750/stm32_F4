#include "main.h"
#include "stdio.h"
#include "sample.h"
#include "w25q128.h"
 unsigned char DMA_Symbol;//dma��־λ
 uint16_t ADC_DATA[10000];
/**
  * @brief  LED������
  * @retval None
  */
void light_Configration(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);//ʹ��ʱ��D
  GPIO_InitTypeDef  GPIO_InitStructure;	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;//PD89 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;//��©���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//��©
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����˿��ٶ�
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ�� 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);//ʹ��ʱ��A  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;//PA13-14
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;//����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ�� 
	
  //GPIO_SetBits(GPIOD, GPIO_Pin_8);//
  //GPIO_SetBits(GPIOD, GPIO_Pin_9);//����ɫ��	����ʾ��������
}
/**
  * @brief  TIM3������
  * @retval None
  */
void TIM3_Configration(uint16_t frequency)	
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period=((uint32_t)(1000/(uint32_t)(frequency/1000))-1);
	TIM_TimeBaseStructure.TIM_Prescaler=83;
	TIM_TimeBaseStructure.TIM_ClockDivision=0x0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM3,ENABLE);//����TIM3 preload ARR2015
	TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update);//added,used to trigger ADC.	
	TIM_Cmd(TIM3,ENABLE);
}
void NVIC_Configration(void)
{
	   NVIC_InitTypeDef NVIC_InitStructure;
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
     NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
	   
	   
     //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
     //NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
     //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
     //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
     //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     //NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  DMA������
  * @retval None
  */
void DMA_Configration(uint16_t numtosample)	
{
	 NVIC_Configration();//dma�ж����ȼ�����   
/* DMA2 Stream0 channel2 configuration **************************************/
 RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE);//ʹ��DMAʱ�� 
	DMA_InitTypeDef   DMA_InitStructure;  
	DMA_DeInit(DMA2_Stream0);	
	DMA_Cmd(DMA2_Stream0, DISABLE);                      //����DMA���� 
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}//�ȴ�DMA������		
  DMA_InitStructure.DMA_Channel =DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_DATA[0]; 
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = numtosample;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;//DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	//DMA_SetCurrDataCounter(DMA2_Stream0,1000);          //���ݴ�����  
	   DMA_ClearFlag(DMA2_Stream0,DMA_IT_TC);
	  // DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TC);
     DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);	 	 
     while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){;}	//ȷ��DMA���Ա�����  
	   DMA_Cmd(DMA2_Stream0, ENABLE);                      //����DMA���� 
}
   void DMA2_Stream0_IRQHandler(void) //DMAChannel0_IRQHandler(void)
   {   
   	
       if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0))
       {        
				  DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);  
   			  //printf("AAAAA");	
				  DMA_Symbol=1;//DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0);
				  //printf("%d",DMA_Symbol);	
         	//DMA_Cmd(DMA2_Stream0, DISABLE);//�ر�DMA����
					//DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);//���DMA2_Steam0������ɱ�־					
				  //DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,DISABLE);
				  //DMA_Symbol=0;//dma������ɱ�־��λ 	 
       }
       
   }
/**
  * @brief  adc������
  * @retval None
  */
void ADC_Configration(int ch)
{
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //��ADCʱ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��	
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 	
	
  GPIO_InitTypeDef  GPIO_InitStructure;	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_0;//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  	
	
ADC_DeInit();

ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  //����Ϊ12λ          
ADC_InitStructure.ADC_ScanConvMode = DISABLE;   //ɨ��ת��ģʽʧ��,��ͨ������
ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //����ת��ʧ��
ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; //�����ش���ת��
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//ADC_ExternalTrigConvEdge_None;//DC_ExternalTrigConv_None;//��ʱ��3����
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //�����Ҷ��룬���ֽڶ���
ADC_InitStructure.ADC_NbrOfConversion = 1;    //�涨��˳����й���ת����ADCͨ������Ŀ
ADC_Init(ADC1, &ADC_InitStructure);      

ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;    //����ģʽ
ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6; //��ƵΪ6��f(ADC3)=14M
ADC_CommonInitStructure.ADC_DMAAccessMode =ADC_DMAAccessMode_1; //ADC_DMAAccessMode_Disabled;//ADC_DMAAccessMode_2; //ʹ����DMA_MODE1
//ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���β������5������
ADC_CommonInit(&ADC_CommonInitStructure);
ADC_RegularChannelConfig(ADC1,ch, 1, ADC_SampleTime_112Cycles); //����ͨ�����ã�1��ʾ���������˳��
//ADC_ITConfig(ADC3, ADC_IT_EOC, ENABLE); //ʹ��ADCת�������ж�
//ADC1->SR=0;  // �ز����٣�λ�ñ���������ADCǰ
ADC_DMACmd(ADC1, ENABLE);   //ʹ��ADC1��DMA 
ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE); //��ͨ��ģʽ���ϴ�ת����ɺ�DMA��������Ҳ���ǳ���DMA
ADC_Cmd(ADC1, ENABLE);  //ʹ��ADC1
//ADC_SoftwareStartConv(ADC1);   //��������ⲿ��������������ʼת��

}
/**
  * @brief  LED������
  * @retval None
  */

