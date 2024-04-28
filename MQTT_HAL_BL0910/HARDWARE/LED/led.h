#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

void LED_Init(void);//初始化
void EC200SPWR_Init(void);

//#define LED1      PBout(4)
#define 	SWTCTR    PBout(0)
//#define READLED0 	GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_3)

#define 	LED0      PBout(9)
#define 	LEDStatus PBout(8)
#define 	PWRKEY    PAout(8)
#define 	BEEP			PCout(13)
#define 	SWITCH		GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_15)

#endif
