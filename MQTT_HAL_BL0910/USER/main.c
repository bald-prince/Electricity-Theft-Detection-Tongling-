#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "wdg.h"
#include "EC200S.h"	
#include "math.h"			
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "bl0910.h"
#include "timer.h"

extern char RxBuffer[100],RxCounter;
extern unsigned char uart1_getok;
extern int RxCounter1;
extern uint16_t RxBuffer1[100];
u8 temp,humi;
u16 i;
int data_val;
char rx_data2[20];
int time_val;
u8 flag_usart_gsm;
int t_json_len;
uint16_t rx_data[16];

void Pub_Msgdata_current(void);
void Pub_Msgdata_Energy(void);
void Pub_Msgdata_Freq(void);
int main(void)
{ 
  HAL_Init();                    												//��ʼ��HAL��    
  Stm32_Clock_Init(RCC_PLL_MUL9);	 											//����ʱ��,72M
	delay_init(72);               												//��ʼ����ʱ����
	LED_Init();																						//��ʼ��LED	
	EC200SPWR_Init();																			//��EC200S����
	uart_init(115200);																		//����1��ʼ������ӡ�����Ե�������
  uart2_init(115200);																		//�Խ�EC200S
	IWDG_Init(7,625);
	PWRKEY = 1;
	EC200S_Init();																				//���豸��ʼ��
	//EC200U_Init();
  MQTT_Init();																					//��ʼ��MQTT
	SPI1_Init();	
	BL0910_Init();
	TIM3_Init(10000-1,7200-1);
	
	//���û�ȡ��վ��γ������
	printf("AT+QLBSCFG=\"token\",\"7313Z68310k1100W\"\r\n"); 
	//printf("AT+QLBSCFG=\"token\",\"083uwqYo3OJ00NP0\"\r\n"); 
	printf("AT+QLBSCFG=\"timeout\",10\r\n");
	
	while(1)
	{
		if((task_cnt % 1) == 0)							//2Sִ��һ��
			BL0910_Elect_Proc();
		IWDG_Feed();												//ι��
	}
}

void Pub_Msgdata_current(void)//�������ݵ���������ʾ
{
	char *strx;
	u8 data_len;
	u8 datastr[10];
	u8 send_jason[250];
	Get_GNSSRMA();									//��ȡGPS����
	data_len = Mqttaliyun_Send_Irms(send_jason,temp,humi,Irms_buff,rx_data2);
	sprintf((char *)datastr,"%d",data_len);
	printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n",PubTopic,datastr);//������Ϣ
	delay_ms(100);
	Uart2_SendDATA(send_jason,data_len);
	strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//�����ɹ�   +QMTPUBEX: 0,0,0
	while(strx == NULL)
	{
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//�����ɹ�   +QMTPUBEX: 0,0,0
	}
	EC200S_RECData();//������ 
	//delay_ms(1000);
}

void Pub_Msgdata_Energy(void)//�������ݵ���������ʾ
{
	char *strx;
	u8 data_len;
	u8 datastr[10];
	u8 send_jason[250];
	Get_GNSSRMA();									//��ȡGPS����
	data_len = Mqttaliyun_Send_Energy(send_jason,temp,humi,Energy_buff,rx_data2);
	sprintf((char *)datastr,"%d",data_len);
	printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n",PubTopic,datastr);//������Ϣ
	delay_ms(100);
	Uart2_SendDATA(send_jason,data_len);
	strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//�����ɹ�   +QMTPUBEX: 0,0,0
	while(strx == NULL)
	{
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//�����ɹ�   +QMTPUBEX: 0,0,0
	}
	EC200S_RECData();//������ 
	//delay_ms(1000);
}

void Pub_Msgdata_Freq(void)//�������ݵ���������ʾ
{
	char *strx;
	u8 data_len;
	u8 datastr[11];
	u8 send_jason[250];
	Get_GNSSRMA();									//��ȡGPS����
	data_len = Mqttaliyun_Send_Freq(send_jason,temp,humi,Freq_Electricity_consumption,rx_data2);
	sprintf((char *)datastr,"%d",data_len);
	printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n",PubTopic,datastr);//������Ϣ
	delay_ms(100);
	Uart2_SendDATA(send_jason,data_len);
	strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//�����ɹ�   +QMTPUBEX: 0,0,0
	while(strx == NULL)
	{
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//�����ɹ�   +QMTPUBEX: 0,0,0
	}
	EC200S_RECData();//������ 
	//delay_ms(1000);
}

