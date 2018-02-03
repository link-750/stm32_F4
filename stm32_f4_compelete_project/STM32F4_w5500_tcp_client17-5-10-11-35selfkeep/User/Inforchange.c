#include "main.h"
#include "stdio.h"
#include "w25q128.h"
#include "socket.h"
#include "mb.h"
#include "mbport.h"
//#include "modbusH.h"
#include "Inforchange.h"
extern uint16_t usRegHoldingBuf[100];
void MAC_Change()//mac地址修改函数
{
	unsigned char n=0,j;
	u8 datatemp1[6];	
	for(j=1;j<7;j++)
	{
		if(usRegHoldingBuf[j]<=0xff)
		{
			n++;//首先校验mac地址是否合法，是则写入flash并重新加载，不是则返回修改失败
		}
		else;
	}
	if(n==6)//当n=6时则MAC地址符合要求
	{
		for(j=1;j<7;j++)
	{
	  datatemp1[j-1]=usRegHoldingBuf[j];
	}		
	  W25QXX_Write((u8*)datatemp1,0x200000,6); //写入到flash中		 
	}
	else  //不满足n=6时则返回
     ;
		usRegHoldingBuf[0]=0;//归位	
		return;
}
void Self_IP_Change()//自身ip地址修改
{
  unsigned char n=0,j;
	u8 datatemp2[16];
	for(j=1;j<17;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//满足要求即加一
		}
		else
			;		
	}
	if(n==16)//校验通过
	{
	  for(j=0;j<16;j++)
	{
	  datatemp2[j]=usRegHoldingBuf[j+1];
	}
	  W25QXX_Write((u8*)datatemp2,0x200006,16); //写入到flash中	   
	}
	else
		;
		usRegHoldingBuf[0]=0;//归位
		return ;
}
void Server_IP_Change(void)//服务器ip修改
{
  unsigned char n=0,j;
	u8 datatemp4[4];
	for(j=1;j<5;j++)
	{
	  if(usRegHoldingBuf[j]<=0xff)
		{
		  n++;//满足要求即加一
		}
		else
			;		
	}
	if(n==4)//校验通过
	{
	  for(j=0;j<4;j++)
	{
	  datatemp4[j]=usRegHoldingBuf[j+1];
	}
	  W25QXX_Write((u8*)datatemp4,0x200022,4); //写入到flash中	  
	}
	else
		;
		usRegHoldingBuf[0]=0;//归位
		return ;
}
void Device_ID_Change()//设备号修改
{
	u8 datatemp3[2];
  ///u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xff)//设备号满足要求
	{
		datatemp3[0]=usRegHoldingBuf[1];
	  W25QXX_Write((u8*)datatemp3,0x200026,1); //写入到flash中	   
    //W25QXX_Read((u8 *)&dat,0x200026,1);//读取数据
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);			
	}	
	else
		;

		usRegHoldingBuf[0]=0;//归位
		return;
}
void SelfPort_Change()//自身端口号修改
{
  u8 datatemp5[2];	
	//u8 dat[2]={0};
  if(usRegHoldingBuf[1]<=0xffff)//端口号满足要求
	{
		datatemp5[0]=usRegHoldingBuf[1]>>8;//高八位
		datatemp5[1]=usRegHoldingBuf[1];//低八位
	  W25QXX_Write((u8*)datatemp5,0x300000,2); //写入到flash中	 
    //W25QXX_Read((u8 *)&dat,0x300000,2);//读取数据
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);			
	}	
	else
		;

		usRegHoldingBuf[0]=0;//归位
		return;
}
void ServerPort_Change()//服务器端口号修改
{
  u8 datatemp6[2];
  //u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xffff)//设备号满足要求
	{
		datatemp6[0]=usRegHoldingBuf[1]>>8;//高八位
		datatemp6[1]=usRegHoldingBuf[1];//低八位
	  W25QXX_Write((u8*)datatemp6,0x300002,2); //写入到flash中	   
    //W25QXX_Read((u8 *)&dat,0x300002,2);//读取数据
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);		
	}	
	else
		;

		usRegHoldingBuf[0]=0;//归位
		return;
}
void Sample_Frequency_Change()//采集频率修改
{
  u8 datatemp7[2];	
	//u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xffff)//设备号满足要求
	{
		datatemp7[0]=usRegHoldingBuf[1]>>8;//高八位
		datatemp7[1]=usRegHoldingBuf[1];//低八位
	  W25QXX_Write((u8*)datatemp7,0x400000,2); //写入到flash中	 
    //W25QXX_Read((u8 *)&dat,0x400000,2);//读取数据
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);				
	}	
	else
		;

		usRegHoldingBuf[0]=0;//归位
		return;
}
void Sample_Number_Change()//采集点数修改
{
  u8 datatemp8[2];	
	//u8 dat[2]={0};	
  if(usRegHoldingBuf[1]<=0xffff)//设备号满足要求
	{
		datatemp8[0]=usRegHoldingBuf[1]>>8;//高八位
		datatemp8[1]=usRegHoldingBuf[1];//低八位
	  W25QXX_Write((u8*)datatemp8,0x400002,2); //写入到flash中	 
  	//W25QXX_Read((u8 *)&dat,0x400002,2);//读取数据
   	//printf("%c",dat[0]);
    //printf("%c",dat[1]);		
	}	
	else
		;

		usRegHoldingBuf[0]=0;//归位
		return;
}
void Buad_Rate_Change()//波特率修改
{
  u8 datatemp8[4];	
	uint32_t baudrate=0;
	//u8 dat[4]={0};
	baudrate=usRegHoldingBuf[1];//高16位
	baudrate=(baudrate<<16)|usRegHoldingBuf[2];//完整波特率
  if(baudrate<=0x1c200)//波特率小于等于115200
	{
		datatemp8[0]=usRegHoldingBuf[1]>>8;//高八位
		datatemp8[1]=usRegHoldingBuf[1];//低八位
		datatemp8[2]=usRegHoldingBuf[2]>>8;//高八位
		datatemp8[3]=usRegHoldingBuf[2];//低八位
	  W25QXX_Write((u8*)datatemp8,0x500000,4); //写入到flash中	 
    //W25QXX_Read((u8 *)&dat,0x500000,4);//读取数据
    // printf("%c",dat[0]);
    // printf("%c",dat[1]);
    // printf("%c",dat[2]);
    // printf("%c",dat[3]);			
	}	
	else
		;

		usRegHoldingBuf[0]=0;//归位
		return;
}
 
  void READ_Device_Information(void)//读取设备信息	
{
	u8 Device_Information[16],num=0 ;
	xMBPortSerialInit(0, 9600,8, MB_PAR_NONE);
  if(usRegHoldingBuf[1]==1)//以串口方式输出
	{
		W25QXX_Read((u8*)Device_Information,0x200000,6); //读取mac
		for(num=0;num<6;num++)
	  {
			printf("%c",Device_Information[num]);//发送mac
		}
		W25QXX_Read((u8*)Device_Information,0x200006,16); //读取自身ip
		for(num=0;num<16;num++)
	  {
			printf("%c",Device_Information[num]);//发送自身ip
		}
		W25QXX_Read((u8*)Device_Information,0x200022,4); //读取服务器ip
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//发送服务器ip
		}
		W25QXX_Read((u8*)Device_Information,0x200026,1); //读取设备号
		printf("%c",Device_Information[0]);//发送设备号
		W25QXX_Read((u8*)Device_Information,0x300000,4);//读取自身端口号和目标端口号
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//发送自身端口号和服务器端口号
		}
		W25QXX_Read((u8*)Device_Information,0x400000,4);//读取采样频率与采样点数
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//发送采样频率与采样点数
		}
    W25QXX_Read((u8*)Device_Information,0x500000,4);//读取采样频率与采样点数
		for(num=0;num<4;num++)
	  {
			printf("%c",Device_Information[num]);//发送采样频率与采样点数
		}		
		usRegHoldingBuf[0]=0;//功能位归零，复位
	}
	else if(usRegHoldingBuf[1]==2) //以太网方式输出
	{
		W25QXX_Read((u8*)Device_Information,0x200000,6); //读取mac		
		send(0,Device_Information,6);			//发送mac		
		W25QXX_Read((u8*)Device_Information,0x200006,16); //读取自身ip	
		send(0,Device_Information,16);//发送自身ip		
		W25QXX_Read((u8*)Device_Information,0x200022,4); //读取服务器ip		
		send(0,Device_Information,4);//发送服务器ip		
		W25QXX_Read((u8*)Device_Information,0x200026,1); //读取设备号
		send(0,Device_Information,1);//发送设备号
		W25QXX_Read((u8*)Device_Information,0x300000,4);//读取自身端口号和目标端口号
		send(0,Device_Information,4);//发送自身端口号和服务器端口号		
		W25QXX_Read((u8*)Device_Information,0x400000,4);//读取采样频率与采样点数
		send(0,Device_Information,4);//发送采样频率与采样点数		
    W25QXX_Read((u8*)Device_Information,0x500000,4);//读取波特率
		send(0,Device_Information,4);		//发送波特率
		usRegHoldingBuf[0]=0;//功能位归零，复位	  
	}
	else
		;
	return;
}
