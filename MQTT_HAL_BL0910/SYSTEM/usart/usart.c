#include "sys.h"
#include "usart.h"	
#include "stdio.h"
#include "string.h"
 
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40) == 0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

u8 aRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���
UART_HandleTypeDef UART2_Handler; //UART���
//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{	
	//UART ��ʼ������
	UART1_Handler.Instance = USART1;					    //USART1
	UART1_Handler.Init.BaudRate = bound;				    //������
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity = UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
}

void uart2_init(u32 bound)
{	
	//UART ��ʼ������
	UART2_Handler.Instance = USART2;					    //USART1
	UART2_Handler.Init.BaudRate = bound;				    //������
	UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits = UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity = UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode = UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance == USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin = GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull = GPIO_PULLUP;			//����
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;//����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin = GPIO_PIN_10;			//PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,3,3);			//��ռ���ȼ�3�������ȼ�3	
	}
	
	if(huart->Instance == USART2)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART1ʱ��
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin = GPIO_PIN_2;			//PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull = GPIO_PULLUP;			//����
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;//����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin = GPIO_PIN_3;			//PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
		
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART2_IRQn,0,1);			//��ռ���ȼ�3�������ȼ�3	
	}
}

void Uart2_SendDATA(u8*SendBuf,u8 len)//����2��ӡ����
{
  int i;
  for(i = 0; i < len; i++)
	{
	  while((USART2->SR&0X40) == 0){}//�ȴ�������� 
    USART2->DR = (u8) SendBuf[i]; 
	}
}

u16 RxCounter1;
uint16_t RxBuffer1[100];     				//���ջ���,���USART_REC_LEN���ֽ�.
extern uint16_t rx_data[16]; 				//
extern int data_val;
//����1�жϷ������
void USART1_IRQHandler(void)                	
{ 
	u8 i;
	data_val = 0;
//	HAL_StatusTypeDef err;
	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_IT_RXNE) != RESET))  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		RxBuffer1[++RxCounter1] = USART1->DR; 
   	if ((RxBuffer1[RxCounter1 - 19] == 0x0003) && (RxBuffer1[RxCounter1 - 20] == 0x001E))
		{
			data_val = 1;
			for(i = 0; i < 16; i++)
			{
				rx_data[i] = RxBuffer1[RxCounter1 - 17 + i];
			}
			RxCounter1 = 0;
		}
		if(RxCounter1 > 99)
			RxCounter1 = 0;	 
	}
	HAL_UART_IRQHandler(&UART1_Handler);	
} 

char RxBuffer[250];     //���ջ���,
u8 RxCounter;
extern char rx_data2[20]; 
extern int time_val;
extern u8 flag_usart_gsm;
extern char GSM_LongLatistr[100];
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 i;
	time_val = 0;
	flag_usart_gsm = 0;
  if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE) != RESET))  //����ģ�鷵�ص�����
	{    
		RxBuffer[RxCounter++] = USART2->DR;//����ģ�������
		if((RxBuffer[RxCounter - 3] == '+') && (RxBuffer[RxCounter-6] == ':')&&(RxBuffer[RxCounter - 9] == ':')&&(RxBuffer[RxCounter - 15] == '/')&&(RxBuffer[RxCounter - 18] == '/'))
		{
			time_val = 1;
			for(i = 0; i < 20; i++)
			{
				rx_data2[i] = RxBuffer[RxCounter - 20 + i];
			}
			RxCounter = 0;
		}
		if((RxBuffer[RxCounter - 22] == ':') && (RxBuffer[RxCounter - 23] == 'S') && (RxBuffer[RxCounter - 24] == 'B') && (RxBuffer[RxCounter - 25] == 'L') && (RxBuffer[RxCounter - 26] == 'Q') && (RxBuffer[RxCounter - 27] == '+'))
		{
			flag_usart_gsm = 1;
			for(i = 0; i <= 19; i++)
				GSM_LongLatistr[i] = RxBuffer[RxCounter - 18 + i];
			RxCounter = 0;
		}
  }
} 	

