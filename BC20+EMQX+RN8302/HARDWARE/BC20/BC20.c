///AT+NSOCL=0
#include "BC20.h"
//#include "main.h"
#include "string.h"
#include "oled.h"
char *strx,*extstrx;
char atstr[BUFLEN];
int err;    //全局变量
char atbuf[BUFLEN];
char objtnum[BUFLEN];//观察号
char distnum[BUFLEN];//观察号
BC20 BC20_Status;
int  errcount = 0;	//发送命令失败次数 防止死循环
char ATSTR[BUFLEN];	//组建AT命令的函数
char time[BUFLEN];

//////////////////////////////////////////////////////////////////////////////////////////////////
char GPRMCSTR[128]; //转载GPS信息 GPRMC 经纬度存储的字符串
char GPRMCSTRLON[64]; //经度存储字符串 也就是119.20694
char GPRMCSTRLAT[64]; //维度存储字符串，也就是26.06451
char IMEINUMBER[64];//+CGSN: "869523052178994"
//////////////////下面是纠正火星坐标的变量定义/////////////////////////
int Get_GPSdata(void);
void Getdata_Change(char status);


LongLatidata latdata;
float tempdata[2];
char latStrAF[64];          //存放数据经纬度用来发送
char lonStrAF[64];   //存放数据经纬度用来显示
//////////////////////////火星纠偏结束///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void Clear_Buffer(void)//清空串口2缓存
{
	printf(buf_uart2.buf);  //清空前打印信息
  Delay(300);
  buf_uart2.index=0;
  memset(buf_uart2.buf,0,BUFLEN);
}

