#include "gpio.h"


void GPIO_INIT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);	//打开GPIO使能时钟，先开复用功能
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//映射PB3 PB4 口到GPIO功能 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //输出口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //频率
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //初始化PB口
	//PB5 PB4 PB3
	GPIO_ResetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9); 	//关闭灯

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //输出口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //频率
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //初始化PB口
	//PB5 PB4 PB3
	GPIO_SetBits(GPIOA,GPIO_Pin_1); 	//关闭灯  	
}

/*****************************************************
开启模块 低电平对模块进行供电 高电平不对模块进行供电
*****************************************************/
void BC20_GpioInit(void)
{  
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PC端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 		//PA12直接控制模块进行供电端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);		    //PC3 输出高
	
		GPIO_SetBits(GPIOA,GPIO_Pin_12); 	
		delay_ms(1000);
		GPIO_ResetBits(GPIOA,GPIO_Pin_12); 	 
}
