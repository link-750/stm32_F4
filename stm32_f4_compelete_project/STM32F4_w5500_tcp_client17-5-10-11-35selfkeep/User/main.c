/**
  ******************************************************************************
  * @file    main.c
  * $Author:link-chuan  $
  * $Revision: 17 $
  * $Date:: 2017-4-11 11:16:48 +0800 #$
  * @brief   主函数.

  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "delay.h"
#include "spi.h"
#include "port.h"
#include "mb.h"
#include "mbutils.h"
#include "socket.h"
#include "w25q128.h"
#include "sample.h"
///////////////////////////////////
unsigned char SEND_Way;//串口或以太网发送信息标志，等于1时使用以太网，等于2时用串口
extern USHORT usRegHoldingBuf[100];
/**
  * @brief  主函数
  * @param  None
  * @retval None
  */
int main(void)
 {		
	 u8 Device_Num[2]={0};//读取设备号数组	
	//uint32_t retu,p=0; 
	//下面数组是设备初始信息，分别是mac，自身ip，服务器ip，设备号
	//u8 dd[]={0x00,0x08,0xdc,0x00,0xab,0xcd,192, 168, 1, 123,255,255,255,0,192, 168, 1, 1,8,8,8,8,192,168,1,190,0x01};
	//u8 ff[27]={0};		
	//SystemInit();//系统时钟初始化
	delay_init(168);	
	//eMBInit(MB_RTU, 0x01, 0, 115200, MB_PAR_NONE);
	//xMBPortSerialInit(0, 115200,8, MB_PAR_NONE);	
	//USART_Configuration();//串口1初始化
  light_Configration();	//配置灯函数 	
	W25QXX_Init();//flash芯片初始化	
	while(W25QXX_ReadID()!=W25Q128)	//检测不到W25Q128,红灯间隔1s闪烁
	{				
	     GPIO_ResetBits(GPIOD, GPIO_Pin_8);//红色闪烁时，蓝色关闭
	     GPIO_SetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);
		   GPIO_ResetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);//未连接上红灯100ms间隔闪烁//红色100ms间隔
	}	
	//W25QXX_Write((u8*)dd,0x100000,27); //将初始信息写入flash写入到flash中	
	//W25QXX_Read((u8*)&ff,0x100000,27);
  //for(p=0;p<=26;p++)  
	//{
	 	
	//}	
 	W25QXX_Read((u8*)Device_Num,0x200026,1); //读取设备号 
  delay_ms(3000);	//延时3s，使其内部运行稳定	
if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)<=0)//当读取到引脚PB3的状态是低电平时使用以太网modbus，PB3默认电平是低电平
{
	  	
	  SEND_Way=1;//等于1使用以太网传输数据	  	 
    while(1)
     {          
				 if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)<=0)//PB4默认电平是低电平,使用修改后的设备信息链接
     	   {					  
					  GPIO_ResetBits(GPIOD, GPIO_Pin_9);//红色灯灭
	          GPIO_SetBits(GPIOD, GPIO_Pin_8);//蓝色灯亮
					  platform_init(1);	//装载初始设备信息并初始化 
					 while(1)
					 { 
						  while(usRegHoldingBuf[0]==0x00)
		         {
					     eMBPoll_TCP(1,Device_Num[0]);//在等待传输完成过程中不断对数据进行查询，使用修改之后的信息链接    
					   }	
            	Function_Chose();//当不等于0x00时进入功能选择函数	
              GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
	            GPIO_SetBits(GPIOD, GPIO_Pin_8);//指示灯颜色恢复							 
					 }
 				 }						 
         else//使用默认设备信息链接
 				 {				            					 
					  GPIO_SetBits(GPIOD, GPIO_Pin_9);//
	          GPIO_SetBits(GPIOD, GPIO_Pin_8);//紫色色灯亮
					  platform_init(0);	//装载初始设备信息并初始化 
					 while(1)
 				    {
 				   	  while(usRegHoldingBuf[0]==0x00)
            	 {
 		            eMBPoll_TCP(0,Device_Num[0]);//在等待传输完成过程中不断对数据进行查询	，使用默认地址链接
 				     	 }
 				   	   Function_Chose();//当不等于0x00时进入功能选择函数	
					     GPIO_SetBits(GPIOD, GPIO_Pin_9);//
	             GPIO_SetBits(GPIOD, GPIO_Pin_8);//指示灯颜色恢复	
 				    }
 			   }			    	   	   	  			
      }		 
  }
 else//读取到PB3引脚为高电平时使用串口以太网modbus
 {
     
 	  GPIO_ResetBits(GPIOD, GPIO_Pin_8);//蓝色灯灭
 	  GPIO_SetBits(GPIOD, GPIO_Pin_9);//红色灯亮
    SEND_Way=2;//等于3使用串口传输数据		
 	  eMBPoll_USART(Device_Num[0]);//串口modbus只需调用此语句，传入设备号
 		
    
 }
}
/**
  * @brief  Loopback Test Example Code using ioLibrary_BSD	
  * @retval None
  */
