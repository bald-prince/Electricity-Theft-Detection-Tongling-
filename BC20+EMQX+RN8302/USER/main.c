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
	char *gpsStr; 							//GPS指向位置
	char sendMutiData[BUFLEN]; 	//发送数据到服务
	char gpsDatalat[64];
	char gpsDatalon[64];
	
	delay_init();	    	 //延时函数初始化	
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	BC20_GpioInit();
	GPIO_INIT();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
  uart1_init(115200);
  uart2_init(115200);
  //uart3_init(115200);
  NBIOT_RESET();
	OLED_Clear();
	OLED_ShowString(0,0,"STM32+BC20+WIFI",16,1);
	OLED_Refresh();
	Uart1_SendStr("init stm32L COM1 \r\n"); //打印信息
	rn8302_SpiConfig();
	rn8302_EmuInit();
	while(BC20_Init());
  BC20_PDPACT();
	BC20_EmqxInit();
	BC20_INITGNSS();
  TIM3_Int_Init(4999,7199);//10khz计数频率，计数到5000为500ms
	OLED_ShowString(0,16,"BC20 Ready",16,1);
	OLED_Refresh();
	
	while(1)
	{ 
		rn8302_ReadData();
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		memset(tempstr,0,BUFLEN);
		memset(senddata,0,BUFLEN);
		gpsStr = NULL;
		memset(sendMutiData,0,BUFLEN);//发送多传感器数据到服务器
		memset(gpsDatalat,0,64);//维度
		memset(gpsDatalon,0,64);//经度
		strcat(sendMutiData,"latitude=");//传递维度
		gpsStr = Get_GPS_RMC(1);//获取维度 经过纠偏之后的
							
		if(gpsStr)	//如果读取到了
		{
			strcat(sendMutiData,gpsStr);//传递维度
			strcat(gpsDatalat,gpsStr);//传递维度
		}
		else	//还没定位好
		{
			strcat(sendMutiData,"39.897445");//传递维度
			strcat(gpsDatalat,"39.897445");//传递维度
		}
							
		strcat(sendMutiData,"&longitude="); //传递经度
		gpsStr = Get_GPS_RMC(2);//获取经度 经过纠偏之后的
		if(gpsStr)	//如果读取到了
		{
			strcat(sendMutiData,gpsStr); //传递经度
			strcat(gpsDatalon,gpsStr); //传递经度
		}
		else	//还没定位好
		{
			strcat(sendMutiData,"116.331398");//传递经度
			strcat(gpsDatalon,"116.331398");//传递经度
		}
		printf("sendMutiData=%s \r\n",sendMutiData);
		CSTX_4G_ONENETIOTSenddataPowerData(Power_Data,gpsDatalat,gpsDatalon);
		//CSTX_4G_ONENETIOTSenddataGPS(gpsDatalat,gpsDatalon);
		delay_ms(100);
	}
}


