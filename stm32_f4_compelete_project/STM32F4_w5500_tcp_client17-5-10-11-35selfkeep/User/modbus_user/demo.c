/*此程序的重点是反复查询这个函数(void) eMBPoll();只有在此函数中查询到modbus上位机
有命令时，才会发送相关的数据，而当其他程序的时间占用的时间比较长，此时相对于查询函数
的时间就比较少，上位机就很难在那查询短时间内发送数据，解决方法是，将查询函数(void) eMBPoll();
执行5000次，此时留在(void) eMBPoll();函数的时间就比较充足，占整个的时间比重才大，上位机在
在查询函数的时间内才能读到相关数据。
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

//#define Device_symbol 0x01 //设备号
#define DATA_BUF_SIZE   1024 //接收缓冲区
extern unsigned char DMA_Symbol;//dma标志位	
extern uint16_t ADC_DATA[10000];
extern unsigned char SEND_Way;
uint8_t gDATABUF[70];
uint8_t databuffer[70]={0};//数据缓冲区

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
	W25QXX_Read((u8 *)&databuffer,0x500000,4);//读取数据
	baudrate=databuffer[0];
	baudrate=(baudrate<<8)|databuffer[1];
	baudrate=(baudrate<<8)|databuffer[2];
	baudrate=(baudrate<<8)|databuffer[3];//波特率
  eMBErrorCode eStatus;
  eStatus=eMBInit(MB_RTU, SlaveAdress, 0,baudrate , MB_PAR_NONE);
  eStatus=eMBEnable();  
  while (1)
  {  
      while(usRegHoldingBuf[0]==0x00)
			{
			  eMBPoll();       		
			}
			Function_Chose();//当不等于0x00时进入功能选择函数
      GPIO_ResetBits(GPIOD, GPIO_Pin_8);//
   	  GPIO_SetBits(GPIOD, GPIO_Pin_9);////指示灯颜色恢复				
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
  switch(getSn_SR(SOCK_TCPS))	//获取socket0的状态		
		{
			//socket初始化完成
			case SOCK_INIT:	
				listen(SOCK_TCPS);			//监听TCP客户端程序连接
				break;
			//socket连接建立
			case SOCK_ESTABLISHED:
         if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
					 {
            //printf("%d:Connected\r\n",SOCK_TCPS);
            setSn_IR(SOCK_TCPS,Sn_IR_CON);
           }
				 ret = recv(SOCK_TCPS,gDATABUF,DATA_BUF_SIZE);	//W5500接收来自Client的数据，ret是接受数据的长度
				 if(((gDATABUF[5]+6)==ret)&&(gDATABUF[6]==Device_symbol))//当发送的字节数与报文中的字节数描述一样时，且设备号一样 则认为报文合法
				 {
					 GPIO_ResetBits(GPIOD, GPIO_Pin_8);//蓝灯的控制引脚关闭					
					 for(ii=0;ii<70;ii++)//将正确报文放到报文缓冲区，防止被覆盖
					 {
					  databuffer[ii]=gDATABUF[ii];
            //USART_SendData(USART1, gDATABUF[ii]);
	          //while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);					 
					 }
					 switch(databuffer[7])//读取命令
					 {						 
					   case 0x01://读线圈状态							 
							 break;
						 case 0x02://读开关输入状态							 
							 break;
             case 0x03://读保持寄存器
							  add=add|databuffer[8];//高八位起始地址
						    add=add<<8;
						    add=add|databuffer[9];//低八位
						    num=num|databuffer[10];//高八位起始地址
						    num=num<<8;
						    num=2*(num|databuffer[11]);//低八位						    
							  eMBRegHoldingCB((UCHAR *)&TX_Buffer[0],(USHORT)add,(USHORT)num,(eMBRegisterMode)MB_REG_READ);/////////						    
						    send(SOCK_TCPS,TX_Buffer,num);			//发送需要读取的数据	 					 
						    add=0;//归位
						    num=0;
							 break;
             case 0x04://读输入寄存器
							  add=add|databuffer[8];//高八位起始地址
						    add=add<<8;
						    add=add|databuffer[9];//低八位
						    num=num|databuffer[10];//高八位起始地址
						    num=num<<8;
						    num=num|databuffer[11];//低八位
							// eMBRegInputCB(&TX_Buffer[0],add,num);
						  eMBRegInputCB((UCHAR *)&TX_Buffer[0],(USHORT)add,(USHORT)num);/////////				
						    send(SOCK_TCPS,(uint8_t *)&TX_Buffer,num*2);			//发送需要读取的数据
						    add=0;//归位
						    num=0;
							 break;
             case 0x05:	//写单个线圈							 
							 break;
             case 0x06://写单个保持寄存器
							  add=add|databuffer[8];//高八位起始地址
						    add=add<<8;
						    add=add|databuffer[9];//低八位
						    num=num|databuffer[10];//高八位起始地址
						    num=num<<8;
						    num=num|databuffer[11];//低八位
							  //eMBRegHoldingCB((unsigned char *)&databuffer[13],add,1,1);
						  eMBRegHoldingCB((UCHAR *)&databuffer[13],(USHORT)add,1,(eMBRegisterMode)MB_REG_WRITE);/////////	
						    add=0;//归位
						    num=0;
							 break;
             case 0x15://写多个线圈							 
               break;						 
						 
             case 0x10://写多个保持寄存器
							  add=add|databuffer[8];//高八位起始地址
						    add=add<<8;
						    add=add|databuffer[9];//低八位
						    num=num|databuffer[10];//高八位起始地址
						    num=num<<8;
						    num=num|databuffer[11];//低八位
							  //eMBRegHoldingCB((unsigned char *)&usRegHoldingBuf,add,num,1);
						 eMBRegHoldingCB((UCHAR *)&databuffer[13],(USHORT)add,num,(eMBRegisterMode)MB_REG_WRITE);/////////						
						    add=0;//归位
						    num=0;
               break;							 
					 }					 
					 			   	
				 }
				 else
				 {
				  ;
				 }			
			
			 break;
			//socket等待关闭状态
			case SOCK_CLOSE_WAIT:
         printf("%d:CloseWait\r\n",SOCK_TCPS);
         if((ret=disconnect(SOCK_TCPS)) != SOCK_OK){
					 break;
				 }
         printf("%d:Closed\r\n",SOCK_TCPS);
         break;
			//socket关闭
			case SOCK_CLOSED:
				//ret = socket(SOCK_TCPS,Sn_MR_TCP,5000,Sn_MR_ND);//打开socket0的一个端口modbus专用端口502
				//if(ret != SOCK_TCPS)
				//{
				//	printf("%d:Socket Error\r\n",SOCK_TCPS);
				//	while(1);
				//}	
        platform_init(info_symbol);	//当检测到断开之后，自动重连					
				break;
     }	
}

/**
  * @brief  数据传输函数
  * @retval None
  */
