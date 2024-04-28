
#ifndef __DL645_TYPE_H
#define __DL645_TYPE_H
#include "stm32f10x.h"

//===基本数据格式定义=============
#pragma pack(1)

typedef struct {
	u8 Second;
	u8 Minute;
	u8 Hour;
	u8 Day;
	u8 Month : 5;
	u8 Week : 3;	
	u8 Year;
} sDF01;

typedef struct {
//	u32 Dat : 23;
//	u32 S : 1;
	u8 Dat0;
	u8 Dat1;
	u8 Dat2 : 7;
	u8 S : 1;			
} sDF09;

#pragma pack()


#endif	//__DL645_TYPE_H

