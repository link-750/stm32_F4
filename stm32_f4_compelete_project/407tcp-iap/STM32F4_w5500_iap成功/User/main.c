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
#include "socket.h"	// Just include one header for WIZCHIP
#include "iap.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SOCK_TCPS        0
#define DATA_BUF_SIZE   1024*8
#define symbol 0x01
unsigned char TX_Buffer[10];
uint16_t kk;	
/* Private macro -------------------------------------------------------------*/
unsigned char gDATABUF[DATA_BUF_SIZE] __attribute__ ((at(0X20001000)));//数据缓冲区
uint8_t sym[2]={0x11,0x11};

// Default Network Configuration
wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 1, 123},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void platform_init(void);								// initialize the dependent host peripheral
void network_init(void);								// Initialize Network information and display it
void eMBPoll();//查询函数	
void wiznet_register();//设备注册函数															
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
	 
	while(1)
	{		
		   eMBPoll();//在等待传输完成过程中不断对数据进行查询	       		
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
	uint8_t memsize[2][8] = {{8,1,1,1,1,1,1,1},{8,1,1,1,1,1,1,1}};
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
/**
  * @brief  查询函数
  * @retval None
  */
void eMBPoll()
{
	int32_t ret = 0;
	uint16_t add=0,num=0,ii=0;
	
  switch(getSn_SR(SOCK_TCPS))	//获取socket0的状态
		{
			//socket初始化完成
			case SOCK_INIT:	
				listen(SOCK_TCPS);			//监听TCP客户端程序连接
				break;
			//socket连接建立
			case SOCK_ESTABLISHED:
         if(getSn_IR(SOCK_TCPS) & Sn_IR_CON){
            printf("%d:Connected\r\n",SOCK_TCPS);
            setSn_IR(SOCK_TCPS,Sn_IR_CON);
         }
				 ret = recv(SOCK_TCPS,gDATABUF,DATA_BUF_SIZE);	//W5200接收来自Client的数据，ret是接受数据的长度
				
      	 if(ret!=0)//表示接受到数据
				  {
						 
				         if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				      {	 
								
				      	iap_write_appbin(FLASH_APP1_ADDR,gDATABUF,ret);//更新FLASH代码   				
			          printf("overwrite ok");
				      }
		           if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
		          	{	 
									printf("excute app");
		          		iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
		          	}		
           // printf("22222");								
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
				ret = socket(SOCK_TCPS,Sn_MR_TCP,502,Sn_MR_ND);//打开socket0的一个端口modbus专用端口502
				if(ret != SOCK_TCPS){
					printf("%d:Socket Error\r\n",SOCK_TCPS);
					while(1);
				}
				break;
     }	
}


/*********************************END OF FILE**********************************/
