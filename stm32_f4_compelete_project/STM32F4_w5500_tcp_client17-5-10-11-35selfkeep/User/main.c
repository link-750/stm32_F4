/**
  ******************************************************************************
  * @file    main.c
  * $Author:link-chuan  $
  * $Revision: 17 $
  * $Date:: 2017-4-11 11:16:48 +0800 #$
  * @brief   ������.

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
unsigned char SEND_Way;//���ڻ���̫��������Ϣ��־������1ʱʹ����̫��������2ʱ�ô���
extern USHORT usRegHoldingBuf[100];
/**
  * @brief  ������
  * @param  None
  * @retval None
  */
int main(void)
 {		
	 u8 Device_Num[2]={0};//��ȡ�豸������	
	//uint32_t retu,p=0; 
	//�����������豸��ʼ��Ϣ���ֱ���mac������ip��������ip���豸��
	//u8 dd[]={0x00,0x08,0xdc,0x00,0xab,0xcd,192, 168, 1, 123,255,255,255,0,192, 168, 1, 1,8,8,8,8,192,168,1,190,0x01};
	//u8 ff[27]={0};		
	//SystemInit();//ϵͳʱ�ӳ�ʼ��
	delay_init(168);	
	//eMBInit(MB_RTU, 0x01, 0, 115200, MB_PAR_NONE);
	//xMBPortSerialInit(0, 115200,8, MB_PAR_NONE);	
	//USART_Configuration();//����1��ʼ��
  light_Configration();	//���õƺ��� 	
	W25QXX_Init();//flashоƬ��ʼ��	
	while(W25QXX_ReadID()!=W25Q128)	//��ⲻ��W25Q128,��Ƽ��1s��˸
	{				
	     GPIO_ResetBits(GPIOD, GPIO_Pin_8);//��ɫ��˸ʱ����ɫ�ر�
	     GPIO_SetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);
		   GPIO_ResetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);//δ�����Ϻ��100ms�����˸//��ɫ100ms���
	}	
	//W25QXX_Write((u8*)dd,0x100000,27); //����ʼ��Ϣд��flashд�뵽flash��	
	//W25QXX_Read((u8*)&ff,0x100000,27);
  //for(p=0;p<=26;p++)  
	//{
	 	
	//}	
 	W25QXX_Read((u8*)Device_Num,0x200026,1); //��ȡ�豸�� 
  delay_ms(3000);	//��ʱ3s��ʹ���ڲ������ȶ�	
