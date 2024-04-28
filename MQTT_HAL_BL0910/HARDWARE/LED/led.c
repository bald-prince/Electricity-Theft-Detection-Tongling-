#include "led.h"


//��ʼ��PA15 PA1   
//LED IO��ʼ��  ��Щ�˿ڸ�����İ��ӵĵ������壬û���ر�ָ��
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	__HAL_RCC_GPIOA_CLK_ENABLE(); 
	__HAL_RCC_GPIOB_CLK_ENABLE(); 
	__HAL_RCC_GPIOC_CLK_ENABLE(); 
	
  GPIO_InitStructure.Pin = GPIO_PIN_15;				 
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //�������
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;		 //IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA15
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);		
	//LED
	GPIO_InitStructure.Pin = GPIO_PIN_8|GPIO_PIN_9;				 
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);					 		//�����趨������ʼ��GPIOB0
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);	

	//BEEP	
	GPIO_InitStructure.Pin = GPIO_PIN_13;				 
  HAL_GPIO_Init(GPIOC,&GPIO_InitStructure);					 			//�����趨������ʼ��GPIOB0
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);	
}
 

void EC200SPWR_Init(void)
{  
	GPIO_InitTypeDef  GPIO_InitStructure;
 	__HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin = GPIO_PIN_8;				 //PWRKEY-->PA8 �˿�����
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //�������
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;		 //IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);		    //PA8 �����  
}

