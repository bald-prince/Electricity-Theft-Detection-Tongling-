#include "led.h"


//初始化PA15 PA1   
//LED IO初始化  这些端口根据你的板子的灯来定义，没有特别指定
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOA_CLK_ENABLE(); 
	__HAL_RCC_GPIOB_CLK_ENABLE(); 
	__HAL_RCC_GPIOC_CLK_ENABLE(); 
	
  GPIO_InitStructure.Pin = GPIO_PIN_15;				 
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;		 //IO口速度为50MHz
	GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA15
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);		
	//LED
	GPIO_InitStructure.Pin = GPIO_PIN_8|GPIO_PIN_9;				 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);					 		//根据设定参数初始化GPIOB0
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);	

	//BEEP	
	GPIO_InitStructure.Pin = GPIO_PIN_13;				 
  HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);					 			//根据设定参数初始化GPIOB0
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);	
}
 

void EC200SPWR_Init(void)
{  
	GPIO_InitTypeDef  GPIO_InitStructure;
 	__HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin = GPIO_PIN_8;				 //PWRKEY-->PA8 端口配置
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //推挽输出
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;		 //IO口速度为50MHz
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);		    //PA8 输出高  
}