int BC20_Init(void)
{
	int errcount = 0;
  err = 0;    //判断模块卡是否就绪最重要
  printf("start init BC20\r\n");
  Uart2_SendStr("AT\r\n");
  Delay(300);
  printf(buf_uart2.buf);      //打印收到的串口信息
  printf("get back BC20\r\n");
  strx = strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
  Clear_Buffer();	
  while(strx == NULL)
  {
		printf("\r\n单片机正在连接到模块...\r\n");
    Clear_Buffer();	
    Uart2_SendStr("AT\r\n");
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
	}
    
  Uart2_SendStr("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"ERROR");//只要卡不错误 基本就成功
  if(strx == NULL)
  {
    printf("我的卡号是 : %s \r\n",&buf_uart2.buf[8]);
    Clear_Buffer();	
    Delay(300);
  }
  else
  {
    err = 1;
    printf("卡错误 : %s \r\n",buf_uart2.buf);
    Clear_Buffer();
    Delay(300);
  }
  Uart2_SendStr("ATI\r\n");//激活网络，PDP
  Delay(300);
  Clear_Buffer();	

  Uart2_SendStr("AT+CGATT?\r\n");//查询激活状态
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//返1 表明激活成功 获取到IP地址了
  Clear_Buffer();	
  errcount = 0;
  while(strx == NULL)
  {
    errcount++;
    Clear_Buffer();	
    Uart2_SendStr("AT+CGATT?\r\n");//获取激活状态
    Delay(300);
    strx = strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//返回1,表明注网成功
    if(errcount > 100)     //防止死循环
    {
			err = 1;
      errcount = 0;
      break;
    }
  }
  Uart2_SendStr("AT+QBAND?\r\n"); //允许错误值
  Delay(300);
  strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK
  if(strx)
  {
		printf("========BAND========= \r\n %s \r\n",buf_uart2.buf);
    Clear_Buffer();
    Delay(300);
  }
	Uart2_SendStr("AT+CSQ\r\n");//查看获取CSQ值
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+CSQ");//返回CSQ
  if(strx)
  {
    printf("信号质量:%s\r\n",buf_uart2.buf);
    Clear_Buffer();
    Delay(300);
	}
  Uart2_SendStr("AT+QCCID\r\n");
  Delay(300);
  Clear_Buffer();	
  return err;
}

void BC20_PDPACT(void)//激活场景，为连接服务器做准备
{
	int errcount = 0;
  Uart2_SendStr("AT+CGPADDR=1\r\n");//激活场景
  Delay(300);
  Clear_Buffer();
  Uart2_SendStr("AT+CGSN=1\r\n");//激活场景
  Delay(300);
  Clear_Buffer();
  Uart2_SendStr("AT+CGATT?\r\n");//激活场景
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//注册上网络信息
  Clear_Buffer();	
  while(strx == NULL)
  {
		errcount++;
    Clear_Buffer();
    Uart2_SendStr("AT+CGATT?\r\n");//激活场景
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//一定要终端入网
    if(errcount>100)     //防止死循环
    {
			errcount = 0;
      break;
    }
  }
  Delay(300);
  Clear_Buffer();	
  Delay(300);
}


void BC20_INITGNSS(void)//启动GPS
{
  int errcount = 0;
  Uart2_SendStr("AT+QGNSSC=1\r\n");//激活GPS 要等待很久启动GNSS
  Delay(1000);
	Delay(1000);
	Delay(1000);
  Clear_Buffer();
  Uart2_SendStr("AT+QGNSSC?\r\n");//查询GPS激活情况
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QGNSSC: 1");//启动成功
  Clear_Buffer();
  while(strx == NULL)
  {
    errcount++;
    Clear_Buffer();
    Uart2_SendStr("AT+QGNSSC?\r\n");//查询
    Delay(500);
    strx = strstr((const char*)buf_uart2.buf,(const char*)"+QGNSSC: 1");//启动成功
    if(errcount>100)     //防止死循环
    {
      errcount = 0;
      break;
    }
  }
  Clear_Buffer();
  Uart2_SendStr("AT+QGNSSRD=\"NMEA/RMC\"\r\n");
  Delay(300);
  Clear_Buffer();
}


/*********************************************************************************
AT+QGNSSRD="NMEA/RMC"
+QGNSSRD: $GNRMC,,V,,,,,,,,,,N,V*37
OK
***********************************************************************************/

char *Get_GPS_RMC(char type)
{
	Clear_Buffer();	
	memset(GPRMCSTR,0,128);
	Uart2_SendStr("AT+QGNSSRD=\"NMEA/RMC\"\r\n");//查询激活状态
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"$GNRMC");//返1
		
	while(strx == NULL)
	{
		Clear_Buffer();	
		Uart2_SendStr("AT+QGNSSRD=\"NMEA/RMC\"\r\n");//获取激活状态
		delay_ms(300);
		strx = strstr((const char*)buf_uart2.buf,(const char*)"$GNRMC");//返回1,表明注网成功
	}
	sprintf(GPRMCSTR,"%s",strx);
	
	Clear_Buffer();	//打印收到的GPS信息
	GPRMCSTR[2]=	'P';
		
	printf("============GETGPRMC==============\r\n%s",GPRMCSTR);		//打印GPRMC
	if(GPRMCSTR[17]=='A')
	{
		memset(latStrAF,0,64);
		memset(lonStrAF,0,64);
		Get_GPSdata();
		if(type == 1)
			return latStrAF;
		if(type == 2)
			return lonStrAF;
	}
	return 0;
}



/*****************************************************
下面是矫正火星坐标的
AT+QGNSSRD="NMEA/RMC"
+QGNSSRD: $GNRMC,091900.00,A,2603.9680,N,11912.4174,E,0.189,,201022,,,A,V*1A
OK
*****************************************************/
//解GPS析函数
// $GNRMC,091900.00,A,2603.9680,N,11912.4174,E,0.189,,201022,,,A,V*1A
int Get_GPSdata()
{
	int i = 0;
  strx = strstr((const char*)GPRMCSTR,(const char*)"A,");//获取纬度的位置
  if(strx)
  {
		for(i = 0; i < 9; i++)
			latdata.Latitude[i] = strx[i + 2];//获取纬度值2603.9576   
		strx = strstr((const char*)GPRMCSTR,(const char*)"N,");//获取经度值
		if(strx)
		{
			for(i = 0; i < 10; i++)	//获取经度 11912.4098
				latdata.longitude[i] = strx[i + 2];		 
		}  
		printf("latdata.Latitude ,%s \r\n",latdata.Latitude);
		printf("latdata.longitude ,%s \r\n",latdata.longitude);
    latdata.getstautus = 1;//                 
	}                        
	else			
		latdata.getstautus = 0;
	Getdata_Change(latdata.getstautus);//数据换算
	Clear_Buffer();
	return 0;
}

/*************解析出经纬度数据,然后直接提交数据*******************/	

