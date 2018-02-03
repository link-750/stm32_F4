#ifndef __MODBUSH_H_
#define __MODBUSH_H_
#include "port.h"
/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 0001
#define REG_INPUT_NREGS 10
#define REG_HOLDING_START 0001
#define REG_HOLDING_NREGS 100
#define REG_COILS_START     0001
#define REG_COILS_SIZE      16
#define REG_DISC_START     0001
#define REG_DISC_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
 USHORT usRegInputStart = REG_INPUT_START;
 USHORT usRegInputBuf[REG_INPUT_NREGS];
 USHORT usRegHoldingStart = REG_HOLDING_START;
 USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
 UCHAR ucRegCoilsBuf[REG_COILS_SIZE / 8];
 UCHAR ucRegDiscBuf[REG_DISC_SIZE / 8] ={ 0, 0 };

#endif
	
  
         	