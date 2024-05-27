#ifndef __GPIO_H
#define __GPIO_H
#include "sys.h"
#include "delay.h"

#define 	LED1      PBout(8)
#define 	LED2      PBout(9)

void GPIO_INIT(void);
void BC20_GpioInit(void);



#endif