void Getdata_Change(char status)
{
	unsigned char i;	
  if(status)
  {
		for(i = 0; i < 3; i++)
			latdata.longitudess[i]=latdata.longitude[i];
		for(i = 3; i < 10; i++)
			latdata.longitudedd[i - 3] = latdata.longitude[i];
		latdata.gpsdata[0] = (latdata.longitudess[0] - 0x30) * 100 + (latdata.longitudess[1] - 0x30) * 10 + (latdata.longitudess[2] - 0x30)\
		    + ((latdata.longitudedd[0] - 0x30) * 10 + (latdata.longitudedd[1] - 0x30) + (float)(latdata.longitudedd[3] - 0x30) / 10\
		    + (float)(latdata.longitudedd[4] - 0x30) / 100 + (float)(latdata.longitudedd[5] - 0x30) / 1000 \
				+ (float)(latdata.longitudedd[6] - 0x30) / 10000) / 60.0;//获取完整的数据
       
		for(i = 0; i < 2; i++)
			latdata.Latitudess[i] = latdata.Latitude[i];
		for(i=2;i<9;i++)
			latdata.Latitudedd[i - 2] = latdata.Latitude[i];	
				 
		latdata.gpsdata[1] = (float)(latdata.Latitudess[0] - 0x30) * 10 + (latdata.Latitudess[1] - 0x30)\
					+((latdata.Latitudedd[0]-0x30)*10+(latdata.Latitudedd[1]-0x30)+(float)(latdata.Latitudedd[3]-0x30)/10\
					+(float)(latdata.Latitudedd[4] - 0x30) / 100 + (float)(latdata.Latitudedd[5] - 0x30) / 1000\
					+(float)(latdata.Latitudedd[6] - 0x30) / 10000) / 60.0;//获取完整的数据b

		sprintf(latStrAF,"%f",latdata.gpsdata[1]);
		sprintf(lonStrAF,"%f",latdata.gpsdata[0]);
				 	 			 
		printf("latStrAF,%s \r\n",latStrAF);
		printf("lonStrAF,%s \r\n",lonStrAF);		 
	}
  else
  {
		latdata.gpsdata[0] = 0;
    latdata.gpsdata[1] = 0;
  }
}

void BC20_EmqxInit(void)
{
	memset(ATSTR,0,BUFLEN);//"AT+QMTOPEN=0,\"%s\",%s\r\n",ServerIP,Port
	sprintf(ATSTR,"AT+QMTOPEN=0,\"%s\",%s\r\n",ServerIP,Port); 
	Uart2_SendStr(ATSTR);//选定客户端链路
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTOPEN: 0,0");//返OK
	errcount = 0;	
	while(strx == NULL)
	{
		errcount++;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTOPEN: 0,0");//返回OK,连接阿里云成功
		delay_ms(100);
		if(errcount > 100)     //超时退出死循环 表示服务器连接失败
		{
			errcount = 0;
			break;
		}
	}
	Clear_Buffer();
	
	memset(ATSTR,0,BUFLEN);
	sprintf(ATSTR,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",ClientID,Username,Password); //+QMTCONN: 0,0,0
	Uart2_SendStr(ATSTR);//选定客户端链路
	delay_ms(1000);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//返OK
	errcount = 0;	
	while(strx == NULL)
	{
		errcount++;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//返回OK,连接阿里云成功
		delay_ms(100);
		if(errcount > 100)     //超时退出死循环 表示服务器连接失败
		{
			errcount = 0;
			break;
		}
	}
	Clear_Buffer();
	
	Uart2_SendStr("AT+CCLK=?\r\n");//查询激活状态
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"OK");//返OK
	errcount = 0;	
	while(strx==NULL)
	{
		errcount++;
		strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//返回OK,连接阿里云成功
		delay_ms(100);
		if(errcount > 100)     //超时退出死循环 表示服务器连接失败
		{
			errcount = 0;
			break;
		}
	}
	Clear_Buffer();
	
	memset(ATSTR,0,BUFLEN);
	Clear_Buffer();	//发送命令之前清空之前的模块反馈的数据
  sprintf(ATSTR,"AT+QMTSUB=0,1,\"%s\",0\r\n",Topic);	//发送ONENET命令
  printf("atstr = %s \r\n",ATSTR);
  Uart2_SendStr(ATSTR);//mqtt提交
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTSUB: 0,1,0,0");//返订阅成功
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTSUB: 0,1,0,0");//返订阅成功
		delay_ms(10);
		if(errcount > 100)     //超时退出死循环 表示服务器连接失败
    {
			errcount = 0;
      break;
    }
  }
  Clear_Buffer();
}

