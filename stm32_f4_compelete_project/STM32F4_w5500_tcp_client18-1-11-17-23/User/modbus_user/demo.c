/*�˳�����ص��Ƿ�����ѯ�������(void) eMBPoll();ֻ���ڴ˺����в�ѯ��modbus��λ��
������ʱ���Żᷢ����ص����ݣ��������������ʱ��ռ�õ�ʱ��Ƚϳ�����ʱ����ڲ�ѯ����
��ʱ��ͱȽ��٣���λ���ͺ������ǲ�ѯ��ʱ���ڷ������ݣ���������ǣ�����ѯ����(void) eMBPoll();
ִ��5000�Σ���ʱ����(void) eMBPoll();������ʱ��ͱȽϳ��㣬ռ������ʱ����زŴ���λ����
�ڲ�ѯ������ʱ���ڲ��ܶ���������ݡ�
 * main.c
 *
 * 
 ----------------------- Modbus includes ----------------------------------*/
#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "modbusH.h"
#include "socket.h"
#include "sample.h"
#include "w25q128.h"
#include "Inforchange.h"
#include "delay.h"
/* ----------------------- Defines ------------------------------------------*/

//#define Device_symbol 0x01 //�豸��
#define DATA_BUF_SIZE   1024 //���ջ�����
extern unsigned char DMA_Symbol;//dma��־λ	
extern uint16_t ADC_DATA[10000];
extern unsigned char SEND_Way;

uint8_t databuffer[100];//���ݻ�����

/* ----------------------- Static variables ---------------------------------*/

//int eMBPoll_USART(void);

