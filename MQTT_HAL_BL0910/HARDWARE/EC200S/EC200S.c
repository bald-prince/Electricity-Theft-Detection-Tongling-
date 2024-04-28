#include "EC200S.h"
#include "string.h"
#include "usart.h"
#include "wdg.h"
#include "led.h"
#include "cjson.h"

char *strx,*extstrx;
extern char RxBuffer[250];     //接收缓冲,
extern u8 RxCounter;

char GSM_LongLatistr[100];//基站经纬度数据字符串

//GPS模块的经纬度数据值
struct 
{
	char Latitude[10];					//经度原数据
	char longitude[9];					//纬度源数据
	char Latitudess[3];					//整数部分
	char longitudess[2];
	char Latitudedd[7];					//小数点部分
	char longitudedd[7];
	float TrueLatitude;					//转换过数据
	float Truelongitude;				//转换过数据	
	char buffer[200];						//存储转换的经纬度数据
	char data_len[10];	
}LongLatidata;

void Clear_Buffer(void)//清空缓存
{
	u8 i;
	for(i = 0;i < 250;i++)
		RxBuffer[i] = 0;//缓存
	RxCounter = 0;
	IWDG_Feed();//喂狗
}
void reset(void)
{
	PWRKEY = 0;
	delay_ms(2000);
	PWRKEY = 1;
}

void EC200U_Init(void)
{
	u8 step = 0;
	switch(step)
	{
		case 0:
			printf("AT\r\n");
			strx = strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
			if(strx != NULL)
			{
				Clear_Buffer();
				step++;
			}
			else
			{
				reset();
				break;
			}
		case 1:
			printf("ATE0&W\r\n"); //关闭回显
			printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。	
			strx = strstr((const char*)RxBuffer,(const char*)"460");//返460，表明识别到卡了
			if(strx != NULL)
			{
				Clear_Buffer();
				step++;
			}
			else
			{
				reset();
				break;
			}
		case 2:
			printf("AT+CREG?\r\n");//查看是否注册GSM网络
			strx = strstr((const char*)RxBuffer,(const char*)"+CREG: 0,1");//返回正常
			extstrx = strstr((const char*)RxBuffer,(const char*)"+CREG: 0,5");//返回正常，漫游
			if((strx != NULL) && (extstrx != NULL))
			{
				Clear_Buffer();
				step++;
			}
			else
			{
				reset();
				break;
			}
		case 3:
			printf("AT+CGREG?\r\n");//查看是否注册GPRS网络
			strx = strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
			extstrx = strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//返回正常，漫游
			if((strx != NULL) && (extstrx != NULL))
			{
				Clear_Buffer();
				step++;
			}
			else
			{
				reset();
				break;
			}
		case 4:
			printf("AT+COPS?\r\n");//查看注册到哪个运营商，支持移动 联通 电信 
			Clear_Buffer();
			printf("AT+QMTDISC=0\r\n");//关闭socket连接
			Clear_Buffer();
			printf("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n");//接入APN，无用户名和密码
			Clear_Buffer();
			printf("AT+QIACT=1\r\n");//激活
			Clear_Buffer();
			printf("AT+QIACT?\r\n");//获取当前卡的IP地址
			Clear_Buffer();
			printf("AT+QGPS=1\r\n"); //对GNSS开机
			printf("AT+QGPS?\r\n"); //对GNSS开机
			Clear_Buffer();	
	}
}

