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

//��??��1?D??��t??3��D��
//���騰a,?����?USARTx->SR?������?a?a??????��?�䨪?��   	
char RxCounter1,RxBuffer1[BUFLEN];     //?����??o3?,��?�䨮USART_REC_LEN??��??��.
char RxCounter2,RxBuffer2[BUFLEN];     //?����??o3?,��?�䨮USART_REC_LEN??��??��.
char RxCounter3,RxBuffer3[BUFLEN];     //?����??o3?,��?�䨮USART_REC_LEN??��??��.

extern u8 Timeout;

//3?��??��IO ��??��1 
//bound:2����??��
void uart1_init(u32 bound){
	
		GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA  ,  ENABLE );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                   //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;              //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate            = bound  ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;           //8������λ
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;              //1��ֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;              //����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����Ӳ��������
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;  //ʹ�ܷ���/����
	USART_Init(USART1, &USART_InitStructure);                           
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                                  //ʹ�ܽ����ж�
	USART_Cmd(USART1, ENABLE);     
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			                            //�жϷ���2
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 		                          //USART1�����ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                       //��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		                          //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//     //GPIO�˿�����
//    GPIO_InitTypeDef GPIO_InitStructure;
//		USART_InitTypeDef USART_InitStructure;
//		NVIC_InitTypeDef NVIC_InitStructure;
//		 
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
//		USART_DeInit(USART1);  //��λ����1
//	 //USART1_TX   PA.9
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
//   
//    //USART1_RX	  PA.10
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
//    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

//   //Usart1 NVIC ����

//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//��ռ���ȼ�3
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
//		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//		
//		 //USART ��ʼ������

//		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ115200;
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

//    USART_Init(USART1, &USART_InitStructure); //��ʼ������
//    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
//    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}


//3?��??��IO ��??��3 
//bound:2����??��
void uart2_init(u32 bound)
{
//GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ�ܣ�GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//USART2
		USART_DeInit(USART2);  //��λ����2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//115200
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); //��ʼ������
		
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}

//3?��??��IO ��??��3 
//bound:2����??��
void uart3_init(u32 bound)
{
     //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ�ܣ�GPIOAʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//USART3
		USART_DeInit(USART3);  //��λ����3
	 //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB11

   //Usart3 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//115200
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure); //��ʼ������
	
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
}

void Uart1_SendStr(char*SendBuf)//��??��1�䨰��?��y?Y�䨲??
{
    while(*SendBuf)
    {
        while((USART1->SR&0X40)==0)
        {
        }//�̨���y����?������3��
        USART1->DR = (u8) *SendBuf; 
        SendBuf++;
    }
}


void Uart2_SendStr(char*SendBuf)//��??��1�䨰��?��y?Y�䨲??
{
    while(*SendBuf)
    {
        while((USART2->SR&0X40)==0)
        {
        }//�̨���y����?������3��
        USART2->DR = (u8) *SendBuf; 
        SendBuf++;
    }
}

void Uart3_SendStr(char*SendBuf)//��??��1�䨰��?��y?Y�䨲??
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

void USART1_IRQHandler(void)                                //��??��1?D??��t??3��D��
{

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //?����??D??��??����?��??1����????
    {
        buf_uart1.buf[buf_uart1.index++] =USART_ReceiveData(USART1);//?����??��?����?��y?Y
    }
} 


void nbiot_receive_process_event(unsigned char ch )     //��??��2??nbiot��?
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

void USART2_IRQHandler(void)                            //��??��2?����?o����y
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

void USART3_IRQHandler(void)                                //��??��3?D??��t??3��D��
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)   //?����??��?������??��?��y?Y
    {
        buf_uart3.buf[buf_uart3.index++]=USART_ReceiveData(USART3);  //?����??��?����?��y?Y;
    } 

} 	