void platform_init(char infor_symbol)
{
	uint8_t tmp;	  
	uint8_t memsize[2][8] = {{16,0,0,0,0,0,0,0},{16,0,0,0,0,0,0,0}};
	//SystemInit();//系统时钟初始化
	//USART_Configuration();//串口1初始化	
	//Config SPI
	SPI_Configuration();
	//延时初始化
	//delay_init(168);
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
		 //printf("WIZCHIP Initialized fail.\r\n");
		 while(1);
	}

	/* PHY link status check */
	do{
		 if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1){
				//printf("Unknown PHY Link stauts.\r\n");
		 }
	 }while(tmp == PHY_LINK_OFF);
	network_init(infor_symbol);	
  return;	 
}
/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void network_init(char infor_symbol)
{
	
  uint8_t tmpstr[6],infor_str[26];//后面数组用于存储从flash读出的设备信息	
	uint16_t ret = 0;  	
  if(infor_symbol==0)//当等于0时装载初始信息
  {		
	 W25QXX_Read((u8 *)&infor_str,0x100000,22);//从0x100000读取22个数据，顺序为下面数组中的的顺序，mac 自身ip sn gw dns	目标ip
	 tmpstr[0]=192;tmpstr[1]=168;tmpstr[2]=1;tmpstr[3]=190;
	}	
	else//非0时装载更改之后的信息
	{
	  W25QXX_Read((u8 *)&infor_str,0x200000,22);//从0x200000读取22个数据，顺序为下面数组中的的顺序，mac ip sn gw dns
		W25QXX_Read((u8 *)&tmpstr, 0x200022,4);		
	}
  wiz_NetInfo gWIZNETINFO = { .mac ={infor_str[0], infor_str[1], infor_str[2],infor_str[3], infor_str[4], infor_str[5]},
                              .ip = {infor_str[6], infor_str[7], infor_str[8], infor_str[9]},
                              .sn = {infor_str[10],infor_str[11],infor_str[12],infor_str[13]},
                              .gw = {infor_str[14], infor_str[15],infor_str[16], infor_str[17]},
                              .dns ={infor_str[18],infor_str[19],infor_str[20],infor_str[21]},
                              .dhcp = NETINFO_STATIC 
                          };//wiz_netinfo为结构体，设备信息													
	uint8_t DstIP[]={tmpstr[0],tmpstr[1],tmpstr[2],tmpstr[3]};//服务器地址
	//uint8_t DstIP[]={192,168,1,191};
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	//printf("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr);
	//printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],
	//	  gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	//printf("SIP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
	//printf("GAR: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
	//printf("SUB: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
	//printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);
	//printf("DestinationIP: %d.%d.%d.%d\r\n",  DstIP[0], DstIP[1], DstIP[2], DstIP[3]);
//	printf("======================\r\n");
	//新建一个Socket并绑定本地端口5000
	if(infor_symbol==0)//当等于0时装载初始信息
	  {
	     ret = socket(SOCK_TCPS,Sn_MR_TCP,5000,0x00);
			 ret = connect(SOCK_TCPS,DstIP,6000);	
	  }
	else//非0时装载更改之后的信息
		{
	        W25QXX_Read((u8 *)&tmpstr,0x300000,4);//读取本地端口
	        ret=tmpstr[0];//高八位
	        ret=(ret<<8)|tmpstr[1];//
	        ret = socket(SOCK_TCPS,Sn_MR_TCP,ret,0x00);
	        //if(ret != SOCK_TCPS)
	        //	{
	        //	 printf("%d:Socket Error\r\n",SOCK_TCPS);
	        //	 while(1);
	        //  }
	        //else
	        //	{
	        //  	printf("%d:Opened\r\n",SOCK_TCPS);
	        //  }
	        //连接TCP服务器
	        	//while(ret != SOCK_OK)
	        	//{		
	          ret=tmpstr[2];//高八位
	          ret=(ret<<8)|tmpstr[3];//
	          ret = connect(SOCK_TCPS,DstIP,ret);		
	        		 //LED_Shine(0,100);//红色100ms间隔
	        		
	           //  if(ret != SOCK_OK)
	        		//	 {
	           //  	  printf("%d:Socket Connect Error\r\n",SOCK_TCPS);
	           //    	while(1);
	           //    }	
          //}	
    }
  return;		
}

////////////////////////////////////////////////////////////////


