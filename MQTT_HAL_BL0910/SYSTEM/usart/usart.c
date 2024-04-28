#include "sys.h"
#include "usart.h"	
#include "stdio.h"
#include "string.h"
 
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40) == 0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

u8 aRxBuffer[RXBUFFERSIZE];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART1_Handler; //UART句柄
UART_HandleTypeDef UART2_Handler; //UART句柄
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance = USART1;					    //USART1
	UART1_Handler.Init.BaudRate = bound;				    //波特率
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity = UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
}

void uart2_init(u32 bound)
{	
	//UART 初始化设置
	UART2_Handler.Instance = USART2;					    //USART1
	UART2_Handler.Init.BaudRate = bound;				    //波特率
	UART2_Handler.Init.WordLength = UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART2_Handler.Init.StopBits = UART_STOPBITS_1;	    //一个停止位
	UART2_Handler.Init.Parity = UART_PARITY_NONE;		    //无奇偶校验位
	UART2_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //无硬件流控
	UART2_Handler.Init.Mode = UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}

//UART底层初始化，时钟使能，引脚配置，中断配置
//此函数会被HAL_UART_Init()调用
//huart:串口句柄
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance == USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin = GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin = GPIO_PIN_10;			//PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART1_IRQn,3,3);			//抢占优先级3，子优先级3	
	}
	
	if(huart->Instance == USART2)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART2_CLK_ENABLE();			//使能USART1时钟
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin = GPIO_PIN_2;			//PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;//高速
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

		GPIO_Initure.Pin = GPIO_PIN_3;			//PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
		
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//使能USART1中断通道
		HAL_NVIC_SetPriority(USART2_IRQn,0,1);			//抢占优先级3，子优先级3	
	}
}

void Uart2_SendDATA(u8*SendBuf,u8 len)//串口2打印数据
{
  int i;
  for(i = 0; i < len; i++)
	{
	  while((USART2->SR&0X40) == 0){}//等待发送完成 
    USART2->DR = (u8) SendBuf[i]; 
	}
}

u16 RxCounter1;
uint16_t RxBuffer1[100];     				//接收缓冲,最大USART_REC_LEN个字节.
extern uint16_t rx_data[16]; 				//
extern int data_val;
//串口1中断服务程序
void USART1_IRQHandler(void)                	
{ 
	u8 i;
	data_val = 0;
//	HAL_StatusTypeDef err;
	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_IT_RXNE) != RESET))  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
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

char RxBuffer[250];     //接收缓冲,
u8 RxCounter;
extern char rx_data2[20]; 
extern int time_val;
extern u8 flag_usart_gsm;
extern char GSM_LongLatistr[100];
void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 i;
	time_val = 0;
	flag_usart_gsm = 0;
  if((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_RXNE) != RESET))  //接收模块返回的数据
	{    
		RxBuffer[RxCounter++] = USART2->DR;//接收模块的数据
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

