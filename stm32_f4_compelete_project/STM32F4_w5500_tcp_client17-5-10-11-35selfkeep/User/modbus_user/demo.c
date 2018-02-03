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
/* ----------------------- Defines ------------------------------------------*/

//#define Device_symbol 0x01 //�豸��
#define DATA_BUF_SIZE   1024 //���ջ�����
extern unsigned char DMA_Symbol;//dma��־λ	
extern uint16_t ADC_DATA[10000];
extern unsigned char SEND_Way;
uint8_t gDATABUF[70];
uint8_t databuffer[70]={0};//���ݻ�����

/* ----------------------- Static variables ---------------------------------*/

//int eMBPoll_USART(void);

/* ----------------------- Start implementation -----------------------------*/

int  eMBPoll_USART(unsigned char SlaveAdress)
{

  
  // usRegHoldingBuf[0]=88;
  // usRegHoldingBuf[2]=77;
  // usRegHoldingBuf[1]=11;
  // usRegHoldingBuf[3]=99;
  // usRegInputBuf[1]=88;
  uint32_t baudrate=0;
	W25QXX_Read((u8 *)&databuffer,0x500000,4);//��ȡ����
	baudrate=databuffer[0];
	baudrate=(baudrate<<8)|databuffer[1];
	baudrate=(baudrate<<8)|databuffer[2];
	baudrate=(baudrate<<8)|databuffer[3];//������
  eMBErrorCode eStatus;
  eStatus=eMBInit(MB_RTU, SlaveAdress, 0,baudrate , MB_PAR_NONE);
  eStatus=eMBEnable();  
  while (1)
  {  
      while(usRegHoldingBuf[0]==0x00)
			{
			  eMBPoll();       		
			}
			Function_Chose();//��������0x00ʱ���빦��ѡ����
      GPIO_ResetBits(GPIOD, GPIO_Pin_8);//
   	  GPIO_SetBits(GPIOD, GPIO_Pin_9);////ָʾ����ɫ�ָ�				
  }	
  return eStatus;
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
void eMBPoll_TCP(unsigned char info_symbol,unsigned char Device_symbol)
{
	int32_t ret = 0;
	unsigned char TX_Buffer[200]={0};
	uint16_t add=0,num=0,ii=0;	
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
				 ret = recv(SOCK_TCPS,gDATABUF,DATA_BUF_SIZE);	//W5500��������Client�����ݣ�ret�ǽ������ݵĳ���
				 if(((gDATABUF[5]+6)==ret)&&(gDATABUF[6]==Device_symbol))//�����͵��ֽ����뱨���е��ֽ�������һ��ʱ�����豸��һ�� ����Ϊ���ĺϷ�
				 {
					 GPIO_ResetBits(GPIOD, GPIO_Pin_8);//���ƵĿ������Źر�					
					 for(ii=0;ii<70;ii++)//����ȷ���ķŵ����Ļ���������ֹ������
					 {
					  databuffer[ii]=gDATABUF[ii];
            //USART_SendData(USART1, gDATABUF[ii]);
	          //while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);					 
					 }
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
        platform_init(info_symbol);	//����⵽�Ͽ�֮���Զ�����					
				break;
     }	
}

/**
  * @brief  ���ݴ��亯��
  * @retval None
  */
void data_transmition(int ch)
	{         	       
		     u8 dat[4]={0};
	       uint16_t numtosample=0,kk=0;
				 GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
				 GPIO_ResetBits(GPIOD, GPIO_Pin_8);//���������ʾ�ر�ָʾ�ƣ���ʼ�ɼ�����
         W25QXX_Read((u8 *)&dat,0x400000,4);//��ȡ����	      
				 numtosample=dat[0];
	       numtosample=(numtosample<<8)|dat[1];//����Ƶ��	
         //printf("%d\t",numtosample);				 
         TIM3_Configration(numtosample);//tim3����adc���ã�����ɼ�Ƶ��
				 
			   ADC_Configration(ch);//����adcͨ������ʼ��
				 
				 numtosample=dat[2];
	       numtosample=(numtosample<<8)|dat[3];//���ո���
				 // printf("%d\t",numtosample);
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
				  uint8_t TxBuffer[numtosample*2];
			   for(kk=0;kk<numtosample;kk++)//���ɼ��������ݷ��ͳ�ȥ
				 {
				   TxBuffer[kk*2]=ADC_DATA[kk]>>8;
           TxBuffer[kk*2+1]=ADC_DATA[kk];				 
				 }
				 if(SEND_Way<=1)
				 {
					 send(0,(uint8_t *)&TxBuffer,numtosample*2);//��̫����������
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
				 case 0x0201://�޸�mac��ַ	
            MAC_Change();	
            platform_init(1);	//װ�ظ��ĺ��豸��Ϣ				 
					  break;
				 case 0x0202://�޸�����ip��ַ		
            Self_IP_Change();
            platform_init(1);	//װ�ظ��ĺ��豸��Ϣ						 
					  break;
				 case 0x0203://�޸��豸��	
           Device_ID_Change();
				   //platform_init(1);	//װ�ظ��ĺ��豸��Ϣ		
					  break;
				  case 0x0204://�޸ķ�����ip��ַ		
            Server_IP_Change();
					  platform_init(1);	//װ�ظ��ĺ��豸��Ϣ		
					  break;
					case 0x0205://�޸�����˿ں�
						SelfPort_Change();//����˿ں��޸�
					  platform_init(1);	//װ�ظ��ĺ��豸��Ϣ
						break;
					case 0x0206://�޸ķ������˿ں�
						ServerPort_Change();//�������˿ں��޸�
					  platform_init(1);	//װ�ظ��ĺ��豸��Ϣ
						break;
					case 0x0207://�޸Ĳ���Ƶ��
						Sample_Frequency_Change();//�ɼ�Ƶ���޸�
					  //platform_init(1);	//װ�ظ��ĺ��豸��Ϣ
						break;
					case 0x0208://�޸Ĳ�������
						Sample_Number_Change();//�ɼ������޸�
					  //platform_init(1);	//װ�ظ��ĺ��豸��Ϣ
						break;
					case 0x0209:	//�޸Ĳ�����
						Buad_Rate_Change();					
						 break;
    			case 0x0301://��ȡ�豸��Ϣ
          	READ_Device_Information();					
          break;          
				   default:
					 usRegHoldingBuf[0]=0x00;		//�������λ	
           break;
			 }
		 //return;
		 }
  

    		 