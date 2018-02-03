#include "main.h"
#include "stdio.h"
#include "w25q128.h"
#include "socket.h"
#include "mb.h"
#include "mbport.h"
//#include "modbusH.h"
#include "Inforchange.h"
extern uint16_t usRegHoldingBuf[100];
void DeviceInformation_Change()//信息修改函数
{
	//首先检验相关信息是否合法
	unsigned char n=0,j;
	u8 datatemp[40]={0};	
//////////////////////////////////////////////////////
	//修改mac
//////////////////////////////////////////////////////
	for(j=1;j<7;j++)
	{
		if(usRegHoldingBuf[j]<=0xff)
		{
			n++;//首先校验mac地址是否合法，是则写入flash并重新加载，不是则返回修改失败
		}
		else;
	}

	//////////////////////////////////////////////////////
	//修改自身ip
	//////////////////////////////////////////////////////
	for(j=7;j<23;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//满足要求即加一
		}
		else
			;		
	}
	
	//////////////////////////////////////////////////////
	//修改服务器ip
	//////////////////////////////////////////////////////
	for(j=23;j<27;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//满足要求即加一
		}
		else
			;		
	}

		//////////////////////////////////////////////////////
	//修改设备号
	//////////////////////////////////////////////////////
	  if(usRegHoldingBuf[27]<=0xff)//设备号满足要求
	{
		n++;			
	}	
	else
		;	
		//////////////////////////////////////////////////////
	//修改自身端口号
	//////////////////////////////////////////////////////
	if(usRegHoldingBuf[28]<=0xffff)//端口号满足要求
	{
	  n++;			
	}	
	else
		;
	
		//////////////////////////////////////////////////////
	//修改服务器端口号
	//////////////////////////////////////////////////////
	 if(usRegHoldingBuf[29]<=0xffff)//设备号满足要求
	{
		n++;		
	}	
	else
		;
		//////////////////////////////////////////////////////
	//修改采样频率
	//////////////////////////////////////////////////////
	if(usRegHoldingBuf[30]<=0xffff)//设备号满足要求
	{
	  n++;			
	}	
	else
		;
	//////////////////////////////////////////////////////
	//修改采样数量
	//////////////////////////////////////////////////////	
	 if(usRegHoldingBuf[31]<=0x2710)//采样数量满足要求小于等于10000个数
	{
		n++;	
	}	
	else
		;
//////////////////////////////////////////////////////
	//修改波特率
	//////////////////////////////////////////////////////	
	uint32_t baudrate=0;	
	baudrate=usRegHoldingBuf[32];//高16位
	baudrate=(baudrate<<16)|usRegHoldingBuf[33];//	
  if(baudrate<=0x1c200)//波特率小于等于115200
	{
		n++;	
	}	
	else
		;
	if(n==32)//当累加值等于32时，则认为信息全部校验通过，此时将所有设备信息重新写入
	{
	  for(j=1;j<28;j++)
		{
		  datatemp[j-1]=usRegHoldingBuf[j];
		}
		datatemp[27]=usRegHoldingBuf[28]>>8;//自身端口号高八位
		datatemp[28]=usRegHoldingBuf[28];//自身端口号低八位
		datatemp[29]=usRegHoldingBuf[29]>>8;//服务器端口号高八位
		datatemp[30]=usRegHoldingBuf[29];//服务器端口低八位
		
		datatemp[31]=usRegHoldingBuf[30]>>8;//采样频率高八位
		datatemp[32]=usRegHoldingBuf[30];//采样频率低八位
		datatemp[33]=usRegHoldingBuf[31]>>8;//采样个数高八位
		datatemp[34]=usRegHoldingBuf[31];//采样个数低八位
		
		datatemp[35]=usRegHoldingBuf[32];//波特率最高八位
		datatemp[36]=usRegHoldingBuf[33]>>8;//波特率中八位
		datatemp[37]=usRegHoldingBuf[33];//波特率低八位
		
		W25QXX_Write((u8*)datatemp,0x200000,38); //写入到flash中  //初始化信息导入flash
		//W25QXX_Read((u8*)datatemp,0x200000,38); //读取各项数据		
		//for(j=1;j<40;j++)
		//{
		//  printf("%d\t",datatemp[j-1]);
		//}
		
	}
	else;
		
		return;
}

 
  void READ_Device_Information(void)//读取设备信息	
{
	u8 Device_Information[38] ;//设备信息一共包含38个字节
	//xMBPortSerialInit(0, 9600,8, MB_PAR_NONE);  
	// if(usRegHoldingBuf[1]==0x0A) //以太网方式输出
	//{
		W25QXX_Read((u8*)Device_Information,0x200000,38); //读取各项数据		
		send(0,Device_Information,38);			//发送各项数据
		  
	//}
	//else
	//	;
	return;
}


