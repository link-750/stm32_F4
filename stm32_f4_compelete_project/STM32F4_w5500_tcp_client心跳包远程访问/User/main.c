/**
  ******************************************************************************
  * @file    main.c
  * $Author: 飞鸿踏雪 $
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
#include "socket.h"	// Just include one header for WIZCHIP
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SOCK_TCPS        0
#define DATA_BUF_SIZE   2048
/* Private macro -------------------------------------------------------------*/
uint8_t gDATABUF[DATA_BUF_SIZE];
uint8_t dat[10]={1111};
// Default Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {202, 199, 236, 116},
                            .sn = {255,255,255,0},
                            .gw = {1, 1, 1, 1},
                            .dns = {8,8,8,8},
                            .dhcp = NETINFO_STATIC };
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void platform_init(void);								// initialize the dependent host peripheral
void network_init(void);								// Initialize Network information and display it
/**
  * @brief  串口打印输出
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t tmp;
	int32_t ret = 0;
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
	uint8_t DstIP[]={202,199,236,112};
	//Host dependent peripheral initialized
	platform_init();
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

	/* PHY link status check */
	do{
		 if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
				printf("Unknown PHY Link stauts.\r\n");
		 }
	}while(tmp == PHY_LINK_OFF);

	/* Network initialization */
	network_init();
	//新建一个Socket并绑定本地端口5000
	ret = socket(SOCK_TCPS,Sn_MR_TCP,5000,0x00);
	if(ret != SOCK_TCPS)
		{
		printf("%d:Socket Error\r\n",SOCK_TCPS);
		while(1);
	}
	else
		{
		printf("%d:Opened\r\n",SOCK_TCPS);
	}
	//连接TCP服务器
	ret = connect(SOCK_TCPS,DstIP,6000);
	if(ret != SOCK_OK){
		printf("%d:Socket Connect Error\r\n",SOCK_TCPS);
		while(1);
	}		
	while(1)
	{
		//下面的两条
    ret = socket(SOCK_TCPS,Sn_MR_TCP,5000,0x00);		
		ret = connect(SOCK_TCPS,DstIP,6000);		
		//接收TCP服务器发送的数据
		//ret = recv(SOCK_TCPS,gDATABUF,DATA_BUF_SIZE);
		//if(ret <= 0)
			//{
			 //  continue;		
		//	}
		//将接收的数据原样返回给TCP服务器
		//ret= send(SOCK_TCPS,gDATABUF,ret);
			ret= send(SOCK_TCPS,dat,ret);
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

/*********************************END OF FILE**********************************/