if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)<=0)//����ȡ������PB3��״̬�ǵ͵�ƽʱʹ����̫��modbus��PB3Ĭ�ϵ�ƽ�ǵ͵�ƽ
{
	  	
	  SEND_Way=1;//����1ʹ����̫����������	  	 
    while(1)
     {          
				 if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)<=0)//PB4Ĭ�ϵ�ƽ�ǵ͵�ƽ,ʹ���޸ĺ���豸��Ϣ����
     	   {					  
					  GPIO_ResetBits(GPIOD, GPIO_Pin_9);//��ɫ����
	          GPIO_SetBits(GPIOD, GPIO_Pin_8);//��ɫ����
					  platform_init(1);	//װ�س�ʼ�豸��Ϣ����ʼ�� 
					 while(1)
					 { 
						  while(usRegHoldingBuf[0]==0x00)
		         {
					     eMBPoll_TCP(1,Device_Num[0]);//�ڵȴ�������ɹ����в��϶����ݽ��в�ѯ��ʹ���޸�֮�����Ϣ����    
					   }	
            	Function_Chose();//��������0x00ʱ���빦��ѡ����	
              GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
	            GPIO_SetBits(GPIOD, GPIO_Pin_8);//ָʾ����ɫ�ָ�							 
					 }
 				 }						 
         else//ʹ��Ĭ���豸��Ϣ����
 				 {				            					 
					  GPIO_SetBits(GPIOD, GPIO_Pin_9);//
	          GPIO_SetBits(GPIOD, GPIO_Pin_8);//��ɫɫ����
					  platform_init(0);	//װ�س�ʼ�豸��Ϣ����ʼ�� 
					 while(1)
 				    {
 				   	  while(usRegHoldingBuf[0]==0x00)
            	 {
 		            eMBPoll_TCP(0,Device_Num[0]);//�ڵȴ�������ɹ����в��϶����ݽ��в�ѯ	��ʹ��Ĭ�ϵ�ַ����
 				     	 }
 				   	   Function_Chose();//��������0x00ʱ���빦��ѡ����	
					     GPIO_SetBits(GPIOD, GPIO_Pin_9);//
	             GPIO_SetBits(GPIOD, GPIO_Pin_8);//ָʾ����ɫ�ָ�	
 				    }
 			   }			    	   	   	  			
      }		 
  }
 else//��ȡ��PB3����Ϊ�ߵ�ƽʱʹ�ô�����̫��modbus
 {
     
 	  GPIO_ResetBits(GPIOD, GPIO_Pin_8);//��ɫ����
 	  GPIO_SetBits(GPIOD, GPIO_Pin_9);//��ɫ����
    SEND_Way=2;//����3ʹ�ô��ڴ�������		
 	  eMBPoll_USART(Device_Num[0]);//����modbusֻ����ô���䣬�����豸��
 		
    
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
	//SystemInit();//ϵͳʱ�ӳ�ʼ��
	//USART_Configuration();//����1��ʼ��	
	//Config SPI
	SPI_Configuration();
	//��ʱ��ʼ��
	//delay_init(168);
		// First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP 
	/* Critical section callback */
	reg_wizchip_cris_cbfunc(SPI_CrisEnter, SPI_CrisExit);	//ע���ٽ�������
	/* Chip selection call back */
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
	reg_wizchip_cs_cbfunc(SPI_CS_Select, SPI_CS_Deselect);//ע��SPIƬѡ�źź���
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
	reg_wizchip_spi_cbfunc(SPI_ReadByte, SPI_WriteByte);	//ע���д����
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
	
  uint8_t tmpstr[6],infor_str[26];//�����������ڴ洢��flash�������豸��Ϣ	
	uint16_t ret = 0;  	
  if(infor_symbol==0)//������0ʱװ�س�ʼ��Ϣ
  {		
	 W25QXX_Read((u8 *)&infor_str,0x100000,22);//��0x100000��ȡ22�����ݣ�˳��Ϊ���������еĵ�˳��mac ����ip sn gw dns	Ŀ��ip
	 tmpstr[0]=192;tmpstr[1]=168;tmpstr[2]=1;tmpstr[3]=190;
	}	
	else//��0ʱװ�ظ���֮�����Ϣ
	{
	  W25QXX_Read((u8 *)&infor_str,0x200000,22);//��0x200000��ȡ22�����ݣ�˳��Ϊ���������еĵ�˳��mac ip sn gw dns
		W25QXX_Read((u8 *)&tmpstr, 0x200022,4);		
	}
  wiz_NetInfo gWIZNETINFO = { .mac ={infor_str[0], infor_str[1], infor_str[2],infor_str[3], infor_str[4], infor_str[5]},
                              .ip = {infor_str[6], infor_str[7], infor_str[8], infor_str[9]},
                              .sn = {infor_str[10],infor_str[11],infor_str[12],infor_str[13]},
                              .gw = {infor_str[14], infor_str[15],infor_str[16], infor_str[17]},
                              .dns ={infor_str[18],infor_str[19],infor_str[20],infor_str[21]},
                              .dhcp = NETINFO_STATIC 
                          };//wiz_netinfoΪ�ṹ�壬�豸��Ϣ													
	uint8_t DstIP[]={tmpstr[0],tmpstr[1],tmpstr[2],tmpstr[3]};//��������ַ
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
	//�½�һ��Socket���󶨱��ض˿�5000
	if(infor_symbol==0)//������0ʱװ�س�ʼ��Ϣ
	  {
	     ret = socket(SOCK_TCPS,Sn_MR_TCP,5000,0x00);
			 ret = connect(SOCK_TCPS,DstIP,6000);	
	  }
	else//��0ʱװ�ظ���֮�����Ϣ
		{
	        W25QXX_Read((u8 *)&tmpstr,0x300000,4);//��ȡ���ض˿�
	        ret=tmpstr[0];//�߰�λ
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
	        //����TCP������
	        	//while(ret != SOCK_OK)
	        	//{		
	          ret=tmpstr[2];//�߰�λ
	          ret=(ret<<8)|tmpstr[3];//
	          ret = connect(SOCK_TCPS,DstIP,ret);		
	        		 //LED_Shine(0,100);//��ɫ100ms���
	        		
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


