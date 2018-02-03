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
unsigned char SEND_Way;
extern USHORT usRegHoldingBuf[100];
extern uint8_t databuffer[100];//demo.c全局数组复用
/**
  * @brief  主函数
  * @param  None
  * @retval None
  */
int main(void)
 {		
	u8 Communicate_Way=0,Device_Symble=0;//缓冲数组	
	delay_init(168);	
  light_Configration();	//配置灯函数 
 xMBPortSerialInit(0, 9600,8, MB_PAR_NONE);	
	
	W25QXX_Init();//flash芯片初始化	
	while(W25QXX_ReadID()!=W25Q128)	//检测不到W25Q128,红灯间隔1s闪烁
	{				
	     GPIO_ResetBits(GPIOD, GPIO_Pin_8);//红色闪烁时，蓝色关闭
	     GPIO_SetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);
		   GPIO_ResetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);//未连接上红灯100ms间隔闪烁//红色100ms间隔
	}
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)>0)//PB3默认低电平//判断跳线帽状态，满足条件则将数据放到falsh中
							 {
							   delay_ms(500);//延时排除杂波影响
								 if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)>0)//将初始化信息写入flash中
								 {
									   Init_Information();
								 }
								 else;
							 }
							 else;				
	
	
	
	
	while(1)//大循环检测传送方式
{
	W25QXX_Read((u8*)databuffer,0x600000,2); //读取通讯方式,0x0E表示以太网，0x01表示485通讯，其他值则首先将以太网初始化信息写入flash，再重载成tcp初始模式
  Communicate_Way=databuffer[0];
	W25QXX_Read((u8*)databuffer,0x200000,28);//读取设备号
	Device_Symble=databuffer[26];	
 // printf("%d\t",Device_Symble);	
 // 	while(1);
	if(Communicate_Way==0x0E)//0x0E,修改之后的以太网方式
	{
	          GPIO_ResetBits(GPIOD, GPIO_Pin_9);//红色灯灭
	          GPIO_SetBits(GPIOD, GPIO_Pin_8);//蓝色灯亮
					  platform_init();	//装载修改后的tcp设备信息并初始化	
            SEND_Way=1;//以太网方式		
					 while(1)
					 { 						
						  while(usRegHoldingBuf[0]==0x00)//检测是否有功能函数传入
		         {
					     eMBPoll_TCP(Device_Symble);//在等待传输完成过程中不断对数据进行查询，使用修改之后的信息链接 ，传入设备号              
					   }	
            	Function_Chose();//当不等于0x00时进入功能选择函数	
              GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
	            GPIO_SetBits(GPIOD, GPIO_Pin_8);//指示灯颜色恢复						 
							W25QXX_Read((u8*)databuffer,0x600000,2);//通讯方式
						  Communicate_Way=databuffer[0];
						  W25QXX_Read((u8*)databuffer,0x200000,28);//设备号	读取
              //printf("%d\t",datatemp[0]);
							//printf("%d\t",Device_Symble);						 
							if((Communicate_Way!=0x0e)||(Device_Symble!=databuffer[26]))//如果通讯方式改变或者设备号改变，则跳出大循环重新加载信息
							{break;}
							else;
					 }
	}
	else if(Communicate_Way==0x01)//0x01,485模式
	{
		SEND_Way=3;//485模式
	  GPIO_ResetBits(GPIOD, GPIO_Pin_8);//蓝色灯灭
 	  GPIO_SetBits(GPIOD, GPIO_Pin_9);//红色灯亮   
 	  
		eMBPoll_USART(Device_Symble);//串口modbus只需调用此语句，传入设备号		
	}
	else//将以太网初始化信息放到flash中，改变标志位为0x0E，并以初始化信息重连
	{
      Init_Information();
	}
		
 }	
}
 /**
  * @brief 设备初始信息
  * @retval None
  */


void Init_Information()
{
  u8 datatemp[2]={0};  
	datatemp[0]=0x0E;
	uint8_t Init_Information[]={ 
/*mac0-5*/			                                   0x01,0x08,0xdc,0x00,0xab,0xcd,\
/*自身ip信息6-21*/			                             0xc0,0xa8,0x01,0x7b,\
																										0xff,0xff,0xff,0x0,\
																										0xc0,0xa8,0x1,0x1,\
																										0x08,0x8,0x8,0x8,\
/*目标ip22-25*/		                                 0xc0,0xa8,0x1,0xbe,\
/*设备号26*/		                                     0x01,\
			
/*自身端口号，前高八位后低八位27-28*/	               0x13,0x88,\
/*服务器端口号，前高八位后低八位29-30*/              0x17,0x70,\
/*采样频率，先高八位后低八位31-32*/		               0x03,0xe8,\
/*采样点数，先高八位后低八位33-34*/		               0x0f,0xa0,\
/*波特率，先高后低，一共3个字节，最大115200.35-37*/	 0x00,0x25,0x80};
	  W25QXX_Write((u8*)Init_Information,0x200000,38); //写入到flash中  //初始化信息导入flash
		W25QXX_Write((u8*)datatemp,0x600000,2);//将以太网标志位0x0E信息写入flash中     
  
}
/**
  * @brief  Loopback Test Example Code using ioLibrary_BSD	
  * @retval None
  */
void platform_init()
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
	network_init();	
  return;	 
}
/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void network_init()
{
	
  uint8_t tmpstr[6],infor_str[32];//后面数组用于存储从flash读出的设备信息	
	uint16_t ret = 0; 
	W25QXX_Read((u8 *)&infor_str,0x200000,31);//从0x200000读取22个数据，顺序为下面数组中的的顺序，mac ip sn gw dns	
  wiz_NetInfo gWIZNETINFO = { .mac ={infor_str[0], infor_str[1], infor_str[2],infor_str[3], infor_str[4], infor_str[5]},
                              .ip = {infor_str[6], infor_str[7], infor_str[8], infor_str[9]},
                              .sn = {infor_str[10],infor_str[11],infor_str[12],infor_str[13]},
                              .gw = {infor_str[14], infor_str[15],infor_str[16], infor_str[17]},
                              .dns ={infor_str[18],infor_str[19],infor_str[20],infor_str[21]},
                              .dhcp = NETINFO_STATIC 
                          };//wiz_netinfo为结构体，设备信息													
	//uint8_t DstIP[]={tmpstr[0],tmpstr[1],tmpstr[2],tmpstr[3]};//服务器地址
	uint8_t DstIP[]={infor_str[22],infor_str[23],infor_str[24],infor_str[25]};	
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	//新建一个Socket并绑定本地端口5000         
	        ret=infor_str[27];//高八位
	        ret=(ret<<8)|infor_str[28];//或上低八位    
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
	          ret=infor_str[29];//高八位，目标端口
	          ret=(ret<<8)|infor_str[30];//或上低八位											
	          ret = connect(SOCK_TCPS,DstIP,ret);		
	        		 //LED_Shine(0,100);//红色100ms间隔	        		
	        //  if(ret != SOCK_OK)
	        // {
					//	 	printf("%d:Socket Connect Error\r\n",SOCK_TCPS);
					//	   //while(1);
	        //  }	
								// int k=0;
								// W25QXX_Read((u8 *)&infor_str,0x200000,38);
								//	 for(k=0;k<31;k++)
								//	 {	             	      
								//	   printf("%d\t",infor_str[k]);
								//	 
								//	 }
          //}	
   
  return;		
}

////////////////////////////////////////////////////////////////


