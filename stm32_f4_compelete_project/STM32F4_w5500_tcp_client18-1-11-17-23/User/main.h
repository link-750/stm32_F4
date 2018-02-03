/**
  ******************************************************************************
  * @file    main.h
  * $Author: wdluo $
  * $Revision: 17 $
  * $Date:: 2012-07-06 11:16:48 +0800 #$
  * @brief   主函数包含的头文件.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, EmbedNet</center>
  *<center><a href="http:\\www.embed-net.com">http://www.embed-net.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_conf.h"
/* Exported Functions --------------------------------------------------------*/
void platform_init(void);								// initialize the dependent host peripheral
void network_init(void);							// Initialize Network information and display it
void Function_Chose(void);               //功能选择函数
void LED_Shine(unsigned char ,uint16_t);                        //led闪烁程序
void Init_Information(void);
#endif /* __MAIN_H */

/*********************************END OF FILE**********************************/
