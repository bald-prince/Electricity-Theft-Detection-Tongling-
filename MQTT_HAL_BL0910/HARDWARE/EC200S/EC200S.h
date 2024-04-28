#ifndef __M5311_H
#define __M5311_H	
#include "usart.h"
#include <stm32f10x.h>
#include "delay.h"
void Clear_Buffer(void);//清空缓存	
void EC200S_Init(void);
void EC200S_CreateTCPSokcet(void);
void EC200S_TCPSend(uint8_t *data);
void EC200S_RECData(void);
void Clear_Buffer(void);
void MQTT_Init(void);
u8 Mqttaliyun_Send_Irms(u8 *t_payload,u8 temp,u8 humi,double rx_data[],char rx_data2[]);
u8 Mqttaliyun_Send_Energy(u8 *t_payload,u8 temp,u8 humi,double buff[],char rx_data2[]);
u8 Mqttaliyun_Send_Freq(u8 *t_payload,u8 temp,u8 humi,double buff[],char rx_data2[]);
void Getdata_Change(void);
void Get_GNSSRMA(void);
void Get_GSM_loc(char*data);
void CParsejson(void *data);
void EC200U_Init(void);
void reset(void);
////阿里云的三元素,在设备里面都可以查询到//////////////////////

/*设备1*/
/*#define ProductKey     			"k0hztO8QbfI"             //产品KEY
#define DeviceName        	"qie_dian1"      
#define DeviceSecret    		"c5c4271b627bbf689e1afdef34d3e7b7"  
#define PubTopic        	 	"/sys/k0hztO8QbfI/qie_dian1/thing/event/property/post"
#define SubTopic        		"/sys/k0hztO8QbfI/qie_dian1/thing/service/property/set"*/

/*设备2*/
/*#define ProductKey     			"k0hztO8QbfI"             //产品KEY
#define DeviceName        	"qie_dian2"      
#define DeviceSecret    		"effbfcb438354280f4a3528fe527903f"  
#define PubTopic        	 	"/sys/k0hztO8QbfI/qie_dian2/thing/event/property/post"
#define SubTopic        		"/sys/k0hztO8QbfI/qie_dian2/thing/service/property/set"*/

/*设备3*/
/*#define ProductKey     			"k0hztO8QbfI"             //产品KEY
#define DeviceName        	"qie_dian3"      
#define DeviceSecret    		"fcafde1e2fc3428dc8075670e9ad4d7d"  
#define PubTopic        	 	"/sys/k0hztO8QbfI/qie_dian3/thing/event/property/post"
#define SubTopic        		"/sys/k0hztO8QbfI/qie_dian3/thing/service/property/set"*/

/*设备4*/
/*#define ProductKey     			"k0hztO8QbfI"             //产品KEY
#define DeviceName        	"qie_dian4"      
#define DeviceSecret    		"445044abf37e81e5b445e84651308b43"  
#define PubTopic        	 	"/sys/k0hztO8QbfI/qie_dian4/thing/event/property/post"
#define SubTopic        		"/sys/k0hztO8QbfI/qie_dian4/thing/service/property/set"*/

/*设备5*/
//#define ProductKey     			"k0hztO8QbfI"             //产品KEY
//#define DeviceName        	"qie_dian5"      
//#define DeviceSecret    		"8f097d4c3daaa443bd930f621cd95492"  
//#define PubTopic        	 	"/sys/k0hztO8QbfI/qie_dian5/thing/event/property/post"
//#define SubTopic        		"/sys/k0hztO8QbfI/qie_dian5/thing/service/property/set"

/*设备5*/
#define ProductKey     			"k0hztO8QbfI"             //产品KEY
#define DeviceName        	"qie_dian6"      
#define DeviceSecret    		"945850b3ef1da9ea7df09f1574b8b6e6"  
#define PubTopic        	 	"/sys/k0hztO8QbfI/qie_dian6/thing/event/property/post"
#define SubTopic        		"/sys/k0hztO8QbfI/qie_dian6/thing/service/property/set"



#endif





