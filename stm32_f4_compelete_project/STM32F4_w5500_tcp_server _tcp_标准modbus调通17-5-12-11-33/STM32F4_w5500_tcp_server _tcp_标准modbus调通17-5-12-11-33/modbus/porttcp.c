/*
* FreeModbus Libary: lwIP Port
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
* File: $Id: porttcp.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
*/

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <string.h>
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "socket.h"
/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7

#define MB_TCP_DEFAULT_PORT  502          /* TCP listening port. */
#define MB_TCP_BUF_SIZE     ( 256 + 7 )   /* Must hold a complete Modbus TCP frame. */

/* ----------------------- Prototypes ---------------------------------------*/
static UCHAR    aucTCPBuf[MB_TCP_BUF_SIZE];	   //接收缓冲区
static USHORT   usTCPBufLen;


BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
   SOCKET sn;
    sn=0;
    if(getSn_SR(sn)==SOCK_CLOSED)
    {
       socket(sn,Sn_MR_TCP,usTCPPort,0x00);  //打开socket
    }
    if (getSn_SR(sn)==SOCK_INIT)
    {
     listen(sn);  //监听
     return TRUE;
    }
    return FALSE;
}


void 
vMBTCPPortClose(  )
{
	
}

void
vMBTCPPortDisable( void )
{
	   
}

BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{
	*ppucMBTCPFrame = &aucTCPBuf[0];
    *usTCPLength = usTCPBufLen;
    /* Reset the buffer. */
    usTCPBufLen = 0;
    return TRUE;
}

BOOL
xMBTCPPortSendResponse(const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
	
	send(0,(UCHAR*)pucMBTCPFrame,usTCPLength);			
    return TRUE;
}



BOOL
xMBPortTCPPool( void )
{  
	unsigned short int us_rlen;
		us_rlen	= recv(0,aucTCPBuf,1024);
		if(us_rlen==0)
		return FALSE;
		usTCPBufLen = us_rlen;				 //读到新数据，得到长度
	( void )xMBPortEventPost( EV_FRAME_RECEIVED );			//发送已接收到新数据到Modbus-TCP状态机

		if(getSn_SR(0)==SOCK_CLOSED)		/* TCP Disconnect */
		{
			listen(0);			//监听TCP客户端程序连接
		}
	

	
	
	return TRUE;
}

void
EnterCriticalSection( void )
{
  __disable_irq();
}

void
ExitCriticalSection( void )
{
  __enable_irq();
}
