#ifndef __BC20_H
#define __BC20_H
#include "usart.h"
#include <stm32f10x.h>
#include "gpio.h"

//////////////////////////////�����������޸Ĳ����ĵط�////////////////////////////////////////
#define ServerIP 		"101.132.164.133"					//emqx������ip
#define Port 				"1883"										//�˿ڣ�mqttΪ1883������Ҫ�Ķ�
#define ClientID 		"bc20_1"    							//�Զ��壬ֻ��Ҫ��emqx�Ĳ�ͬ������
#define Username   	"admin"										//emqx��������¼�û���
#define Password   	"smart_luggage"  					//emqx��������¼����
#define Topic   		"sys/bc20/subtopic" 			//���Ļ���
#define TopicPost   "sys/bc20/pubtopic" 			//��������


void Clear_Buffer(void);//��ջ���	
int BC20_Init(void);
void BC20_INITGNSS(void);//����GPS
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
   uint8_t Socketnum;   //���
   uint8_t reclen[10];   //��ȡ�����ݵĳ���
   uint8_t res;      
   uint8_t recdatalen[10];
   uint8_t recdata[100];
} BC20;

typedef struct 
{
	char UtcDate[6];
	char longitude[11];//����ԭ����
	char Latitude[10];//γ��Դ����
	char longitudess[4];//��������
	char Latitudess[3];
	char longitudedd[8];//С���㲿��
	char Latitudedd[8];
	char Truelongitude[12];//ת��������
	char TrueLatitude[11];//ת��������
	char getstautus;//��ȡ����λ�ı�־״̬	
	float gpsdata[2];
}LongLatidata;






#endif
