#include "main.h"
#include "stdio.h"
#include "sample.h"
#include "w25q128.h"
 unsigned char DMA_Symbol;//dma标志位
 uint16_t ADC_DATA[10000];
/**
  * @brief  LED的配置
  * @retval None
  */
void light_Configration(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);//使能时钟D
  GPIO_InitTypeDef  GPIO_InitStructure;	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;//PD89 通道5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;//开漏输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//开漏
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义端口速度
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);//使能时钟A  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;//PA13-14
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;//输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化 
	
  //GPIO_SetBits(GPIOD, GPIO_Pin_8);//
  //GPIO_SetBits(GPIOD, GPIO_Pin_9);//亮紫色灯	，表示开机正常
}
/**
  * @brief  TIM3的配置
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
	TIM_ARRPreloadConfig(TIM3,ENABLE);//允许TIM3 preload ARR2015
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
  * @brief  DMA的配置
  * @retval None
  */
void DMA_Configration(uint16_t numtosample)	
{
	 NVIC_Configration();//dma中断优先级配置   
/* DMA2 Stream0 channel2 configuration **************************************/
 RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE);//使能DMA时钟 
	DMA_InitTypeDef   DMA_InitStructure;  
	DMA_DeInit(DMA2_Stream0);	
	DMA_Cmd(DMA2_Stream0, DISABLE);                      //开启DMA传输 
	while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){}//等待DMA可配置		
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
	//DMA_SetCurrDataCounter(DMA2_Stream0,1000);          //数据传输量  
	   DMA_ClearFlag(DMA2_Stream0,DMA_IT_TC);
	  // DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TC);
     DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);	 	 
     while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE){;}	//确保DMA可以被设置  
	   DMA_Cmd(DMA2_Stream0, ENABLE);                      //开启DMA传输 
}
   void DMA2_Stream0_IRQHandler(void) //DMAChannel0_IRQHandler(void)
   {   
   	
       if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0))
       {        
				  DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);  
   			  //printf("AAAAA");	
				  DMA_Symbol=1;//DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0);
				  //printf("%d",DMA_Symbol);	
         	//DMA_Cmd(DMA2_Stream0, DISABLE);//关闭DMA传输
					//DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);//清除DMA2_Steam0传输完成标志					
				  //DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,DISABLE);
				  //DMA_Symbol=0;//dma传输完成标志归位 	 
       }
       
   }
/**
  * @brief  adc的配置
  * @retval None
  */
void ADC_Configration(int ch)
{
  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //开ADC时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟	
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 	
	
  GPIO_InitTypeDef  GPIO_InitStructure;	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_0;//PA5 通道5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  	
	
ADC_DeInit();

ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;  //精度为12位          
ADC_InitStructure.ADC_ScanConvMode = DISABLE;   //扫描转换模式失能,单通道不用
ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //连续转换失能
ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising; //上升沿触发转换
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;//ADC_ExternalTrigConvEdge_None;//DC_ExternalTrigConv_None;//定时器3触发
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //数据右对齐，低字节对齐
ADC_InitStructure.ADC_NbrOfConversion = 1;    //规定了顺序进行规则转换的ADC通道的数目
ADC_Init(ADC1, &ADC_InitStructure);      

ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;    //独立模式
ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div6; //分频为6，f(ADC3)=14M
ADC_CommonInitStructure.ADC_DMAAccessMode =ADC_DMAAccessMode_1; //ADC_DMAAccessMode_Disabled;//ADC_DMAAccessMode_2; //使能能DMA_MODE1
//ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两次采样间隔5个周期
ADC_CommonInit(&ADC_CommonInitStructure);
ADC_RegularChannelConfig(ADC1,ch, 1, ADC_SampleTime_112Cycles); //规则通道配置，1表示规则组采样顺序
//ADC_ITConfig(ADC3, ADC_IT_EOC, ENABLE); //使能ADC转换结束中断
//ADC1->SR=0;  // 必不可少，位置必须在启动ADC前
ADC_DMACmd(ADC1, ENABLE);   //使能ADC1的DMA 
ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE); //单通道模式下上次转换完成后DMA请求允许，也就是持续DMA
ADC_Cmd(ADC1, ENABLE);  //使能ADC1
//ADC_SoftwareStartConv(ADC1);   //如果不是外部触发则必须软件开始转换

}
/**
  * @brief  LED的配置
  * @retval None
  */