/* ----------------------- Start implementation -----------------------------*/
int  eMBPoll_USART(u8 SlaveAdress)
{

  
  // usRegHoldingBuf[0]=88;
  // usRegHoldingBuf[2]=77;
  // usRegHoldingBuf[1]=11;
  // usRegHoldingBuf[3]=99;
  // usRegInputBuf[1]=88;
	
  uint32_t baudrate=0;	
	W25QXX_Read((u8*)databuffer,0x200000,38);//��ȡ����������	,���������ݷŵ���0x200035��0x200036��0x200037�����ֽ��У��ȸߺ��
	baudrate=databuffer[35];
	baudrate=(baudrate<<8)|databuffer[36];
	baudrate=(baudrate<<8)|databuffer[37];
	
  eMBErrorCode eStatus;	
  eStatus=eMBInit(MB_RTU, SlaveAdress,0,baudrate , MB_PAR_NONE);
  eStatus=eMBEnable();
	
  uint8_t ConnectSymble=0; 
  while (1)
  {  
      while(usRegHoldingBuf[0]==0x00)
			{
			  eMBPoll(); 
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)>0)//PB3Ĭ�ϵ͵�ƽ//�ж�����ñ״̬�������ʼ��״̬�ı䣬��ô����ѭ����������ͨѶ��ʽ��־λ	
							 {
							   delay_ms(500);//��ʱ�ų��Ӳ�Ӱ��
								 if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)>0)//��ȷ����Ҫ�ı�ͨѶ��ʽ������£����ñ�־λ��������ѭ��
								 {
									 ConnectSymble=databuffer[0]=0x0E;//��Ϊ��̫����־λ
								   W25QXX_Write((u8*)databuffer,0x600000,2);//����̫����־λ0x0E��Ϣд��flash��																		 
									 break;
								 }
								 else;
							 }
							 else;				
			}
			Function_Chose();//��������0x00ʱ���빦��ѡ����
      GPIO_ResetBits(GPIOD, GPIO_Pin_8);//
   	  GPIO_SetBits(GPIOD, GPIO_Pin_9);////ָʾ����ɫ�ָ� 
      W25QXX_Read((u8*)databuffer,0x200026,1);//�豸�Ŷ�ȡ			
     if((ConnectSymble!=0x01)||(databuffer[0]!=SlaveAdress))//��ͨѶ��־λ������̫��ʱ�����豸�Ÿı䣬������ѭ��
			{
				 break;
			}			
  }	
  return 0;
}

 eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
  eMBErrorCode eStatus = MB_ENOERR;
  int iRegIndex;
  //printf("%d\n", usAddress);
  if ((usAddress >= REG_INPUT_START)		
      && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
      iRegIndex = (int) (usAddress - usRegInputStart);
      while (usNRegs > 0)
        {
          *pucRegBuffer++ = (unsigned char) (usRegInputBuf[iRegIndex] >> 8);
          *pucRegBuffer++ = (unsigned char) (usRegInputBuf[iRegIndex] & 0xFF);
          iRegIndex++;
          usNRegs--;
        }
    }
  else
    {
      eStatus = MB_ENOREG;
    }

  return eStatus;
}

 eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus;
  int iRegIndex;
  if ((usAddress >= REG_HOLDING_START)
      && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
      iRegIndex = (int) (usAddress - usRegHoldingStart);
      switch (eMode)
        {
      case MB_REG_READ:
        while (usNRegs > 0)
          {
            *pucRegBuffer++ = (UCHAR) (usRegHoldingBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (UCHAR) (usRegHoldingBuf[iRegIndex] & 0xFF);
            iRegIndex++;
							// while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
						
	          while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
            usNRegs--;
          }
        break;
      case MB_REG_WRITE:
        while (usNRegs > 0)
          {
            usRegHoldingBuf[iRegIndex] = (USHORT) (*pucRegBuffer++ << 8);
            usRegHoldingBuf[iRegIndex] |= (USHORT) (*pucRegBuffer++);
            iRegIndex++;
            usNRegs--;
          }
        }
    }
  else
    {
      eStatus = MB_ENOREG;
    }
  return eStatus;
}

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus = MB_ENOERR;
  int iNCoils = (int) usNCoils;
  unsigned short usBitOffset;

  /* Check if we have registers mapped at this block. */
  if ((usAddress >= REG_COILS_START)
      && (usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE))
    {
      usBitOffset = (unsigned short) (usAddress - REG_COILS_START);
      switch (eMode)
        {
      /* Read current values and pass to protocol stack. */
      case MB_REG_READ:
        while (iNCoils > 0)
          {
            *pucRegBuffer++ = xMBUtilGetBits(ucRegCoilsBuf, usBitOffset,
                (unsigned char) (iNCoils > 8 ? 8 : iNCoils));
            iNCoils -= 8;
            usBitOffset += 8;
          }
        break;

        /* Update current register values. */
      case MB_REG_WRITE:
        while (iNCoils > 0)
          {
            xMBUtilSetBits(ucRegCoilsBuf, usBitOffset,
                (unsigned char) (iNCoils > 8 ? 8 : iNCoils), *pucRegBuffer++);
            iNCoils -= 8;
            usBitOffset += 8;
          }
        break;
        }

    }
  else
    {
      eStatus = MB_ENOREG;
    }
  return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
  eMBErrorCode eStatus = MB_ENOERR;
  short iNDiscrete = (short) usNDiscrete;
  unsigned short usBitOffset;

  /* Check if we have registers mapped at this block. */
  if ((usAddress >= REG_DISC_START)
      && (usAddress + usNDiscrete <= REG_DISC_START + REG_DISC_SIZE))
    {
      usBitOffset = (unsigned short) (usAddress - REG_DISC_START);
      while (iNDiscrete > 0)
        {
          *pucRegBuffer++ = xMBUtilGetBits(ucRegDiscBuf, usBitOffset,
              (unsigned char) (iNDiscrete > 8 ? 8 : iNDiscrete));
          iNDiscrete -= 8;
          usBitOffset += 8;
        }
    }
  else
    {
      eStatus = MB_ENOREG;
    }
  return eStatus;
}
////////////////////////////////////////////////////////////////////////////////////
 void eMBPoll_TCP(u8 Device_Symble)
{
	int32_t ret = 0;	
	unsigned char TX_Buffer[200]={0};
	uint16_t add=0,num=0;	
  switch(getSn_SR(SOCK_TCPS))	//��ȡsocket0��״̬		
		{
			//socket��ʼ�����
			case SOCK_INIT:	
				listen(SOCK_TCPS);			//����TCP�ͻ��˳�������
				break;
			//socket���ӽ���
			case SOCK_ESTABLISHED:
         if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
					 {
            //printf("%d:Connected\r\n",SOCK_TCPS);
            setSn_IR(SOCK_TCPS,Sn_IR_CON);
           }
				 ret = recv(SOCK_TCPS,databuffer,DATA_BUF_SIZE);	//W5500��������Client�����ݣ�ret�ǽ������ݵĳ���
				 if(((databuffer[5]+6)==ret)&&(databuffer[6]==Device_Symble))//�����͵��ֽ����뱨���е��ֽ�������һ��ʱ�����豸��һ�� ����Ϊ���ĺϷ�
				 {
					 GPIO_ResetBits(GPIOD, GPIO_Pin_8);//���ƵĿ������Źر�	
					
					 switch(databuffer[7])//��ȡ����
					 {						 
					   case 0x01://����Ȧ״̬							 
							 break;
						 case 0x02://����������״̬							 
							 break;
             case 0x03://�����ּĴ���
							  add=add|databuffer[8];//�߰�λ��ʼ��ַ
						    add=add<<8;
						    add=add|databuffer[9];//�Ͱ�λ
						    num=num|databuffer[10];//�߰�λ��ʼ��ַ
						    num=num<<8;
						    num=2*(num|databuffer[11]);//�Ͱ�λ						    
							  eMBRegHoldingCB((UCHAR *)&TX_Buffer[0],(USHORT)add,(USHORT)num,(eMBRegisterMode)MB_REG_READ);/////////						    
						    send(SOCK_TCPS,databuffer,4);//�����������ʶ����Э���ʶ��
						    send(SOCK_TCPS,&databuffer[6],2);//���ͷ����豸��ַ�͹�����
						    send(SOCK_TCPS,TX_Buffer,num);			//������Ҫ��ȡ������	 					 
						    add=0;//��λ
						    num=0;
							 break;
             case 0x04://������Ĵ���
							  add=add|databuffer[8];//�߰�λ��ʼ��ַ
						    add=add<<8;
						    add=add|databuffer[9];//�Ͱ�λ
						    num=num|databuffer[10];//�߰�λ��ʼ��ַ
						    num=num<<8;
						    num=num|databuffer[11];//�Ͱ�λ
							// eMBRegInputCB(&TX_Buffer[0],add,num);
						  eMBRegInputCB((UCHAR *)&TX_Buffer[0],(USHORT)add,(USHORT)num);/////////		
						    send(SOCK_TCPS,databuffer,4);//�����������ʶ����Э���ʶ��
						    send(SOCK_TCPS,&databuffer[6],2);//���ͷ����豸��ַ�͹�����
						    send(SOCK_TCPS,(uint8_t *)&TX_Buffer,num*2);			//������Ҫ��ȡ������
						    add=0;//��λ
						    num=0;
							 break;
             case 0x05:	//д������Ȧ							 
							 break;
             case 0x06://д�������ּĴ���
							  add=add|databuffer[8];//�߰�λ��ʼ��ַ
						    add=add<<8;
						    add=add|databuffer[9];//�Ͱ�λ
						    num=num|databuffer[10];//�߰�λ��ʼ��ַ
						    num=num<<8;
						    num=num|databuffer[11];//�Ͱ�λ
							  //eMBRegHoldingCB((unsigned char *)&databuffer[13],add,1,1);
						  eMBRegHoldingCB((UCHAR *)&databuffer[13],(USHORT)add,1,(eMBRegisterMode)MB_REG_WRITE);/////////	
						  send(SOCK_TCPS,databuffer,4);//�����������ʶ����Э���ʶ��
						  send(SOCK_TCPS,&databuffer[6],2);//���ͷ����豸��ַ�͹�����
						    add=0;//��λ
						    num=0;
							 break;
             case 0x15://д�����Ȧ							 
               break;						 
             case 0x10://д������ּĴ���
							  add=add|databuffer[8];//�߰�λ��ʼ��ַ
						    add=add<<8;
						    add=add|databuffer[9];//�Ͱ�λ
						    num=num|databuffer[10];//�߰�λ��ʼ��ַ
						    num=num<<8;
						    num=num|databuffer[11];//�Ͱ�λ
							  //eMBRegHoldingCB((unsigned char *)&usRegHoldingBuf,add,num,1);
						 eMBRegHoldingCB((UCHAR *)&databuffer[13],(USHORT)add,num,(eMBRegisterMode)MB_REG_WRITE);/////////	
             send(SOCK_TCPS,databuffer,4);//�����������ʶ����Э���ʶ��
						 send(SOCK_TCPS,&databuffer[6],2);//���ͷ����豸��ַ�͹�����						 
						    add=0;//��λ
						    num=0;
               break;							 
					 }					 
					 			   	
				 }
				 else
				 {
				  ;
				 }			
			
			 break;
			//socket�ȴ��ر�״̬
			case SOCK_CLOSE_WAIT:
         printf("%d:CloseWait\r\n",SOCK_TCPS);
         if((ret=disconnect(SOCK_TCPS)) != SOCK_OK){
					 break;
				 }
         printf("%d:Closed\r\n",SOCK_TCPS);
         break;
			//socket�ر�
			case SOCK_CLOSED:
				//ret = socket(SOCK_TCPS,Sn_MR_TCP,5000,Sn_MR_ND);//��socket0��һ���˿�modbusר�ö˿�502
				//if(ret != SOCK_TCPS)
				//{
				//	printf("%d:Socket Error\r\n",SOCK_TCPS);
				//	while(1);
				//}	
        platform_init();	//����⵽�Ͽ�֮���Զ�����					
				break;
     }	
}