//AT&F&W  恢复出厂设置
void EC200S_Init(void)
{
	delay_ms(300);
  printf("AT\r\n"); 
  delay_ms(300);
  strx = strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
  Clear_Buffer();	
  while(strx == NULL)
  {
		Clear_Buffer();	
    printf("AT\r\n"); 
    delay_ms(300);
    strx = strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
  }
  printf("ATE0&W\r\n"); //关闭回显
  delay_ms(300);
  printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
  delay_ms(300);
  strx = strstr((const char*)RxBuffer,(const char*)"460");//返460，表明识别到卡了
  Clear_Buffer();	
  while(strx == NULL)
  {
		Clear_Buffer();	
    printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(300);
    strx = strstr((const char*)RxBuffer,(const char*)"460");//返回OK,说明卡是存在的
  }
  printf("AT+CREG?\r\n");//查看是否注册GSM网络
  delay_ms(500);
  strx = strstr((const char*)RxBuffer,(const char*)"+CREG: 0,1");//返回正常
  extstrx = strstr((const char*)RxBuffer,(const char*)"+CREG: 0,5");//返回正常，漫游
  while(strx == NULL && extstrx == NULL)
  {
		Clear_Buffer();
    printf("AT+CREG?\r\n");//查看是否注册GSM网络
    delay_ms(500);
    strx = strstr((const char*)RxBuffer,(const char*)"+CREG: 0,1");//返回正常
    extstrx = strstr((const char*)RxBuffer,(const char*)"+CREG: 0,5");//返回正常，漫游
  }
  Clear_Buffer();
  printf("AT+CGREG?\r\n");//查看是否注册GPRS网络
  delay_ms(500);
  strx = strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
  extstrx = strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//返回正常，漫游
  while(strx == NULL&&extstrx==NULL)
  {
		Clear_Buffer();
    printf("AT+CGREG?\r\n");//查看是否注册GPRS网络
    delay_ms(500);
    strx = strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,1");//，这里重要，只有注册成功，才可以进行GPRS数据传输。
    extstrx = strstr((const char*)RxBuffer,(const char*)"+CGREG: 0,5");//返回正常，漫游
  }
  Clear_Buffer();
  printf("AT+COPS?\r\n");//查看注册到哪个运营商，支持移动 联通 电信 
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QMTDISC=0\r\n");//关闭socket连接
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",0\r\n");//接入APN，无用户名和密码
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QIACT=1\r\n");//激活
  delay_ms(500);
  Clear_Buffer();
  printf("AT+QIACT?\r\n");//获取当前卡的IP地址
  delay_ms(500);
  Clear_Buffer();
	printf("AT+QGPS=1\r\n"); //对GNSS开机
  delay_ms(300);
	printf("AT+QGPS?\r\n"); //对GNSS开机
  delay_ms(300);
	Clear_Buffer();
}

void EC200S_CreateTCPSokcet(void)//创建sokcet
{
	printf("AT+QIOPEN=1,0,\"TCP\",\"47.92.146.210\",10000,0,1\r\n");//这里是需要登陆的IP号码，采用直接吐出模式
  delay_ms(500);
  strx = strstr((const char*)RxBuffer,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
  while(strx == NULL)
  {
		strx = strstr((const char*)RxBuffer,(const char*)"+QIOPEN: 0,0");//检查是否登陆成功
    delay_ms(100);   
  }
  delay_ms(500);
  Clear_Buffer(); 
}

//数据发送函数
void EC200S_TCPSend(uint8_t *data)
{
	printf("AT+QISEND=0\r\n");
  delay_ms(100);
  printf("%s",data);
  delay_ms(100);	
  //USART_SendData(USART2, (u8)0x1a);									//发送完成函数
	HAL_UART_Transmit(&UART2_Handler,(u8 *)0x1a,2,1000);
  while((__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC) == RESET))
	{
	}
	delay_ms(100);
  strx = strstr((char*)RxBuffer,(char*)"SEND OK");//是否正确发送
  while(strx == NULL)
  {
		strx = strstr((char*)RxBuffer,(char*)"SEND OK");//是否正确发送
    delay_ms(10);
  }
  delay_ms(100);
  Clear_Buffer();
}

void MQTT_Init(void)
{
  printf("AT+QMTCFG=\"aliauth\",0,\"%s\",\"%s\",\"%s\"\r\n",ProductKey,DeviceName,DeviceSecret);
  delay_ms(300);
  printf("AT+QMTOPEN=0,\"139.196.135.135\",1883\r\n");//通过TCP方式去连接MQTT阿里云服务器
	//printf("AT+QMTOPEN=0,\"192.168.95.197\",1883\r\n");//通过TCP方式去连接MQTT阿里云服务器	
  delay_ms(300);
  strx = strstr((const char*)RxBuffer,(const char*)"+QMTOPEN: 0,0");//看下返回状态
  while(strx == NULL)
  {
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTOPEN: 0,0");//确认返回值正确
  }
  Clear_Buffer(); 
  printf("AT+QMTCONN=0,\"clientExample_1987\"\r\n");//去登录MQTT服务器
  delay_ms(300);
  strx = strstr((const char*)RxBuffer,(const char*)"+QMTCONN: 0,0,0");//看下返回状态
  while(strx == NULL)
  {
		strx = strstr((const char*)RxBuffer,(const char*)"+QMTCONN: 0,0,0");//看下返回状态
  }
  Clear_Buffer(); 
}

//接收数据
void EC200S_RECData(void)
{
	u8 recdata[250];
  strx = strstr((const char*)RxBuffer,(const char*)"+QMTRECV:");//返回+QMTRECV:，表明接收到onenet平台
  if(strx)
  {
		strx = strstr((const char*)RxBuffer,(const char*)"}\"");  //  有}"出现代表数据接收完成
    if(strx)
    {
			memcpy(recdata, RxBuffer, RxCounter);//接收到的数据保存 
      CParsejson(recdata); 
      Clear_Buffer();    
    }
    memset(recdata, 0, 250);      
   } 
   RxCounter = 0; 
}


