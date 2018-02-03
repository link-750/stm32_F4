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
unsigned char SEND_Way;
extern USHORT usRegHoldingBuf[100];
extern uint8_t databuffer[100];//demo.cȫ�����鸴��
/**
  * @brief  ������
  * @param  None
  * @retval None
  */
int main(void)
 {		
	u8 Communicate_Way=0,Device_Symble=0;//��������	
	delay_init(168);	
  light_Configration();	//���õƺ��� 
 xMBPortSerialInit(0, 9600,8, MB_PAR_NONE);	
	
	W25QXX_Init();//flashоƬ��ʼ��	
	while(W25QXX_ReadID()!=W25Q128)	//��ⲻ��W25Q128,��Ƽ��1s��˸
	{				
	     GPIO_ResetBits(GPIOD, GPIO_Pin_8);//��ɫ��˸ʱ����ɫ�ر�
	     GPIO_SetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);
		   GPIO_ResetBits(GPIOD, GPIO_Pin_9);
		   delay_ms(100);//δ�����Ϻ��100ms�����˸//��ɫ100ms���
	}
	
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)>0)//PB3Ĭ�ϵ͵�ƽ//�ж�����ñ״̬���������������ݷŵ�falsh��
							 {
							   delay_ms(500);//��ʱ�ų��Ӳ�Ӱ��
								 if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)>0)//����ʼ����Ϣд��flash��
								 {
									   Init_Information();
								 }
								 else;
							 }
							 else;				
	
	
	
	
	while(1)//��ѭ����⴫�ͷ�ʽ
{
	W25QXX_Read((u8*)databuffer,0x600000,2); //��ȡͨѶ��ʽ,0x0E��ʾ��̫����0x01��ʾ485ͨѶ������ֵ�����Ƚ���̫����ʼ����Ϣд��flash�������س�tcp��ʼģʽ
  Communicate_Way=databuffer[0];
	W25QXX_Read((u8*)databuffer,0x200000,28);//��ȡ�豸��
	Device_Symble=databuffer[26];	
 // printf("%d\t",Device_Symble);	
 // 	while(1);
	if(Communicate_Way==0x0E)//0x0E,�޸�֮�����̫����ʽ
	{
	          GPIO_ResetBits(GPIOD, GPIO_Pin_9);//��ɫ����
	          GPIO_SetBits(GPIOD, GPIO_Pin_8);//��ɫ����
					  platform_init();	//װ���޸ĺ��tcp�豸��Ϣ����ʼ��	
            SEND_Way=1;//��̫����ʽ		
					 while(1)
					 { 						
						  while(usRegHoldingBuf[0]==0x00)//����Ƿ��й��ܺ�������
		         {
					     eMBPoll_TCP(Device_Symble);//�ڵȴ�������ɹ����в��϶����ݽ��в�ѯ��ʹ���޸�֮�����Ϣ���� �������豸��              
					   }	
            	Function_Chose();//��������0x00ʱ���빦��ѡ����	
              GPIO_ResetBits(GPIOD, GPIO_Pin_9);//
	            GPIO_SetBits(GPIOD, GPIO_Pin_8);//ָʾ����ɫ�ָ�						 
							W25QXX_Read((u8*)databuffer,0x600000,2);//ͨѶ��ʽ
						  Communicate_Way=databuffer[0];
						  W25QXX_Read((u8*)databuffer,0x200000,28);//�豸��	��ȡ
              //printf("%d\t",datatemp[0]);
							//printf("%d\t",Device_Symble);						 
							if((Communicate_Way!=0x0e)||(Device_Symble!=databuffer[26]))//���ͨѶ��ʽ�ı�����豸�Ÿı䣬��������ѭ�����¼�����Ϣ
							{break;}
							else;
					 }
	}
	else if(Communicate_Way==0x01)//0x01,485ģʽ
	{
		SEND_Way=3;//485ģʽ
	  GPIO_ResetBits(GPIOD, GPIO_Pin_8);//��ɫ����
 	  GPIO_SetBits(GPIOD, GPIO_Pin_9);//��ɫ����   
 	  
		eMBPoll_USART(Device_Symble);//����modbusֻ����ô���䣬�����豸��		
	}
	else//����̫����ʼ����Ϣ�ŵ�flash�У��ı��־λΪ0x0E�����Գ�ʼ����Ϣ����
	{
      Init_Information();
	}
		
 }	
}
 /**
  * @brief �豸��ʼ��Ϣ
  * @retval None
  */


