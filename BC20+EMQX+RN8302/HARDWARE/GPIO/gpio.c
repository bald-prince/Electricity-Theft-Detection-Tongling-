#include "gpio.h"


void GPIO_INIT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);	//��GPIOʹ��ʱ�ӣ��ȿ����ù���
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//ӳ��PB3 PB4 �ڵ�GPIO���� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //Ƶ��
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //��ʼ��PB��
	//PB5 PB4 PB3
	GPIO_ResetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9); 	//�رյ�

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //Ƶ��
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //��ʼ��PB��
	//PB5 PB4 PB3
	GPIO_SetBits(GPIOA,GPIO_Pin_1); 	//�رյ�  	
}

/*****************************************************
����ģ�� �͵�ƽ��ģ����й��� �ߵ�ƽ����ģ����й���
*****************************************************/
void BC20_GpioInit(void)
{  
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PC�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 		//PA12ֱ�ӿ���ģ����й���˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);		    //PC3 �����
	
		GPIO_SetBits(GPIOA,GPIO_Pin_12); 	
		delay_ms(1000);
		GPIO_ResetBits(GPIOA,GPIO_Pin_12); 	 
}
