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
  HAL_Init();                    												//初始化HAL库    
  Stm32_Clock_Init(RCC_PLL_MUL9);	 											//设置时钟,72M
	delay_init(72);               												//初始化延时函数
	LED_Init();																						//初始化LED	
	EC200SPWR_Init();																			//对EC200S开机
	uart_init(115200);																		//串口1初始化，打印到电脑调试助手
  uart2_init(115200);																		//对接EC200S
	IWDG_Init(7,625);
	PWRKEY = 1;
	EC200S_Init();																				//对设备初始化
	//EC200U_Init();
  MQTT_Init();																					//初始化MQTT
	SPI1_Init();	
	BL0910_Init();
	TIM3_Init(10000-1,7200-1);
	
	//设置获取基站经纬度数据
	printf("AT+QLBSCFG=\"token\",\"7313Z68310k1100W\"\r\n"); 
	//printf("AT+QLBSCFG=\"token\",\"083uwqYo3OJ00NP0\"\r\n"); 
	printf("AT+QLBSCFG=\"timeout\",10\r\n");
	
	while(1)
	{
		if((task_cnt % 1) == 0)							//2S执行一次
			BL0910_Elect_Proc();
		IWDG_Feed();												//喂狗
	}
}

void Pub_Msgdata_current(void)//发布数据到阿里云显示
{
	char *strx;
	u8 data_len;
	u8 datastr[10];
	u8 send_jason[250];
	Get_GNSSRMA();									//获取GPS数据
	data_len = Mqttaliyun_Send_Irms(send_jason,temp,humi,Irms_buff,rx_data2);
	sprintf((char *)datastr,"%d",data_len);
	printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n",PubTopic,datastr);//发布消息
	delay_ms(100);
	Uart2_SendDATA(send_jason,data_len);
	strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//发布成功   +QMTPUBEX: 0,0,0
	while(strx == NULL)
	{
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//发布成功   +QMTPUBEX: 0,0,0
	}
	EC200S_RECData();//收数据 
	//delay_ms(1000);
}

void Pub_Msgdata_Energy(void)//发布数据到阿里云显示
{
	char *strx;
	u8 data_len;
	u8 datastr[10];
	u8 send_jason[250];
	Get_GNSSRMA();									//获取GPS数据
	data_len = Mqttaliyun_Send_Energy(send_jason,temp,humi,Energy_buff,rx_data2);
	sprintf((char *)datastr,"%d",data_len);
	printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n",PubTopic,datastr);//发布消息
	delay_ms(100);
	Uart2_SendDATA(send_jason,data_len);
	strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//发布成功   +QMTPUBEX: 0,0,0
	while(strx == NULL)
	{
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//发布成功   +QMTPUBEX: 0,0,0
	}
	EC200S_RECData();//收数据 
	//delay_ms(1000);
}

void Pub_Msgdata_Freq(void)//发布数据到阿里云显示
{
	char *strx;
	u8 data_len;
	u8 datastr[11];
	u8 send_jason[250];
	Get_GNSSRMA();									//获取GPS数据
	data_len = Mqttaliyun_Send_Freq(send_jason,temp,humi,Freq_Electricity_consumption,rx_data2);
	sprintf((char *)datastr,"%d",data_len);
	printf("AT+QMTPUBEX=0,0,0,0,\"%s\",%s\r\n",PubTopic,datastr);//发布消息
	delay_ms(100);
	Uart2_SendDATA(send_jason,data_len);
	strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//发布成功   +QMTPUBEX: 0,0,0
	while(strx == NULL)
	{
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTPUBEX: 0,0,0");//发布成功   +QMTPUBEX: 0,0,0
	}
	EC200S_RECData();//收数据 
	//delay_ms(1000);
}

double Irms_buff[8] = {0.0};
double Energy_buff[9] = {0.0};
double Freq_Electricity_consumption[10] = {0.0};
BL09XX_Status BL0910_Elect_Proc(void)
{
	B32_B8_TypeDef tmp_D;
	//读写保护寄存器，确认是否出现异常复位情况。如果有则重新初始化,
	BL0910_SPI_Read(Addr_USR_WRPROT,&BL0910_Data);
	BL0910_Data.B8[3] = 0; 
	if(BL0910_Data.B32 != 0x005555)
		BL0910_Init();
	else
	{	
		//读取电流有效值
		//实际电流=BL0910_Elect.RMS_I/100 安
		BL0910_SPI_Read(Addr_1_RMS, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;									//去除校验字节;
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
		
		//读电压有效值
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


		//读有功功率
		//实际功率=BL0910_Elect.WATT/10 瓦
		BL0910_SPI_Read(Addr_WATT1, &BL0910_Data);
		tmp_D.B32=BL0910_Data.B32;
		tmp_D.B8[3]=0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32=0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_1 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT2, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_2 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT3, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_3 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT4, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_4 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT7, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_7 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT8, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32 = 0x1000000-tmp_D.B32;
		tmp_D.B32=tmp_D.B32*100/Power_K;
		BL0910_Elect.WATT_8=tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT9, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
				tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_9 = tmp_D.B32;
		
		BL0910_SPI_Read(Addr_WATT10, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		if (tmp_D.B32 >= 8388607)						//判断是否功率反向，做补码转换
			tmp_D.B32 = 0x1000000 - tmp_D.B32;
		tmp_D.B32 = tmp_D.B32 * 100 / Power_K;
		BL0910_Elect.WATT_10 = tmp_D.B32;
		//读工频,需要注意频率测量与ZX，nSAG信号相关，缺省的SAGLVL对应的V_RMS寄存器值0x100000，比这个值小，频率测试停止，PEROIOD寄存器值=0x0FFFFF
		//实际工频=BL0910_Elect.Period/100
		BL0910_SPI_Read(Addr_PERIOD, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 = 10000000 * 100 / tmp_D.B32;
		BL0910_Elect.Period = tmp_D.B32;
		Freq_Electricity_consumption[0] = (float)BL0910_Elect.Period / 100;

		//实际用电量=BL0910_Elect.Energy_X+Eng_CFCnt.CntX/Energy_K 度电
		//读有功脉冲计数，进行电能累积计算
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
		//总电量	
		BL0910_SPI_Read(Addr_CF_CNT, &BL0910_Data);
		tmp_D.B32 = BL0910_Data.B32;
		tmp_D.B8[3] = 0;
		tmp_D.B32 += Eng_CFCnt.Cnt_Sum;
		if(tmp_D.B32 >= Energy_Sum_K)
		{
			Eng_Cal.Energy001 = tmp_D.B32 / Energy_Sum_K;		//超过1度的电量保存在临时变量中，准备加到对应的电量变量中
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
		Pub_Msgdata_current();										//发数据到阿里云，采用MQTT协议
		Pub_Msgdata_Freq();
		Pub_Msgdata_Energy();
		delay_ms(200);
		LEDStatus = !LEDStatus; 
		EC200S_RECData();													//收数据 
	}
	return BL09XX_OK;
}