/*********************************************************************************
** 函数名称 ： MakeOnenetPayload(char *Str,u8 Temp,u8 Humi)
** 函数功能 ： 将温度与湿度封装成Onenet有效载荷包
** 输    入	： *str有效载荷保存的目标字符串, 
** 输    入	： Temp 温度值
** 输    入	： Humi 湿度值
** 输    出	： 有效载荷字符串
** 返    回	： 有效载荷字符串长度
*********************************************************************************/
char JsonBuf[256];//临时数据缓存 
u8 MakeOnenetPayload(u8 *Str,u8 Temp,u8 Humi,char* time)//将温度与湿度封装成Onenet有效载荷包
{
  char Payload[] = "{\"dev\":\"%s\":[{\"time\":\"%s\"},{\"id\":\"Temp\",\"datapoints\":[{\"value\":%d}]},{\"id\":\"Humi\",\"datapoints\":[{\"value\":%d}]}]}";//onenet有效载荷固定格式
	unsigned short   StrLen;//有效载荷数据长度
	
	memset(JsonBuf,0,256);
	sprintf(JsonBuf,Payload,ClientID,time,Temp,Humi);//合成数据有效载荷封装
	//printf("\r\n有效载荷(%d):[%s]\r\n",strlen(JsonBuf),(char *)JsonBuf);
	StrLen = strlen(JsonBuf)/sizeof(char);//计算有效载荷的数据长度
	//printf("\r\n有效载荷长度(%d)\r\n",StrLen);
	Str[0] = '\x01';//报文第一个字节的固定显示 '\x01'
	//printf("\r\nStr[0]=0X(%02X)\r\n",Str[0]);
	//报文第2个字节和第三个字节表示有效载荷的长度的固定显示
	Str[1] = (StrLen & 0XFF00) >>8;//高位
	//printf("\r\nStr[1]=0X(%02X)\r\n",Str[1]);
	Str[2] =  StrLen & 0XFF;//低位
	//printf("\r\nStr[2]=0X(%02X)\r\n",Str[2]);
	//拷贝有效载荷数据到Str
	memcpy(Str+3,JsonBuf,StrLen);//从第三个字节开始拷贝
	return (StrLen+3);//返回长度
}

u8 Send_Json[BUFLEN];//AT命令临时缓存
void CSTX_4G_ONENETIOTSenddata(u8 temp,u8 humi)//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
{
	u8 JsonLen;//Json包数据长度
	memset((char *)Send_Json, 0, BUFLEN);//开辟数据缓存   
  memset(atstr,0,BUFLEN);
	memset(time,0,BUFLEN);
	
	Clear_Buffer();	//发送命令之前清空之前的模块反馈的数据
	//获取时间
	Uart2_SendStr("AT+CCLK?\r\n");
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+CCLK: ");//模块反馈可以发送数据了
	if(strx != NULL)
		memcpy(time, strx + strlen("+CCLK: "),19);
	
	Clear_Buffer();
  sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"%s\"\r\n",TopicPost);
  printf("atstr = %s \r\n",atstr);
  Uart2_SendStr(atstr);//mqtt提交
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)">");//准备发送数据
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
    strx = strstr((const char*)buf_uart2.buf,(const char*)">");//准备发送数据 模块的反馈
		delay_ms(30);
    if(errcount > 10)     //超时退出
    {
			errcount = 0;
      break;
     }
  }
	//Control_Led();
	JsonLen = MakeOnenetPayload(Send_Json,temp,humi,time);	//得到json
	USART2_Send((char *)Send_Json,JsonLen);//发送json 数据
	UART2_send_byte(0x1A);
	delay_ms(300);
	errcount = 0;
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//返SEND OK +QMTPUB: 0,0,0
  while(strx==NULL)
  {
		errcount++;
		//Control_Led();
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//返SEND OK
		delay_ms(30);
    if(errcount>10)     //超时退出
    {
			 errcount = 0;
       break;
     }
  }
  Clear_Buffer();
}

