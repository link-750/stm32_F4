#include "main.h"
#include "stdio.h"
#include "w25q128.h"
#include "socket.h"
#include "mb.h"
#include "mbport.h"
//#include "modbusH.h"
#include "Inforchange.h"
extern uint16_t usRegHoldingBuf[100];
void MAC_Change()//mac��ַ�޸ĺ���
{
	unsigned char n=0,j;
	u8 datatemp1[6];	
	for(j=1;j<7;j++)
	{
		if(usRegHoldingBuf[j]<=0xff)
		{
			n++;//����У��mac��ַ�Ƿ�Ϸ�������д��flash�����¼��أ������򷵻��޸�ʧ��
		}
		else;
	}
	if(n==6)//��n=6ʱ��MAC��ַ����Ҫ��
	{
		for(j=1;j<7;j++)
	{
	  datatemp1[j-1]=usRegHoldingBuf[j];
	}		
	  W25QXX_Write((u8*)datatemp1,0x200000,6); //д�뵽flash��		 
	}
	else  //������n=6ʱ�򷵻�
     ;
		usRegHoldingBuf[0]=0;//��λ	
		return;
}
void Self_IP_Change()//����ip��ַ�޸�
{
  unsigned char n=0,j;
	u8 datatemp2[16];
	for(j=1;j<17;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//����Ҫ�󼴼�һ
		}
		else
			;		
	}
	if(n==16)//У��ͨ��
	{
	  for(j=0;j<16;j++)
	{
	  datatemp2[j]=usRegHoldingBuf[j+1];
	}
	  W25QXX_Write((u8*)datatemp2,0x200006,16); //д�뵽flash��	   
	}
	else
		;
		usRegHoldingBuf[0]=0;//��λ
		return ;
}
void Server_IP_Change(void)//������ip�޸�
{
  unsigned char n=0,j;
	u8 datatemp4[4];
	for(j=1;j<5;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//����Ҫ�󼴼�һ
		}
		else
			;		
	}
	if(n==4)//У��ͨ��
	{
	  for(j=0;j<4;j++)
	{
	  datatemp4[j]=usRegHoldingBuf[j+1];
	}
	  W25QXX_Write((u8*)datatemp4,0x200022,4); //д�뵽flash��	  
	}
	else
		;
		usRegHoldingBuf[0]=0;//��λ
		return ;
}
void Device_ID_Change()//�豸���޸�
{
	u8 datatemp3[2];
  ///u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xff)//�豸������Ҫ��
	{
		datatemp3[0]=usRegHoldingBuf[1];
	  W25QXX_Write((u8*)datatemp3,0x200026,1); //д�뵽flash��	   
    //W25QXX_Read((u8 *)&dat,0x200026,1);//��ȡ����
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);			
	}	
	else
		;

		usRegHoldingBuf[0]=0;//��λ
		return;
}
void SelfPort_Change()//����˿ں��޸�
{
  u8 datatemp5[2];	
	//u8 dat[2]={0};
  if(usRegHoldingBuf[1]<=0xffff)//�˿ں�����Ҫ��
	{
		datatemp5[0]=usRegHoldingBuf[1]>>8;//�߰�λ
		datatemp5[1]=usRegHoldingBuf[1];//�Ͱ�λ
	  W25QXX_Write((u8*)datatemp5,0x300000,2); //д�뵽flash��	 
    //W25QXX_Read((u8 *)&dat,0x300000,2);//��ȡ����
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);			
	}	
	else
		;

		usRegHoldingBuf[0]=0;//��λ
		return;
}
void ServerPort_Change()//�������˿ں��޸�
{
  u8 datatemp6[2];
  //u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xffff)//�豸������Ҫ��
	{
		datatemp6[0]=usRegHoldingBuf[1]>>8;//�߰�λ
		datatemp6[1]=usRegHoldingBuf[1];//�Ͱ�λ
	  W25QXX_Write((u8*)datatemp6,0x300002,2); //д�뵽flash��	   
    //W25QXX_Read((u8 *)&dat,0x300002,2);//��ȡ����
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);		
	}	
	else
		;

		usRegHoldingBuf[0]=0;//��λ
		return;
}
void Sample_Frequency_Change()//�ɼ�Ƶ���޸�
{
  u8 datatemp7[2];	
	//u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xffff)//�豸������Ҫ��
	{
		datatemp7[0]=usRegHoldingBuf[1]>>8;//�߰�λ
		datatemp7[1]=usRegHoldingBuf[1];//�Ͱ�λ
	  W25QXX_Write((u8*)datatemp7,0x400000,2); //д�뵽flash��	 
    //W25QXX_Read((u8 *)&dat,0x400000,2);//��ȡ����
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);				
	}	
	else
		;

		usRegHoldingBuf[0]=0;//��λ
		return;
}
void Sample_Number_Change()//�ɼ������޸�
{
  u8 datatemp8[2];	
	//u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xffff)//�豸������Ҫ��
	{
		datatemp8[0]=usRegHoldingBuf[1]>>8;//�߰�λ
		datatemp8[1]=usRegHoldingBuf[1];//�Ͱ�λ
	  W25QXX_Write((u8*)datatemp8,0x400002,2); //д�뵽flash��	 
  	//W25QXX_Read((u8 *)&dat,0x400002,2);//��ȡ����
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);		
	}	
	else
		;

		usRegHoldingBuf[0]=0;//��λ
		return;
}
void Buad_Rate_Change()//�������޸�
{
  u8 datatemp8[4];	
	uint32_t baudrate=0;
	//u8 dat[4]={0};
	baudrate=usRegHoldingBuf[1];//��16λ
	baudrate=(baudrate<<16)|usRegHoldingBuf[2];//����������
  if(baudrate<=0x1c200)//������С�ڵ���115200
	{
		datatemp8[0]=usRegHoldingBuf[1]>>8;//�߰�λ
		datatemp8[1]=usRegHoldingBuf[1];//�Ͱ�λ
		datatemp8[2]=usRegHoldingBuf[2]>>8;//�߰�λ
		datatemp8[3]=usRegHoldingBuf[2];//�Ͱ�λ
	  W25QXX_Write((u8*)datatemp8,0x500000,4); //д�뵽flash��	 
    //W25QXX_Read((u8 *)&dat,0x500000,4);//��ȡ����
    // printf("%c",dat[0]);
    // printf("%c",dat[1]);
    // printf("%c",dat[2]);
    // printf("%c",dat[3]);			
	}	
	else
		;

		usRegHoldingBuf[0]=0;//��λ
		return;
}
 
  void READ_Device_Information(void)//��ȡ�豸��Ϣ	
{
	u8 Device_Information[16],num=0 ;
	xMBPortSerialInit(0, 9600,8, MB_PAR_NONE);
  if(usRegHoldingBuf[1]==1)//�Դ��ڷ�ʽ���
	{
		W25QXX_Read((u8*)Device_Information,0x200000,6); //��ȡmac
		for(num=0;num<6;num++)
	  {
			printf("%c",Device_Information[num]);//����mac
		}
		W25QXX_Read((u8*)Device_Information,0x200006,16); //��ȡ����ip
		for(num=0;num<16;num++)
	  {
			printf("%c",Device_Information[num]);//��������ip
		}
		W25QXX_Read((u8*)Device_Information,0x200022,4); //��ȡ������ip
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//���ͷ�����ip
		}
		W25QXX_Read((u8*)Device_Information,0x200026,1); //��ȡ�豸��
		printf("%c",Device_Information[0]);//�����豸��
		W25QXX_Read((u8*)Device_Information,0x300000,4);//��ȡ����˿ںź�Ŀ��˿ں�
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//��������˿ںźͷ������˿ں�
		}
		W25QXX_Read((u8*)Device_Information,0x400000,4);//��ȡ����Ƶ�����������
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//���Ͳ���Ƶ�����������
		}
    W25QXX_Read((u8*)Device_Information,0x500000,4);//��ȡ����Ƶ�����������
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//���Ͳ���Ƶ�����������
		}		
		usRegHoldingBuf[0]=0;//����λ���㣬��λ
	}
	else if(usRegHoldingBuf[1]==2) //��̫����ʽ���
	{
		W25QXX_Read((u8*)Device_Information,0x200000,6); //��ȡmac		
		send(0,Device_Information,6);			//����mac		
		W25QXX_Read((u8*)Device_Information,0x200006,16); //��ȡ����ip	
		send(0,Device_Information,16);//��������ip		
		W25QXX_Read((u8*)Device_Information,0x200022,4); //��ȡ������ip		
		send(0,Device_Information,4);//���ͷ�����ip		
		W25QXX_Read((u8*)Device_Information,0x200026,1); //��ȡ�豸��
		send(0,Device_Information,1);//�����豸��
		W25QXX_Read((u8*)Device_Information,0x300000,4);//��ȡ����˿ںź�Ŀ��˿ں�
		send(0,Device_Information,4);//��������˿ںźͷ������˿ں�		
		W25QXX_Read((u8*)Device_Information,0x400000,4);//��ȡ����Ƶ�����������
		send(0,Device_Information,4);//���Ͳ���Ƶ�����������		
    W25QXX_Read((u8*)Device_Information,0x500000,4);//��ȡ������
		send(0,Device_Information,4);		//���Ͳ�����
		usRegHoldingBuf[0]=0;//����λ���㣬��λ	  
	}
	else
		;
	return;
}
