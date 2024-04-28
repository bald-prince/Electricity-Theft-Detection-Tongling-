#include "usart.h"	  
#include "stdio.h"
#include "string.h"

int fputc(int ch, FILE *f)
{
    while((USART1->SR&0X40)==0)
    {}
    USART1->DR = (u8) ch;
    return ch;

}

UART_BUF buf_uart1;     //CH340
UART_BUF buf_uart2;     //NBIOT
UART_BUF buf_uart3;     //TTL

void UART1_send_byte(char data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}

void UART2_send_byte(char data)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, data);
}

void USART2_Send(char *Data,uint16_t Len)
{ 
	uint16_t i;
	for(i=0; i<Len; i++)
	{
		UART2_send_byte(Data[i]);
	}
}


void UART3_send_byte(char data)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, data);
}

//′??ú1?D??·t??3ìDò
//×￠òa,?áè?USARTx->SR?ü±ü?a?a??????μ?′í?ó   	
char RxCounter1,RxBuffer1[BUFLEN];     //?óê??o3?,×?′óUSART_REC_LEN??×??ú.
char RxCounter2,RxBuffer2[BUFLEN];     //?óê??o3?,×?′óUSART_REC_LEN??×??ú.
char RxCounter3,RxBuffer3[BUFLEN];     //?óê??o3?,×?′óUSART_REC_LEN??×??ú.

extern u8 Timeout;

//3?ê??ˉIO ′??ú1 
//bound:2¨ì??ê
void uart1_init(u32 bound){
	
		GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA  ,  ENABLE );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                   //复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate            = bound  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;           //8个数据位
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;              //1个停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;              //无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//禁用硬件流控制
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;  //使能发送/接收
	USART_Init(USART1, &USART_InitStructure);                           
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                  //使能接收中断
	USART_Cmd(USART1, ENABLE);     
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			                            //中断分组2
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 		                          //USART1接收中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //先占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		                          //次占优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//     //GPIO端口设置
//    GPIO_InitTypeDef GPIO_InitStructure;
//		USART_InitTypeDef USART_InitStructure;
//		NVIC_InitTypeDef NVIC_InitStructure;
//		 
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
//		USART_DeInit(USART1);  //复位串口1
//	 //USART1_TX   PA.9
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
//   
//    //USART1_RX	  PA.10
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
//    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

//   //Usart1 NVIC 配置

//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//		
//		 //USART 初始化设置

//		USART_InitStructure.USART_BaudRate = bound;//一般设置为115200;
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

//    USART_Init(USART1, &USART_InitStructure); //初始化串口
//    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
//    USART_Cmd(USART1, ENABLE);                    //使能串口 

}


//3?ê??ˉIO ′??ú3 
//bound:2¨ì??ê
void uart2_init(u32 bound)
{
//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能，GPIOA时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//USART2
		USART_DeInit(USART2);  //复位串口2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

   //Usart1 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//115200
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口
		
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口 

}

//3?ê??ˉIO ′??ú3 
//bound:2¨ì??ê
void uart3_init(u32 bound)
{
     //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能，GPIOA时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//USART3
		USART_DeInit(USART3);  //复位串口3
	 //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PA2
   
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11

   //Usart3 NVIC 配置

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//115200
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口
	
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART3, ENABLE);                    //使能串口 
}

void Uart1_SendStr(char*SendBuf)//′??ú1′òó?êy?Y′ú??
{
    while(*SendBuf)
    {
        while((USART1->SR&0X40)==0)
        {
        }//μè′y·￠?ííê3é
        USART1->DR = (u8) *SendBuf; 
        SendBuf++;
    }
}


void Uart2_SendStr(char*SendBuf)//′??ú1′òó?êy?Y′ú??
{
    while(*SendBuf)
    {
        while((USART2->SR&0X40)==0)
        {
        }//μè′y·￠?ííê3é
        USART2->DR = (u8) *SendBuf; 
        SendBuf++;
    }
}

void Uart3_SendStr(char*SendBuf)//′??ú1′òó?êy?Y′ú??
{
    while(*SendBuf)
    {
        while((USART3->SR&0X40)==0)
        {
        }
        USART3->DR = (u8) *SendBuf; 
        SendBuf++;
    }
}

void USART1_IRQHandler(void)                                //′??ú1?D??·t??3ìDò
{

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //?óê??D??￡??éò?à??1à′????
    {
        buf_uart1.buf[buf_uart1.index++] =USART_ReceiveData(USART1);//?óê??￡?éμ?êy?Y
    }
} 


void nbiot_receive_process_event(unsigned char ch )     //′??ú2??nbiotó?
{
    if(buf_uart2.index >= BUFLEN)
    {
        buf_uart2.index = 0 ;
    }
    else
    {
        buf_uart2.buf[buf_uart2.index++] = ch;
    }
}

void USART2_IRQHandler(void)                            //′??ú2?óê?oˉêy
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)
    {
        nbiot_receive_process_event(USART_ReceiveData(USART2));
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }

    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
    {
        nbiot_receive_process_event(USART_ReceiveData(USART2));
        USART_ClearFlag(USART2,USART_FLAG_ORE);
    }
}

void USART3_IRQHandler(void)                                //′??ú3?D??·t??3ìDò
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)   //?óê??￡?é·μ??μ?êy?Y
    {
        buf_uart3.buf[buf_uart3.index++]=USART_ReceiveData(USART3);  //?óê??￡?éμ?êy?Y;
    } 

} 	