double Irms_buff[8] = {0.0};
double Energy_buff[9] = {0.0};
double Freq_Electricity_consumption[10] = {0.0};
BL09XX_Status BL0910_Elect_Proc(void)
{
	B32_B8_TypeDef tmp_D;
	//��д�����Ĵ�����ȷ���Ƿ�����쳣��λ���������������³�ʼ��,
	BL0910_SPI_Read(Addr_USR_WRPROT,&BL0910_Data);
	BL0910_Data.B8[3] = 0; 
	if(BL0910_Data.B32 != 0x005555)
		BL0910_Init();
	else
	{	
		//��ȡ������Чֵ
		//ʵ�ʵ���=BL0910_Elect.RMS_I/100 ��
		BL0910_SPI_Read(Addr_1_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;									//ȥ��У���ֽ�;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		Irms_buff[0] = tmp_D.B32;
		BL0910_Elect.RMS_1 = tmp_D.B32;
		Irms_buff[0] = (float)BL0910_Elect.RMS_1 / 100;
		
		BL0910_SPI_Read(Addr_2_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_2 = tmp_D.B32;
		Irms_buff[1] = (float)BL0910_Elect.RMS_2 / 100;
		
		BL0910_SPI_Read(Addr_3_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_3 = tmp_D.B32;
		Irms_buff[2] = (float)BL0910_Elect.RMS_3 / 100;
		
		BL0910_SPI_Read(Addr_4_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_4 = tmp_D.B32;
		Irms_buff[3] = (float)BL0910_Elect.RMS_4 / 100;
		
		BL0910_SPI_Read(Addr_7_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_7 = tmp_D.B32;
		Irms_buff[4] = (float)BL0910_Elect.RMS_7 / 100;
		
		BL0910_SPI_Read(Addr_8_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_8 = tmp_D.B32;
		Irms_buff[5] = (float)BL0910_Elect.RMS_8 / 100;
		
		BL0910_SPI_Read(Addr_9_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_9 = tmp_D.B32;
		Irms_buff[6] = (float)BL0910_Elect.RMS_9 / 100;
		
		BL0910_SPI_Read(Addr_10_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
		BL0910_Elect.RMS_10 = tmp_D.B32;
		Irms_buff[7] = (float)BL0910_Elect.RMS_10 / 100;
		
		//����ѹ��Чֵ
		BL0910_SPI_Read(Addr_V_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = tmp_D.B32 * 100 / Voltage_k;
		BL0910_Elect.RMS_V = tmp_D.B32;
		Energy_buff[0] = (float)BL0910_Elect.RMS_V /100;
		
		Energy_buff[1] = (float)Energy_buff[0] * Irms_buff[0];
		Energy_buff[2] = (float)Energy_buff[0] * Irms_buff[1];
		Energy_buff[3] = (float)Energy_buff[0] * Irms_buff[2];
		Energy_buff[4] = (float)Energy_buff[0] * Irms_buff[3];
		Energy_buff[5] = (float)Energy_buff[0] * Irms_buff[4];
		Energy_buff[6] = (float)Energy_buff[0] * Irms_buff[5];
		Energy_buff[7] = (float)Energy_buff[0] * Irms_buff[6];
		Energy_buff[8] = (float)Energy_buff[0] * Irms_buff[7];


		//���й�����
		//ʵ�ʹ���=BL0910_Elect.WATT/10 ��
		BL0910_SPI_Read(Addr_WATT1, &BL0910_Data);
		tmp_D.B32=BL0910_Data.B32;
		tmp_D.B8[3]=0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32=0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_1 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT2, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_2 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT3, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_3 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT4, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_4 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT7, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_7 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT8, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32 = 0x1000000-tmp_D.B32;
		tmp_D.B32=tmp_D.B32*100/Power_K;
		BL0910_Elect.WATT_8=tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT9, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
				tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_9 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT10, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//�ж��Ƿ��ʷ���������ת��
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_10 = tmp_D.B32;
		//����Ƶ,��Ҫע��Ƶ�ʲ�����ZX��nSAG�ź���أ�ȱʡ��SAGLVL��Ӧ��V_RMS�Ĵ���ֵ0x100000�������ֵС��Ƶ�ʲ���ֹͣ��PEROIOD�Ĵ���ֵ=0x0FFFFF
		//ʵ�ʹ�Ƶ=BL0910_Elect.Period/100
		BL0910_SPI_Read(Addr_PERIOD, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = 10000000 * 100 / tmp_D.B32;
		BL0910_Elect.Period = tmp_D.B32;
		Freq_Electricity_consumption[0] = (float)BL0910_Elect.Period / 100;

		//ʵ���õ���=BL0910_Elect.Energy_X+Eng_CFCnt.CntX/Energy_K �ȵ�
		//���й�������������е����ۻ�����
		BL0910_SPI_Read(Addr_CF1_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt1;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_1 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt1 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF2_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt2;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_2 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt2 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF3_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt3;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_3 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt3 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF4_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3]=0;
		tmp_D.B32 += Eng_CFCnt.Cnt4;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_4 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt4 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF7_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt7;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_7 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt7 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF8_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt8;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_8 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt8 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF9_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt9;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_9 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt9 = Eng_Cal.cnt_remainder;
		
		BL0910_SPI_Read(Addr_CF10_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt10;
		Energy_Cal(tmp_D.B32);
		BL0910_Elect.Energy_10 += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt10 = Eng_Cal.cnt_remainder;
		//�ܵ���	
		BL0910_SPI_Read(Addr_CF_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt_Sum;
		if(tmp_D.B32 >= Energy_Sum_K)
		{
			Eng_Cal.Energy001 = tmp_D.B32 / Energy_Sum_K;		//����1�ȵĵ�����������ʱ�����У�׼���ӵ���Ӧ�ĵ���������
			Eng_Cal.cnt_remainder = tmp_D.B32%Energy_Sum_K;
		}
		else
		{
			Eng_Cal.Energy001 = 0;
			Eng_Cal.cnt_remainder = tmp_D.B32;
		}
		BL0910_Elect.Energy_Sum += Eng_Cal.Energy001;
		Eng_CFCnt.Cnt_Sum = Eng_Cal.cnt_remainder;	
		
		Clear_Buffer();
		printf("AT+CCLK?\r\n");	
		Pub_Msgdata_current();										//�����ݵ������ƣ�����MQTTЭ��
		Pub_Msgdata_Freq();
		Pub_Msgdata_Energy();
		delay_ms(200);
		LEDStatus = !LEDStatus; 
		EC200S_RECData();													//������ 
	}
	return BL09XX_OK;
}