//访问阿里云需要提交JSON数据
extern int t_json_len;
u8 Mqttaliyun_Send_Irms(u8 *t_payload,u8 temp,u8 humi,double rx_data[],char rx_data2[])
{
	char json[] = "{\"params\":{\"down_data\":[{\"time\":\"%s\"},{\"Current1-Current8\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]},{\"GeoLocation\":[{\"Latitude\":%d.%06d},{\"Longitude\":%d.%06d}]}]}}";
  u8 json_len;
	char t_json[250];
  sprintf(t_json,json,rx_data2,rx_data[0],rx_data[1],rx_data[2],rx_data[3],rx_data[4],rx_data[5],rx_data[6],rx_data[7]\
	,(unsigned int )LongLatidata.TrueLatitude,(unsigned int)((LongLatidata.TrueLatitude-(unsigned int )LongLatidata.TrueLatitude)*1000000),(unsigned int)	LongLatidata.Truelongitude,
	(unsigned int)((LongLatidata.Truelongitude - (unsigned int )LongLatidata.Truelongitude) * 1000000));

	json_len = strlen(t_json) / sizeof(char);
  memcpy(t_payload, t_json, (int)json_len);

	t_json_len = json_len;
  return json_len;
}

u8 Mqttaliyun_Send_Energy(u8 *t_payload,u8 temp,u8 humi,double buff[],char rx_data2[])
{
	char json[] = "{\"params\":{\"Energy_parameters\":[{\"time\":\"%s\"},{\"parameters\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]}]}}";
  u8 json_len;
	char t_json[250];
  sprintf(t_json,json,rx_data2,buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8]);
	json_len = strlen(t_json) / sizeof(char);
  memcpy(t_payload, t_json, (int)json_len);

	t_json_len = json_len;
  return json_len;
}

u8 Mqttaliyun_Send_Freq(u8 *t_payload,u8 temp,u8 humi,double buff[],char rx_data2[])
{
	char json[] = "{\"params\":{\"Freq_Electricity_consumption\":[{\"time\":\"%s\"},{\"parameters\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]}]}}";
  u8 json_len;
	char t_json[250];
  sprintf(t_json,json,rx_data2,buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7],buff[8],buff[9]);
	json_len = strlen(t_json) / sizeof(char);
  memcpy(t_payload, t_json, (int)json_len);

	t_json_len = json_len;
  return json_len;
}

//GPS获取经纬度信息
void Get_GNSSRMA(void)
{
	printf("AT+QGPSGNMEA=\"RMC\"\r\n"); //获取GPS定位信息
	delay_ms(300);
	strx = strstr((char*)RxBuffer,(char*)"+QGPSGNMEA:");//是否获取到GPS正确发送
  if(strx)
    Getdata_Change();//解析GPS数据
	Clear_Buffer();
}

//获取基站经纬度数据
void Get_GSM_loc(char *data)
{
	printf("AT+QLBS\r\n");
	delay_ms(300);
}