void data_transmition(int ch)
	{         	       
		     u8 dat[4]={0};
	       uint16_t numtosample=0,kk=0;
				 GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
				 GPIO_ResetBits(GPIOD, GPIO_Pin_8);//以上两句表示关闭指示灯，开始采集数据
         W25QXX_Read((u8 *)&dat,0x400000,4);//读取个数	      
				 numtosample=dat[0];
	       numtosample=(numtosample<<8)|dat[1];//最终频率	
         //printf("%d\t",numtosample);				 
         TIM3_Configration(numtosample);//tim3触发adc配置，输入采集频率
				 
			   ADC_Configration(ch);//配置adc通道并初始化
				 
				 numtosample=dat[2];
	       numtosample=(numtosample<<8)|dat[3];//最终个数
				 // printf("%d\t",numtosample);
			   DMA_Configration(numtosample);//配置dma并启动dma ，输入为采集个数				 
         DMA_Symbol=0;
  	     //DMA_Symbol=DMA_GetFlagStatus( DMA2_Stream0,DMA_FLAG_TCIF0);
	       GPIO_ResetBits(GPIOD, GPIO_Pin_9);//定义a0口的高电平	
	       while(DMA_Symbol!=1)//DMA_GetFlagStatus( DMA2_Stream0,DMA_FLAG_TCIF0)!=SET)//等待DMA2_Steam7传输完成
				 {
				    DMA_GetFlagStatus( DMA2_Stream0,DMA_FLAG_TCIF0);
				 }		 
					//printf("333333333");					
					DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0); 
					DMA_Cmd(DMA2_Stream0, DISABLE);//关闭DMA传输
					DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);//清除DMA2_Steam0传输完成标志					
				  DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,DISABLE);
				  DMA_Symbol=0;//dma传输完成标志归位 	 
 				  GPIO_SetBits(GPIOD, GPIO_Pin_9);// 
				  GPIO_SetBits(GPIOD, GPIO_Pin_8);//以上两句表示开启灯，表示采集完成
				  uint8_t TxBuffer[numtosample*2];
			   for(kk=0;kk<numtosample;kk++)//将采集到的数据发送出去
				 {
				   TxBuffer[kk*2]=ADC_DATA[kk]>>8;
           TxBuffer[kk*2+1]=ADC_DATA[kk];				 
				 }
				 if(SEND_Way<=1)
				 {
					 send(0,(uint8_t *)&TxBuffer,numtosample*2);//以太网发送数据
				 }
				 else if(SEND_Way>=2)
				 {
				     for(kk=0;kk<numtosample;kk++)//将采集到的数据发送出去
				   {
				      USART_SendData(USART1,ADC_DATA[kk]>>8);	  
	            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
						  USART_SendData(USART1,ADC_DATA[kk]);	  
	            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
				   }//串口方式传输数据
				 }
				 else;
				 usRegHoldingBuf[0]=0x00;//归位				
				 return;			  
}
void Function_Chose()
{
  switch(usRegHoldingBuf[0])
		 {
		   case 0x0101://采集1通道数据
				   data_transmition(0);
				   break;
			  case 0x0102://采集2通道数据
				  data_transmition(1);
				  break;
			  case 0x0103://采集3通道数据
				  data_transmition(2);
				  break;
			  case 0x0104://采集4通道数据
				   data_transmition(3);
			     break;
				case 0x0105://采集5通道数据
				   data_transmition(4);
				   break;
				 case 0x0106://采集6通道数据
				    data_transmition(5);
				    break;
				 case 0x0107://采集7通道数据
				    data_transmition(6);
				    break;
				 case 0x0108://采集8通道数据
				    data_transmition(7);
				    break;
				 case 0x0201://修改mac地址	
            MAC_Change();	
            platform_init(1);	//装载更改后设备信息				 
					  break;
				 case 0x0202://修改自身ip地址		
            Self_IP_Change();
            platform_init(1);	//装载更改后设备信息						 
					  break;
				 case 0x0203://修改设备号	
           Device_ID_Change();
				   //platform_init(1);	//装载更改后设备信息		
					  break;
				  case 0x0204://修改服务器ip地址		
            Server_IP_Change();
					  platform_init(1);	//装载更改后设备信息		
					  break;
					case 0x0205://修改自身端口号
						SelfPort_Change();//自身端口号修改
					  platform_init(1);	//装载更改后设备信息
						break;
					case 0x0206://修改服务器端口号
						ServerPort_Change();//服务器端口号修改
					  platform_init(1);	//装载更改后设备信息
						break;
					case 0x0207://修改采样频率
						Sample_Frequency_Change();//采集频率修改
					  //platform_init(1);	//装载更改后设备信息
						break;
					case 0x0208://修改采样点数
						Sample_Number_Change();//采集点数修改
					  //platform_init(1);	//装载更改后设备信息
						break;
					case 0x0209:	//修改波特率
						Buad_Rate_Change();					
						 break;
    			case 0x0301://读取设备信息
          	READ_Device_Information();					
          break;          
				   default:
					 usRegHoldingBuf[0]=0x00;		//其他则归位	
           break;
			 }
		 //return;
		 }
  

    		 