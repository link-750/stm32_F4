/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "main.h"
#include "stdio.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	 if (xRxEnable)
    {
      USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
  else
    {
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    }
  if (xTxEnable)
    {
      //USART_ITConfig(USART1, USART_IT_TC, ENABLE);
      USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
  else
    {
      //USART_ITConfig(USART1, USART_IT_TC, DISABLE);
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    //return FALSE;
	(void) ucPORT;
  (void) ucDataBits;
  (void) eParity;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

  GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = ulBaudRate;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStruct);
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//使能接收中断
	USART_Cmd(USART1, ENABLE);//使能串口3
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	   NVIC_InitTypeDef NVIC_InitStructure;
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
     NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
   return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	  USART_SendData(USART1, ucByte);
	  //while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
	 *pucByte = USART_ReceiveData(USART1);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
 static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}
void USART1_IRQHandler(void)
{
        //uint8_t  dataLen=0;
	    //发生接收中断
  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
     // printf("接收中断\n");
			USART_ClearITPendingBit(USART1, USART_IT_RXNE); //清除中断标志位
      prvvUARTRxISR();
      
    }

  //发生完成中断
 // if (USART_GetITStatus(USART1, USART_IT_TC) == SET)
  if (USART_GetITStatus(USART1, USART_IT_TXE) == SET)
    {
     // printf("完成中断\n");
			USART_ClearITPendingBit(USART1, USART_IT_TXE);//清除中断标志
      prvvUARTTxReadyISR();
     // USART_ClearITPendingBit(USART1, USART_IT_TC);//清除中断标志
     
    }
           
		
       
}
 int fputc(int ch,FILE *f)
{
//ch送给USART1
USART_SendData(USART1, ch);
//等待发送完毕
while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET) { }
//返回ch
return(ch);
}
//需要在target选项中选择使用USE microLIB

//PUTCHAR_PROTOTYPE
//{
//	/* Place your implementation of fputc here */
//	/* e.g. write a character to the USART */
//	USART_SendData(USART1,(uint8_t)ch);
//
//	/* Loop until the end of transmission */
//	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//
//	return ch;
//}
/**
  * @}
  */

/*********************************END OF FILE**********************************/