void Init_Information()
{
  u8 datatemp[2]={0};  
	datatemp[0]=0x0E;
	uint8_t Init_Information[]={ 
/*mac0-5*/			                                   0x01,0x08,0xdc,0x00,0xab,0xcd,\
/*����ip��Ϣ6-21*/			                             0xc0,0xa8,0x01,0x7b,\
																										0xff,0xff,0xff,0x0,\
																										0xc0,0xa8,0x1,0x1,\
																										0x08,0x8,0x8,0x8,\
/*Ŀ��ip22-25*/		                                 0xc0,0xa8,0x1,0xbe,\
/*�豸��26*/		                                     0x01,\
			
/*����˿ںţ�ǰ�߰�λ��Ͱ�λ27-28*/	               0x13,0x88,\
/*�������˿ںţ�ǰ�߰�λ��Ͱ�λ29-30*/              0x17,0x70,\
/*����Ƶ�ʣ��ȸ߰�λ��Ͱ�λ31-32*/		               0x03,0xe8,\
/*�����������ȸ߰�λ��Ͱ�λ33-34*/		               0x0f,0xa0,\
/*�����ʣ��ȸߺ�ͣ�һ��3���ֽڣ����115200.35-37*/	 0x00,0x25,0x80};
	  W25QXX_Write((u8*)Init_Information,0x200000,38); //д�뵽flash��  //��ʼ����Ϣ����flash
		W25QXX_Write((u8*)datatemp,0x600000,2);//����̫����־λ0x0E��Ϣд��flash��     
  
}
/**
  * @brief  Loopback Test Example Code using ioLibrary_BSD	
  * @retval None
  */
void platform_init()
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
	network_init();	
  return;	 
}
/**
  * @brief  Intialize the network information to be used in WIZCHIP
  * @retval None
  */
void network_init()
{
	
  uint8_t tmpstr[6],infor_str[32];//�����������ڴ洢��flash�������豸��Ϣ	
	uint16_t ret = 0; 
	W25QXX_Read((u8 *)&infor_str,0x200000,31);//��0x200000��ȡ22�����ݣ�˳��Ϊ���������еĵ�˳��mac ip sn gw dns	
  wiz_NetInfo gWIZNETINFO = { .mac ={infor_str[0], infor_str[1], infor_str[2],infor_str[3], infor_str[4], infor_str[5]},
                              .ip = {infor_str[6], infor_str[7], infor_str[8], infor_str[9]},
                              .sn = {infor_str[10],infor_str[11],infor_str[12],infor_str[13]},
                              .gw = {infor_str[14], infor_str[15],infor_str[16], infor_str[17]},
                              .dns ={infor_str[18],infor_str[19],infor_str[20],infor_str[21]},
                              .dhcp = NETINFO_STATIC 
                          };//wiz_netinfoΪ�ṹ�壬�豸��Ϣ													
	//uint8_t DstIP[]={tmpstr[0],tmpstr[1],tmpstr[2],tmpstr[3]};//��������ַ
	uint8_t DstIP[]={infor_str[22],infor_str[23],infor_str[24],infor_str[25]};	
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	//�½�һ��Socket���󶨱��ض˿�5000         
	        ret=infor_str[27];//�߰�λ
	        ret=(ret<<8)|infor_str[28];//���ϵͰ�λ    
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
	          ret=infor_str[29];//�߰�λ��Ŀ��˿�
	          ret=(ret<<8)|infor_str[30];//���ϵͰ�λ											
	          ret = connect(SOCK_TCPS,DstIP,ret);		
	        		 //LED_Shine(0,100);//��ɫ100ms���	        		
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


