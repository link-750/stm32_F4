#include "main.h"
#include "stdio.h"
#include "w25q128.h"
#include "socket.h"
#include "mb.h"
#include "mbport.h"
//#include "modbusH.h"
#include "Inforchange.h"
extern uint16_t usRegHoldingBuf[100];
void DeviceInformation_Change()//��Ϣ�޸ĺ���
{
	//���ȼ��������Ϣ�Ƿ�Ϸ�
	unsigned char n=0,j;
	u8 datatemp[40]={0};	
//////////////////////////////////////////////////////
	//�޸�mac
//////////////////////////////////////////////////////
	for(j=1;j<7;j++)
	{
		if(usRegHoldingBuf[j]<=0xff)
		{
			n++;//����У��mac��ַ�Ƿ�Ϸ�������д��flash�����¼��أ������򷵻��޸�ʧ��
		}
		else;
	}

	//////////////////////////////////////////////////////
	//�޸�����ip
	//////////////////////////////////////////////////////
	for(j=7;j<23;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//����Ҫ�󼴼�һ
		}
		else
			;		
	}
	
	//////////////////////////////////////////////////////
	//�޸ķ�����ip
	//////////////////////////////////////////////////////
	for(j=23;j<27;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//����Ҫ�󼴼�һ
		}
		else
			;		
	}

		//////////////////////////////////////////////////////
	//�޸��豸��
	//////////////////////////////////////////////////////
	  if(usRegHoldingBuf[27]<=0xff)//�豸������Ҫ��
	{
		n++;			
	}	
	else
		;	
		//////////////////////////////////////////////////////
	//�޸�����˿ں�
	//////////////////////////////////////////////////////
	if(usRegHoldingBuf[28]<=0xffff)//�˿ں�����Ҫ��
	{
	  n++;			
	}	
	else
		;
	
		//////////////////////////////////////////////////////
	//�޸ķ������˿ں�
	//////////////////////////////////////////////////////
	 if(usRegHoldingBuf[29]<=0xffff)//�豸������Ҫ��
	{
		n++;		
	}	
	else
		;
		//////////////////////////////////////////////////////
	//�޸Ĳ���Ƶ��
	//////////////////////////////////////////////////////
	if(usRegHoldingBuf[30]<=0xffff)//�豸������Ҫ��
	{
	  n++;			
	}	
	else
		;
	//////////////////////////////////////////////////////
	//�޸Ĳ�������
	//////////////////////////////////////////////////////	
	 if(usRegHoldingBuf[31]<=0x2710)//������������Ҫ��С�ڵ���10000����
	{
		n++;	
	}	
	else
		;
//////////////////////////////////////////////////////
	//�޸Ĳ�����
	//////////////////////////////////////////////////////	
	uint32_t baudrate=0;	
	baudrate=usRegHoldingBuf[32];//��16λ
	baudrate=(baudrate<<16)|usRegHoldingBuf[33];//	
  if(baudrate<=0x1c200)//������С�ڵ���115200
	{
		n++;	
	}	
	else
		;
	if(n==32)//���ۼ�ֵ����32ʱ������Ϊ��Ϣȫ��У��ͨ������ʱ�������豸��Ϣ����д��
	{
	  for(j=1;j<28;j++)
		{
		  datatemp[j-1]=usRegHoldingBuf[j];
		}
		datatemp[27]=usRegHoldingBuf[28]>>8;//����˿ںŸ߰�λ
		datatemp[28]=usRegHoldingBuf[28];//����˿ںŵͰ�λ
		datatemp[29]=usRegHoldingBuf[29]>>8;//�������˿ںŸ߰�λ
		datatemp[30]=usRegHoldingBuf[29];//�������˿ڵͰ�λ
		
		datatemp[31]=usRegHoldingBuf[30]>>8;//����Ƶ�ʸ߰�λ
		datatemp[32]=usRegHoldingBuf[30];//����Ƶ�ʵͰ�λ
		datatemp[33]=usRegHoldingBuf[31]>>8;//���������߰�λ
		datatemp[34]=usRegHoldingBuf[31];//���������Ͱ�λ
		
		datatemp[35]=usRegHoldingBuf[32];//��������߰�λ
		datatemp[36]=usRegHoldingBuf[33]>>8;//�������а�λ
		datatemp[37]=usRegHoldingBuf[33];//�����ʵͰ�λ
		
		W25QXX_Write((u8*)datatemp,0x200000,38); //д�뵽flash��  //��ʼ����Ϣ����flash
		//W25QXX_Read((u8*)datatemp,0x200000,38); //��ȡ��������		
		//for(j=1;j<40;j++)
		//{
		//  printf("%d\t",datatemp[j-1]);
		//}
		
	}
	else;
		
		return;
}

 
  void READ_Device_Information(void)//��ȡ�豸��Ϣ	
{
	u8 Device_Information[38] ;//�豸��Ϣһ������38���ֽ�
	//xMBPortSerialInit(0, 9600,8, MB_PAR_NONE);  
	// if(usRegHoldingBuf[1]==0x0A) //��̫����ʽ���
	//{
		W25QXX_Read((u8*)Device_Information,0x200000,38); //��ȡ��������		
		send(0,Device_Information,38);			//���͸�������
		  
	//}
	//else
	//	;
	return;
}