/*********************************************************************************
** 函数名称 ： MakeOnenetPayload(char *Str,u8 Temp,u8 Humi)
** 函数功能 ： 将温度与湿度封装成Onenet有效载荷包
** 输    入	： *str有效载荷保存的目标字符串, 
** 输    入	： Temp 温度值
** 输    入	： Humi 湿度值
** 输    出	： 有效载荷字符串
** 返    回	： 有效载荷字符串长度
*********************************************************************************/

u8 MakeOnenetPayloadGPS(u8* Str,char *latstr,char * lonstr,char* time)//将温度与湿度封装成Onenet有效载荷包
{
  char Payload[] = "{\"location\":[{\"dev\":\"%s\"},{\"time\":\"%s\"},{\"value\":{\"lon\":%s,\"lat\":%s}}]}";//onenet有效载荷固定格式
	
	unsigned short StrLen;//有效载荷数据长度
	memset(JsonBuf,0,256);
	sprintf(JsonBuf,Payload,ClientID,time,lonstr,latstr);//合成数据有效载荷封装
	printf("JSONEND= %s \r\n",JsonBuf);
	//printf("\r\n有效载荷(%d):[%s]\r\n",strlen(JsonBuf),(char *)JsonBuf);
	StrLen = strlen(JsonBuf)/sizeof(char);//计算有效载荷的数据长度
	//printf("\r\n有效载荷长度(%d)\r\n",StrLen);
	Str[0] = '\x01';//报文第一个字节的固定显示
	//报文第2个字节和第三个字节表示有效载荷的长度的固定显示
	Str[1] = (StrLen & 0XFF00) >>8;//高位
	Str[2] =  StrLen & 0XFF;//低位
	//拷贝有效载荷数据到Str
	memcpy(Str+3,JsonBuf,StrLen);//从第三个字节开始拷贝
	return (StrLen+3);//返回长度
}

void CSTX_4G_ONENETIOTSenddataGPS(char *latstr,char *lonstr)//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
{
	int JsonLen = 0;
	memset(Send_Json,0,BUFLEN);
  memset(atstr,0,BUFLEN);
	memset(time,0,BUFLEN);
	
	Clear_Buffer();	//发送命令之前清空之前的模块反馈的数据
	Uart2_SendStr("AT+CCLK?\r\n");//mqtt提交
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+CCLK: ");//模块反馈可以发送数据了
	if(strx != NULL)
		memcpy(time, strx + strlen("+CCLK: "),19);
	
	Clear_Buffer();	//发送命令之前清空之前的模块反馈的数据
  sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"%s\"\r\n",TopicPost);	//发送ONENET命令
  printf("atstr = %s \r\n",atstr);
  Uart2_SendStr(atstr);//mqtt提交
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)">");//模块反馈可以发送数据了
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		//Control_Led();
    strx = strstr((const char*)buf_uart2.buf,(const char*)">");//模块反馈可以发送数据了
		delay_ms(30);
    if(errcount > 100)     //防止死循环跳出
    {
			errcount = 0;
      break;
    }
  }
	//命令发送成功下面就去发送数据了
	//Clear_Buffer();
	JsonLen = MakeOnenetPayloadGPS((u8*)Send_Json,latstr,lonstr,time);	//组建CJSON数据
	USART2_Send((char *)Send_Json,JsonLen);//发送json 数据
	UART2_send_byte(0x1A);
	delay_ms(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//返发送成功
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//返发送成功
		delay_ms(10);
		if(errcount > 100)     //超时退出死循环 表示服务器连接失败
    {
			errcount = 0;
      break;
    }
  }
  Clear_Buffer();
}

u8 MakeOnenetPayloadPowerData(u8* Str,double *power_data,char* time,char *latstr,char * lonstr)//将温度与湿度封装成Onenet有效载荷包
{
  char Payload[] = "{\"power_data\":[{\"dev\":\"%s\"},{\"time\":\"%s\"},{\"data\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]},{\"location\":{\"lon\":%s,\"lat\":%s}}]}";										//onenet有效载荷固定格式
	
	unsigned short StrLen;//有效载荷数据长度
	memset(JsonBuf,0,256);
	sprintf(JsonBuf,Payload,ClientID,time,power_data[0],power_data[1],power_data[2],power_data[3],power_data[4],power_data[5]\
					,power_data[6],power_data[7],power_data[8],power_data[9],power_data[10],lonstr,latstr);//合成数据有效载荷封装
	printf("JSONEND = %s \r\n",JsonBuf);
	//printf("\r\n有效载荷(%d):[%s]\r\n",strlen(JsonBuf),(char *)JsonBuf);
	StrLen = strlen(JsonBuf)/sizeof(char);//计算有效载荷的数据长度
	//printf("\r\n有效载荷长度(%d)\r\n",StrLen);
	//Str[0] = '\x01';//报文第一个字节的固定显示
	//报文第2个字节和第三个字节表示有效载荷的长度的固定显示
	//Str[1] = (StrLen & 0XFF00) >> 8;//高位
	//Str[2] =  StrLen & 0XFF;//低位
	//拷贝有效载荷数据到Str
	memcpy(Str,JsonBuf,StrLen);//从第三个字节开始拷贝
	return StrLen;//返回长度
}