/*************将原始数据解析出经纬度数据*******************/	
void Getdata_Change(void)
{
	unsigned char i;
	char *strx;
	char json[] = "{lon:%d.%06d:lat:%d.%06d}";
	strx = strstr((const char*)RxBuffer,(const char*)"N,");//返回N，表明经纬度数据被正确获取了
	if(strx)
	{
		for(i = 0; i < 9; i++)
			LongLatidata.longitude[i] = strx[i - 10];//纬度获取到3151.0237
		strx = strstr((const char*)RxBuffer,(const char*)"E,");//返回E，读取纬度数据11702.5641
		if(strx)
		{
			for(i = 0; i < 10; i++)
					LongLatidata.Latitude[i] = strx[i - 11];//经度数据获取到	
			for(i = 0;i < 3; i++)
					LongLatidata.Latitudess[i] = LongLatidata.Latitude[i];
			for(i = 3;i < 10; i++)
				LongLatidata.Latitudedd[i - 3] = LongLatidata.Latitude[i];
			LongLatidata.Truelongitude = (LongLatidata.Latitudess[0] - 0x30) * 100 + (LongLatidata.Latitudess[1] - 0x30) * 10 + (LongLatidata.Latitudess[2] - 0x30)\
				+ ((LongLatidata.Latitudedd[0] - 0x30) * 10 + (LongLatidata.Latitudedd[1] - 0x30) + (float)(LongLatidata.Latitudedd[3] - 0x30) / 10 + \
				(float)(LongLatidata.Latitudedd[4] - 0x30) / 100+(float)(LongLatidata.Latitudedd[5] - 0x30) / 1000 + (float)(LongLatidata.Latitudedd[6] - 0x30) / 10000) / 60.0;//获取完整的数据
			for(i = 0; i < 2; i++)
				LongLatidata.longitudess[i] = LongLatidata.longitude[i];
			for(i = 2;i < 9; i++)
				LongLatidata.longitudedd[i - 2] = LongLatidata.longitude[i];	
			LongLatidata.TrueLatitude = (LongLatidata.longitudess[0] - 0x30) * 10 + (LongLatidata.longitudess[1] - 0x30)\
				+((LongLatidata.longitudedd[0]-0x30)*10+(LongLatidata.longitudedd[1]-0x30)+(float)(LongLatidata.longitudedd[3]-0x30)/10+\
				(float)(LongLatidata.longitudedd[4]-0x30)/100+(float)(LongLatidata.longitudedd[5]-0x30)/1000+(float)(LongLatidata.longitudedd[6]-0x30)/10000)/60.0;//获取完整的数据b
			sprintf(LongLatidata.buffer, json, (unsigned int )LongLatidata.Truelongitude,(unsigned int)((LongLatidata.Truelongitude-(unsigned int )LongLatidata.Truelongitude)*1000000)\
				,(unsigned int)LongLatidata.TrueLatitude,(unsigned int)((LongLatidata.TrueLatitude-(unsigned int )LongLatidata.TrueLatitude)*1000000));//浮点型数据转成字符串数据
			}
			else
			{
				Clear_Buffer();
				Get_GSM_loc(GSM_LongLatistr);   //基站定位
				if(GSM_LongLatistr[2] == '.')
			  {
					for(i = 0; i <= 100; i++)
					{
						LongLatidata.Truelongitude = 100 * (GSM_LongLatistr[10] - '0') + 10 * (GSM_LongLatistr[11] - '0') + (GSM_LongLatistr[12] - '0') + 0.1 * (GSM_LongLatistr[14]-'0')+0.01*(GSM_LongLatistr[15]-'0')+0.001*(GSM_LongLatistr[16]-'0')+0.0001*(GSM_LongLatistr[17]-'0')\
						+0.00001*(GSM_LongLatistr[18]-'0')+0.000001*(GSM_LongLatistr[19]-'0');
						LongLatidata.TrueLatitude=10*(GSM_LongLatistr[0]-'0')+(GSM_LongLatistr[1]-'0')+0.1*(GSM_LongLatistr[3]-'0')+0.01*(GSM_LongLatistr[4]-'0')+0.001*(GSM_LongLatistr[5]-'0')+0.0001*(GSM_LongLatistr[6]-'0')+0.00001*(GSM_LongLatistr[7]-'0')\
						+0.000001*(GSM_LongLatistr[8]-'0');
						sprintf(LongLatidata.buffer, json, (unsigned int )LongLatidata.Truelongitude,(unsigned int)((LongLatidata.Truelongitude-(unsigned int )LongLatidata.Truelongitude)*1000000)\
						,(unsigned int)LongLatidata.TrueLatitude,(unsigned int)((LongLatidata.TrueLatitude-(unsigned int )LongLatidata.TrueLatitude)*1000000));//浮点型数据转成字符串数据
					}
        }
				else
				{
					LongLatidata.Truelongitude = 0;
				  LongLatidata.TrueLatitude = 0;
				}
			}
   }
}

void CParsejson(void *data)
{
	u8 i = 0,*jsonstrx;
	u8 jsondata[250];
	u8 status;
  cJSON * root, *itemName,*order;
     //从缓冲区中解析json结构
  jsonstrx = strstr((const char*)data,(const char*)"{"); //判断到有"{出现,代表json产生 
  if(jsonstrx)
  {
		while(1)
    {
			if(jsonstrx[i] == 0x0D)//找到结束符，就退出  
      {   
				jsondata[i - 1] = 0;
        i = 0;
        break;
      }
			jsondata[i] = jsonstrx[i];
      i++;
     }   
  }
  root = cJSON_Parse(jsondata);   
  if(jsondata == NULL)
		return;
  order = cJSON_GetObjectItem(root, "params");
	if(order == NULL)
		return;
  itemName = cJSON_GetObjectItem(order, "BeepSwitch");
  if(itemName)
  {
		status = itemName->valueint;
    if(status)
			BEEP = 1;
    else
			BEEP = 0;
  }
	itemName = cJSON_GetObjectItem(order, "NO");
	if(itemName)
  {
		status = itemName->valueint;
    if(status)
      SWTCTR = 0;
    else
      SWTCTR = 1;
  }
  //将json结构占用的数据空间释放
  memset(jsondata, 0, 250);
  cJSON_Delete(root);  
}