/**
  * @brief  ���ݴ��亯��
  * @retval None
  */
void data_transmition(int ch)
	{         	       
		    
	       uint16_t numtosample=0,kk=0,checkcode=0;
				 GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
				 GPIO_ResetBits(GPIOD, GPIO_Pin_8);//���������ʾ�ر�ָʾ�ƣ���ʼ�ɼ�����
				 
         W25QXX_Read((u8 *)&databuffer,0x200000,38);//��ȡ����	      
				 numtosample=databuffer[31];
	       numtosample=(numtosample<<8)|databuffer[32];//����Ƶ��	
				
         TIM3_Configration(numtosample);//tim3����adc���ã�����ɼ�Ƶ��
				 
			   ADC_Configration(ch);//����adcͨ������ʼ��
				 
				 numtosample=databuffer[33];
	       numtosample=(numtosample<<8)|databuffer[34];//���ո��� 
				 
			   DMA_Configration(numtosample);//����dma������dma ������Ϊ�ɼ�����				 
         DMA_Symbol=0;				 
  	     //DMA_Symbol=DMA_GetFlagStatus( DMA2_Stream0,DMA_FLAG_TCIF0);
	       GPIO_ResetBits(GPIOD, GPIO_Pin_9);//����a0�ڵĸߵ�ƽ	
	       while(DMA_Symbol!=1)//DMA_GetFlagStatus( DMA2_Stream0,DMA_FLAG_TCIF0)!=SET)//�ȴ�DMA2_Steam7�������
				 {
				    DMA_GetFlagStatus( DMA2_Stream0,DMA_FLAG_TCIF0);
				 }		 
					//printf("333333333");					
					DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0); 
					DMA_Cmd(DMA2_Stream0, DISABLE);//�ر�DMA����
					DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);//���DMA2_Steam0������ɱ�־					
				  DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,DISABLE);
				  DMA_Symbol=0;//dma������ɱ�־��λ 	 
 				  GPIO_SetBits(GPIOD, GPIO_Pin_9);// 
				  GPIO_SetBits(GPIOD, GPIO_Pin_8);//���������ʾ�����ƣ���ʾ�ɼ����
				  uint8_t TxBuffer[numtosample*2+4];
				 checkcode=ADC_DATA[0];//װ��ɼ���ʼֵ
			   for(kk=0;kk<numtosample;kk++)//���ɼ��������ݷ��ͳ�ȥ
				 {
				   TxBuffer[kk*2]=ADC_DATA[kk]>>8;
           TxBuffer[kk*2+1]=ADC_DATA[kk];	
					 if((kk+1)<numtosample)
					 {
             checkcode=checkcode^ ADC_DATA[kk+1];//�������У����
					 }						 
				 }
				 TxBuffer[numtosample*2+1]= checkcode>>8;//У����߰�λ
				 TxBuffer[numtosample*2+2]=checkcode;//У����Ͱ�λ
				 TxBuffer[numtosample*2+3]=0x0d;
				 TxBuffer[numtosample*2+4]=0x0a;//ֹͣ��
				 
				 if(SEND_Way<=1)
				 {
					 send(0,(uint8_t *)&TxBuffer,numtosample*2+4);//��̫����������					 
				 }
				 else if(SEND_Way>=2)
				 {
				     for(kk=0;kk<numtosample;kk++)//���ɼ��������ݷ��ͳ�ȥ
				   {
				      USART_SendData(USART1,ADC_DATA[kk]>>8);	  
	            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
						  USART_SendData(USART1,ADC_DATA[kk]);	  
	            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
				   }//���ڷ�ʽ��������
					  USART_SendData(USART1,checkcode>>8);//���߰�λУ����	  
	          while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
					  USART_SendData(USART1,checkcode);//���Ͱ�λ	  
	          while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
					  printf("\n");//����ֹͣ��					 
				 }
				 else;
				 usRegHoldingBuf[0]=0x00;//��λ				
				 return;			  
}
void Function_Chose()
{
  switch(usRegHoldingBuf[0])
		 {
		   case 0x0101://�ɼ�1ͨ������
				   data_transmition(0);
				   break;
			  case 0x0102://�ɼ�2ͨ������
				  data_transmition(1);
				  break;
			  case 0x0103://�ɼ�3ͨ������
				  data_transmition(2);
				  break;
			  case 0x0104://�ɼ�4ͨ������
				   data_transmition(3);
			     break;
				case 0x0105://�ɼ�5ͨ������
				   data_transmition(4);
				   break;
				 case 0x0106://�ɼ�6ͨ������
				    data_transmition(5);
				    break;
				 case 0x0107://�ɼ�7ͨ������
				    data_transmition(6);
				    break;
				 case 0x0108://�ɼ�8ͨ������
				    data_transmition(7);
				    break;
				 case 0x0200://�޸���Ϣ
            DeviceInformation_Change();           
            platform_init();	//װ�ظ��ĺ��豸��Ϣ 
            usRegHoldingBuf[0]=0x00;//��λ					 
					  break;
				 case 0x0201://��ȡ��Ϣ
					 READ_Device_Information();
				 	 usRegHoldingBuf[0]=0x00;//��λ	
					 break;
         case 0x0202://�޸�ͨѶ��ʽ 
				  {
						uint8_t ConnectSymble[2]={0x01};
					  W25QXX_Write((u8*)ConnectSymble,0x600000,2);//�����ڱ�־λ0x01��Ϣд��flash��				    
				    usRegHoldingBuf[0]=0x00;//��λ	
						break;
					}							
				   default:
					 usRegHoldingBuf[0]=0x00;		//�������λ	
           break;
			 }
		 //return;
		 }
  


   

   		 