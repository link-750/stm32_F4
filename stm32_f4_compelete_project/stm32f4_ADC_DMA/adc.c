/*
*********************************************************************************************************
* Filename      : spi.c
* Version       : V1.00
* Programmer(s) : kxx
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "bsp.h"
#include  "adc.h"
#include  "app_assist.h"
#include  "app_cfg.h"
#include  "sample.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define ADC_CDR_ADDRESS         ((uint32_t)0x40012308)
__IO uint32_t ADCDualConvertedValue[6];

static uint8_t iADCPeriod  = 0;
static uint8_t iADCTestNum = 0;
/*
*********************************************************************************************************
*                                               ADCx_Config()
*
* Description : the ADC Configuration.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) .
*********************************************************************************************************
*/

void ADCx_TIMConfig(TIM_TypeDef* TIMx)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			   			//ʹ�ܶ�ʱ��1ʱ��
    
    TIM_Cmd(TIM3, DISABLE);	
    
    /*
     ********************************************************************************
    system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

    HCLK = SYSCLK / 1     (AHB1Periph)
    PCLK2 = HCLK / 2      (APB2Periph)
    PCLK1 = HCLK / 4      (APB1Periph)

    ��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
    ��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

    APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
    APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
          
    TIM6 ���������� = TIM1CLK / ��TIM_Period + 1��/��TIM_Prescaler + 1��
    ********************************************************************************
    */
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 				   //��ʼ����ʱ��1�ļĴ���Ϊ��λֵ
    TIM_TimeBaseStructure.TIM_Period = 100-1;                          //ARR�Զ���װ�ؼĴ������ڵ�ֵ(��ʱʱ�䣩������Ƶ�ʺ���������»����ж�(Ҳ��˵��ʱʱ�䵽)
    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 160000) - 1;   				   //PSCʱ��Ԥ��Ƶ�� ���磺ʱ��Ƶ��=TIM1CLK/(ʱ��Ԥ��Ƶ+1)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    				   //CR1->CKDʱ��ָ�ֵ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	   //CR1->CMS[1:0]��DIR��ʱ��ģʽ ���ϼ���
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	TIM_UpdateDisableConfig(TIM3, DISABLE); 
    TIM_Cmd(TIM3, ENABLE);	
}

/*
*********************************************************************************************************
*                                               ADCx_Config()
*
* Description : the ADC Configuration.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) .
*********************************************************************************************************
*/

