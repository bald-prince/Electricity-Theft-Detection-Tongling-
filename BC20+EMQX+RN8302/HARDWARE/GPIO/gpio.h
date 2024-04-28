#ifndef __GPIO_H
#define __GPIO_H
#include "sys.h"
#include "delay.h"

#define LEDON		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
#define LEDOFF	GPIO_SetBits(GPIOC,GPIO_Pin_13);

void GPIO_INIT(void);
void BC20_GpioInit(void);



#endif
