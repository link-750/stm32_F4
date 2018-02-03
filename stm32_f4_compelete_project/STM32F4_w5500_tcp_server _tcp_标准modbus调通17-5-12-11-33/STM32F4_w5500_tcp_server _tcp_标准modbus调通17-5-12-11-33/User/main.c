/**
  ******************************************************************************
  * @file    main.c
  * $Author: chuan$
  * $Revision: 17 $
  * $Date:: 2014-10-25 11:16:48 +0800 #$
  * @brief   主函数.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, EmbedNet</center>
  *<center><a href="http:\\www.embed-net.com">http://www.embed-net.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "spi.h"
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "port.h"
#include "socket.h"	// Just include one header for WIZCHIP
#define SOCK_TCPS        0
// Default Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };
/* Private variables ---------------------------------------------------------*/
														
#define REG_INPUT_START 0001
#define REG_INPUT_NREGS 10
#define REG_HOLDING_START 0001
#define REG_HOLDING_NREGS 100
#define REG_COILS_START     0001
#define REG_COILS_SIZE      16
#define REG_DISC_START     0001
#define REG_DISC_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
 USHORT usRegInputStart = REG_INPUT_START;
 USHORT usRegInputBuf[REG_INPUT_NREGS];
 USHORT usRegHoldingStart = REG_HOLDING_START;
 USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
 UCHAR ucRegCoilsBuf[REG_COILS_SIZE / 8];
 UCHAR ucRegDiscBuf[REG_DISC_SIZE / 8] ={ 0, 0 };

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void platform_init(void);								// initialize the dependent host peripheral
void network_init(void);								// Initialize Network information and display it	
void wiznet_register(void);//设备注册函数	
	void modbus_tcps(uint8_t sn, uint16_t port);											
/**
  * @brief  串口打印输出
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t tmp;
  	
	platform_init();
	wiznet_register();	
	do{
		if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
			{
			  printf("Unknown PHY Link stauts.\r\n");
			  while(1);
		  }
	 }while(tmp == PHY_LINK_OFF);	 
	network_init();	
  eMBTCPInit( 502 ); 
  eMBEnable();	
  usRegInputBuf[1]=0x88;	 
	 usRegHoldingBuf[0]=0x99;
	while(1)
	{			
		//eMBPoll();
		modbus_tcps(0,502);
	}
 }	
void modbus_tcps(uint8_t sn, uint16_t port)
{
   switch(getSn_SR(sn))    //获取socket状态
   {
      case SOCK_CLOSED:     //socket处于关闭状态
          socket(sn,Sn_MR_TCP,port,0x00);  //打开socket
         break;
      case SOCK_INIT :  //socket处于已经初始化状态
          listen(sn);  //监听
      case SOCK_ESTABLISHED :   //socket处于连接状态
                  if(getSn_IR(sn) & Sn_IR_CON)
                     {
                       setSn_IR(sn,Sn_IR_CON);
                     }
										 eMBPoll();
                 
         break;
      case SOCK_CLOSE_WAIT :   //socket处于等待关闭状态
          disconnect(sn); //关闭连接
          break;
      default:
         break;
   }
} 		
 
 
 
/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void network_init(void)
{
   uint8_t tmpstr[6];
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);

	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
		  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	printf("======================\r\n");
}

/**
  * @brief  Loopback Test Example Code using ioLibrary_BSD	
  * @retval None
  */
void platform_init(void)
{
	SystemInit();//系统时钟初始化
	USART_Configuration();//串口1初始化
	printf("\x0c");printf("\x0c");//超级终端清屏
	printf("\033[1;40;32m");//设置超级终端背景为黑色，字符为绿色
	printf("\r\n*******************************************************************************");
	printf("\r\n************************ Copyright 2009-2014, EmbedNet ************************");
	printf("\r\n*************************** http://www.embed-net.com **************************");
	printf("\r\n***************************** All Rights Reserved *****************************");
	printf("\r\n*******************************************************************************");
	printf("\r\n");
	//Config SPI
	SPI_Configuration();
	//延时初始化
	delay_init(168);
}
/**
  * @brief  设备注册函数
  * @retval None
  */
void wiznet_register()
{
	uint8_t memsize[2][8] = {{8,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2}};
  // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
	/* Critical section callback */
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//注册临界区函数
	/* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);  // CS must be tried with LOW.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif
	/* SPI Read & Write callback function */
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);	//注册读写函数

	/* WIZCHIP SOCKET Buffer initialize */
	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){
		 printf("WIZCHIP Initialized fail.\r\n");
		 while(1);
	}
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

