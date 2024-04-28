#include "sys.h"
#include "delay.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "HEXSTR.h"
#include "bc20.h"
#include "timer.h"
#include "gpio.h"
#include "rn8302.h"
#include "oled.h"
#include "bmp.h"

extern int err;
extern int val;

int main(void)
{
	char senddata[BUFLEN];
  char tempstr[BUFLEN];
	char *gpsStr; 							//GPSָ��λ��
	char sendMutiData[BUFLEN]; 	//�������ݵ�����
	char gpsDatalat[64];
	char gpsDatalon[64];
	
	delay_init();	    	 //��ʱ������ʼ��	
	OLED_Init();
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	BC20_GpioInit();
	GPIO_INIT();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  uart1_init(115200);
  uart2_init(115200);
  //uart3_init(115200);
  NBIOT_RESET();
	OLED_Clear();
	OLED_ShowString(0,0,"STM32+BC20+WIFI",16,1);
	OLED_Refresh();
	Uart1_SendStr("init stm32L COM1 \r\n"); //��ӡ��Ϣ
	rn8302_SpiConfig();
	rn8302_EmuInit();
	while(BC20_Init());
  BC20_PDPACT();
	BC20_EmqxInit();
	BC20_INITGNSS();
  TIM3_Int_Init(4999,7199);//10khz����Ƶ�ʣ�������5000Ϊ500ms
	OLED_ShowString(0,16,"BC20 Ready",16,1);
	OLED_Refresh();
	
	while(1)
	{ 
		rn8302_ReadData();
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		memset(tempstr,0,BUFLEN);
		memset(senddata,0,BUFLEN);
		gpsStr = NULL;
		memset(sendMutiData,0,BUFLEN);//���Ͷഫ�������ݵ�������
		memset(gpsDatalat,0,64);//ά��
		memset(gpsDatalon,0,64);//����
		strcat(sendMutiData,"latitude=");//����ά��
		gpsStr = Get_GPS_RMC(1);//��ȡά�� ������ƫ֮���
							
		if(gpsStr)	//�����ȡ����
		{
			strcat(sendMutiData,gpsStr);//����ά��
			strcat(gpsDatalat,gpsStr);//����ά��
		}
		else	//��û��λ��
		{
			strcat(sendMutiData,"39.897445");//����ά��
			strcat(gpsDatalat,"39.897445");//����ά��
		}
							
		strcat(sendMutiData,"&longitude="); //���ݾ���
		gpsStr = Get_GPS_RMC(2);//��ȡ���� ������ƫ֮���
		if(gpsStr)	//�����ȡ����
		{
			strcat(sendMutiData,gpsStr); //���ݾ���
			strcat(gpsDatalon,gpsStr); //���ݾ���
		}
		else	//��û��λ��
		{
			strcat(sendMutiData,"116.331398");//���ݾ���
			strcat(gpsDatalon,"116.331398");//���ݾ���
		}
		printf("sendMutiData=%s \r\n",sendMutiData);
		CSTX_4G_ONENETIOTSenddataPowerData(Power_Data,gpsDatalat,gpsDatalon);
		//CSTX_4G_ONENETIOTSenddataGPS(gpsDatalat,gpsDatalon);
		delay_ms(100);
	}
}