void ADCx_Config(ADC_TypeDef* ADCx)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;
    DMA_InitTypeDef       DMA_InitStructure;

    /* Enable peripheral clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);

    /* Configure ADC Channel 12 pin as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure ADC Channel 12 pin as analog input */ 
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0 | GPIO_Pin_1);
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 |GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
	/* DMA2 Stream0 channel0 config */
	DMA_InitStructure.DMA_Channel            = DMA_Channel_0;                   /* ADC1 ��DMA2_Stream0 ��ͨ��0 �� */
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&ADCDualConvertedValue;/* �ڴ��ַ */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CDR_ADDRESS;       /* �����ַ */
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;      /* �����赽�ڴ洫�� */
    DMA_InitStructure.DMA_BufferSize         = (12>>1);                         /* ����������Ϊ����ͨ����/2 */
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       /* �����ַ���� */
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;            /* �ڴ��ַ���� */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;     /* ���贫�䵥λΪ32λ */
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;         /* �ڴ�洢��λΪ32λ */
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;               /* ѭ������ģʽ */
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;               /* �����ȼ� */
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;            /* ��ʹ��FIFO */    
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;          /* ��ʹ��FIFO */
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;          /* �洢��ͻ�����δ��� */
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;      /* ����ͻ�����δ��� */
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    /* DMA2_Stream0 interrupt enable */
	BSP_EnableIRQ(DMA2_Stream0_IRQn, 3, 0);
    /* DMA2_Stream0 enable */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE);    
   
	/* ADC Common configuration */
    ADC_CommonInitStructure.ADC_Mode             = ADC_DualMode_RegSimult;          /* ˫��ͨ��ͬʱת�� */      
    ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div4;              /* Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/6=84/6=14Mhz,ADCʱ����ò�Ҫ����36Mhz */
    ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_2;             /* DMAģʽ1 2/3 half-word one by one */
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;   /* ����ͨ����ת���ٶ�Ϊ10������ */
    ADC_CommonInit(&ADC_CommonInitStructure);
    /* ADC regular channel 12 configuration */
    ADC_InitStructure.ADC_Resolution            = ADC_Resolution_12b;               /* 12λ���� */  
    ADC_InitStructure.ADC_ScanConvMode          = ENABLE;                           /* ָ����ͨ��ɨ��ʹ�� */
    ADC_InitStructure.ADC_ContinuousConvMode    = DISABLE;                           /* ����ɨ�� */
    ADC_InitStructure.ADC_ExternalTrigConvEdge  = ADC_ExternalTrigConvEdge_Rising;  /* ʹ���ⲿ���� */
    ADC_InitStructure.ADC_ExternalTrigConv      = ADC_ExternalTrigConv_T3_TRGO;      /* ʹ�ö�ʱ��1�ⲿ�ٷ� */
    ADC_InitStructure.ADC_DataAlign             = ADC_DataAlign_Right;              /* ת�������Ҷ��� */
    ADC_InitStructure.ADC_NbrOfConversion       = (12>>1);                          /* ת���Ĵ��� */
    
    /* ADC1 regular channel 12 configuration */
    ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);//UA
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_480Cycles);//UB
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_480Cycles);//UC
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  4, ADC_SampleTime_480Cycles);//IA1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 5, ADC_SampleTime_480Cycles);//U0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 6, ADC_SampleTime_480Cycles);
	ADC_TempSensorVrefintCmd(ENABLE);   
   
    /* ADC2 regular channel 12 configuration */ 
    ADC_Init(ADC2, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_0,  1, ADC_SampleTime_480Cycles);//IA2
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9,  2, ADC_SampleTime_480Cycles);//I0
	ADC_RegularChannelConfig(ADC2, ADC_Channel_2,  3, ADC_SampleTime_480Cycles);//IC2
	ADC_RegularChannelConfig(ADC2, ADC_Channel_14, 4, ADC_SampleTime_480Cycles);//IC1
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3,  5, ADC_SampleTime_480Cycles);//R
	ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 6, ADC_SampleTime_480Cycles);

    /* Enable DMA request after last transfer (multi-ADC mode) */
    ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

    /* ʹ�� ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    
    /* ʹ�� ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    /* ʹ�� ADC2 */
    ADC_Cmd(ADC2, ENABLE);    
    /* ��ʱ������ת�� */ 
    ADCx_TIMConfig(TIM3);    
    /* �������ת�� */
//    ADC_SoftwareStartConv(ADC1);    
}


/*
*********************************************************************************************************
*                                           ADC_DMA_ISR_Handler()
*
* Description : ADC�жϷ�����.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : NONE.
*               
*********************************************************************************************************
*/
extern int adcnt;
void  DMA2_Stream0_IRQHandler (void)
{    
	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) != RESET)
	{
		u32 i,j;

//        DMA_Cmd(DMA2_Stream0, DISABLE);        
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0 | DMA_IT_TEIF0 | DMA_IT_HTIF0);       

        for(i=0,j=0;i<SampleCh_NUM;j++)
        {
            //UA+IA2, UB+IB2, UC+IC2, IA1+IC1, U0+I0, ITEMP+DG, IREF+REF1.65
            SampleData[i++][iADCTestNum] = ADCDualConvertedValue[j]&0xffff;
            SampleData[i++][iADCTestNum] = ADCDualConvertedValue[j]>>16;
        }
        iADCTestNum++;
        adcnt++;
        if(iADCTestNum>=8)  
        {
            iADCTestNum = 0;
            APPTaskSemNotify(&AppTaskADCTCB);
        } 
          
//        DMA_Cmd(DMA2_Stream0, ENABLE);      
    }
}
