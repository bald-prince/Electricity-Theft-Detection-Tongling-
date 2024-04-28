#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "bl0910.h"

extern TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú 
extern u32 task_cnt;
void TIM3_Init(u16 arr,u16 psc);
#endif

