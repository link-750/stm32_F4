#include "flash_spi.h"
#include "stdio.h"
u8 SPI1_ReadWriteByte(u8 TxData)
{	   
		unsigned char RecevieData,i;			
    for(i=0;i<8;i++)  
    {  
      SCLK_L;  
      if(TxData&0x80)  
      {  
            MOSI_H;   
      }  
      else  
      {  
           MOSI_L;  
      }  
      TxData<<=1;  
      SCLK_H;    //�����ز���  
      RecevieData<<=1;  
      if(MISO)  
      {  
          RecevieData |= 0x1;  
      }  
      else  
      {  
          RecevieData &= ~0x1;   //�½��ؽ�������  
      }  
    }  
    SCLK_L;   //idle�����SCKΪ��ƽ 
	  return RecevieData;  
		
	}    

	
			 