void CSTX_4G_ONENETIOTSenddataPowerData(double *power_data,char *latstr,char *lonstr)//上发数据，上发的数据跟对应的插件有关系，用户需要注意插件然后对应数据即可
{
	int JsonLen= 0;
	memset(Send_Json,0,BUFLEN);
  memset(atstr,0,BUFLEN);
	memset(time,0,BUFLEN);
	
	Clear_Buffer();	//发送命令之前清空之前的模块反馈的数据
	Uart2_SendStr("AT+CCLK?\r\n");//mqtt提交
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+CCLK: ");//模块反馈可以发送数据了
	if(strx != NULL)
		memcpy(time, strx + strlen("+CCLK: "),19);

	Clear_Buffer();	//发送命令之前清空之前的模块反馈的数据
  sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"%s\"\r\n",TopicPost);	//发送ONENET命令
  printf("atstr = %s \r\n",atstr);
  Uart2_SendStr(atstr);//mqtt提交
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)">");//模块反馈可以发送数据了
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		//Control_Led();
    strx = strstr((const char*)buf_uart2.buf,(const char*)">");//模块反馈可以发送数据了
		delay_ms(30);
    if(errcount > 100)     //防止死循环跳出
    {
			errcount = 0;
      break;
    }
  }
	//命令发送成功下面就去发送数据了
	//Clear_Buffer();
	JsonLen = MakeOnenetPayloadPowerData((u8*)Send_Json,power_data,time,latstr,lonstr);	//组建CJSON数据
	USART2_Send((char *)Send_Json,JsonLen);//发送json 数据
	UART2_send_byte(0x1A);
	delay_ms(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//返发送成功
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		//Control_Led();
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//返发送成功
		delay_ms(10);
		if(errcount > 100)     //超时退出死循环 表示服务器连接失败
    {
			errcount = 0;
      break;
    }
  }
  Clear_Buffer();
}

void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
  IWDG_SetPrescaler(prer);  //设置IWDG预分频值:设置IWDG预分频值为64
  IWDG_SetReload(rlr);  //设置IWDG重装载值
  IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器
  IWDG_Enable();  //使能IWDG
}
//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}

void NBIOT_RESET(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);       //?￡?é?′?? PC6??????μ????￡?é?′?? μíμ????y3￡1¤×÷
  delay_ms(500);
  GPIO_ResetBits(GPIOA,GPIO_Pin_1);       //?￡?é?′?? PC6??????μ????￡?é?′?? μíμ????y3￡1¤×÷
  delay_ms(300);
}

//AT+QMTSUB=0,1,"testtopic",0
void Control_Led(void)
{
	strx = 0;
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTRECV: 0,0");
	if(strx)
	{
		strx = 0;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"ON");//·μ??+QMTRECV:￡?±í?÷?óê?μ?·t???÷·￠??μ?êy?Y ?aμ?1
    if(strx)
		{
			strx = 0;
			//LEDON;
			printf(buf_uart2.buf);  //?????°′òó?D??￠
			buf_uart2.index = 0;
			memset(buf_uart2.buf,0,BUFLEN);
		}
		strx = strstr((const char*)buf_uart2.buf,(const char*)"OFF");//·μ??+QMTRECV:￡?±í?÷?óê?μ?·t???÷·￠??μ?êy?Y 1?±?μ?1
    if(strx)
		{
			strx = 0;
			//LEDOFF;
			printf(buf_uart2.buf);  //?????°′òó?D??￠
			buf_uart2.index = 0;
			memset(buf_uart2.buf,0,BUFLEN);
		}
	}
}
