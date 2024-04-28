#ifndef __BC20_H
#define __BC20_H
#include "usart.h"
#include <stm32f10x.h>
#include "gpio.h"

//////////////////////////////下面是自行修改参数的地方////////////////////////////////////////
#define ServerIP 		"101.132.164.133"					//emqx服务器ip
#define Port 				"1883"										//端口，mqtt为1883，不需要改动
#define ClientID 		"bc20_1"    							//自定义，只需要和emqx的不同！！！
#define Username   	"admin"										//emqx服务器登录用户名
#define Password   	"smart_luggage"  					//emqx服务器登录密码
#define Topic   		"sys/bc20/subtopic" 			//订阅话题
#define TopicPost   "sys/bc20/pubtopic" 			//发布话题


void Clear_Buffer(void);//清空缓存	
int BC20_Init(void);
void BC20_INITGNSS(void);//启动GPS
void BC20_PDPACT(void);
void BC20_RegONENETIOT(void);
void CSTX_4G_ONENETIOTSenddata(u8 temp,u8 humi);
void CSTX_4G_ONENETIOTSenddataGPS(char *latstr,char *lonstr);
char *Get_GPS_RMC(char type);
u8 MakeOnenetPayloadPowerData(u8* Str,double *power_data,char* time,char *latstr,char * lonstr);
u8 MakeOnenetPayloadGPS(u8* Str,char *latstr,char * lonstr,char* time);
u8 MakeOnenetPayload(u8 *Str,u8 Temp,u8 Humi,char* time);
void CSTX_4G_ONENETIOTSenddataPowerData(double *power_data,char *latstr,char *lonstr);
void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
void BC20_EmqxInit(void);
void NBIOT_RESET(void);
void Control_Led(void);

typedef struct
{
   uint8_t CSQ;    
   uint8_t Socketnum;   //编号
   uint8_t reclen[10];   //获取到数据的长度
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
} BC20;

typedef struct 
{
	char UtcDate[6];
	char longitude[11];//经度原数据
	char Latitude[10];//纬度源数据
	char longitudess[4];//整数部分
	char Latitudess[3];
	char longitudedd[8];//小数点部分
	char Latitudedd[8];
	char Truelongitude[12];//转换过数据
	char TrueLatitude[11];//转换过数据
	char getstautus;//获取到定位的标志状态	
	float gpsdata[2];
}LongLatidata;






#endif
