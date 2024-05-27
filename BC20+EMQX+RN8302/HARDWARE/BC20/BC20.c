///AT+NSOCL=0
#include "BC20.h"
//#include "main.h"
#include "string.h"
#include "oled.h"
char *strx,*extstrx;
char atstr[BUFLEN];
int err;    //ȫ�ֱ���
char atbuf[BUFLEN];
char objtnum[BUFLEN];//�۲��
char distnum[BUFLEN];//�۲��
BC20 BC20_Status;
int  errcount = 0;	//��������ʧ�ܴ��� ��ֹ��ѭ��
char ATSTR[BUFLEN];	//�齨AT����ĺ���
char time[BUFLEN];

//////////////////////////////////////////////////////////////////////////////////////////////////
char GPRMCSTR[128]; //ת��GPS��Ϣ GPRMC ��γ�ȴ洢���ַ���
char GPRMCSTRLON[64]; //���ȴ洢�ַ��� Ҳ����119.20694
char GPRMCSTRLAT[64]; //ά�ȴ洢�ַ�����Ҳ����26.06451
char IMEINUMBER[64];//+CGSN: "869523052178994"
//////////////////�����Ǿ�����������ı�������/////////////////////////
int Get_GPSdata(void);
void Getdata_Change(char status);


LongLatidata latdata;
float tempdata[2];
char latStrAF[64];          //������ݾ�γ����������
char lonStrAF[64];   //������ݾ�γ��������ʾ
//////////////////////////���Ǿ�ƫ����///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void Clear_Buffer(void)//��մ���2����
{
	printf(buf_uart2.buf);  //���ǰ��ӡ��Ϣ
  Delay(300);
  buf_uart2.index=0;
  memset(buf_uart2.buf,0,BUFLEN);
}

int BC20_Init(void)
{
	int errcount = 0;
  err = 0;    //�ж�ģ�鿨�Ƿ��������Ҫ
  printf("start init BC20\r\n");
  Uart2_SendStr("AT\r\n");
  Delay(300);
  printf(buf_uart2.buf);      //��ӡ�յ��Ĵ�����Ϣ
  printf("get back BC20\r\n");
  strx = strstr((const char*)buf_uart2.buf,(const char*)"OK");//����OK
  Clear_Buffer();	
  while(strx == NULL)
  {
		printf("\r\n��Ƭ���������ӵ�ģ��...\r\n");
    Clear_Buffer();	
    Uart2_SendStr("AT\r\n");
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//����OK
	}
    
  Uart2_SendStr("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"ERROR");//ֻҪ�������� �����ͳɹ�
  if(strx == NULL)
  {
    printf("�ҵĿ����� : %s \r\n",&buf_uart2.buf[8]);
    Clear_Buffer();	
    Delay(300);
  }
  else
  {
    err = 1;
    printf("������ : %s \r\n",buf_uart2.buf);
    Clear_Buffer();
    Delay(300);
  }
  Uart2_SendStr("ATI\r\n");//�������磬PDP
  Delay(300);
  Clear_Buffer();	

  Uart2_SendStr("AT+CGATT?\r\n");//��ѯ����״̬
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//��1 ��������ɹ� ��ȡ��IP��ַ��
  Clear_Buffer();	
  errcount = 0;
  while(strx == NULL)
  {
    errcount++;
    Clear_Buffer();	
    Uart2_SendStr("AT+CGATT?\r\n");//��ȡ����״̬
    Delay(300);
    strx = strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//����1,����ע���ɹ�
    if(errcount > 100)     //��ֹ��ѭ��
    {
			err = 1;
      errcount = 0;
      break;
    }
  }
  Uart2_SendStr("AT+QBAND?\r\n"); //�������ֵ
  Delay(300);
  strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//����OK
  if(strx)
  {
		printf("========BAND========= \r\n %s \r\n",buf_uart2.buf);
    Clear_Buffer();
    Delay(300);
  }
	Uart2_SendStr("AT+CSQ\r\n");//�鿴��ȡCSQֵ
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+CSQ");//����CSQ
  if(strx)
  {
    printf("�ź�����:%s\r\n",buf_uart2.buf);
    Clear_Buffer();
    Delay(300);
	}
  Uart2_SendStr("AT+QCCID\r\n");
  Delay(300);
  Clear_Buffer();	
  return err;
}

void BC20_PDPACT(void)//�������Ϊ���ӷ�������׼��
{
	int errcount = 0;
  Uart2_SendStr("AT+CGPADDR=1\r\n");//�����
  Delay(300);
  Clear_Buffer();
  Uart2_SendStr("AT+CGSN=1\r\n");//�����
  Delay(300);
  Clear_Buffer();
  Uart2_SendStr("AT+CGATT?\r\n");//�����
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//ע����������Ϣ
  Clear_Buffer();	
  while(strx == NULL)
  {
		errcount++;
    Clear_Buffer();
    Uart2_SendStr("AT+CGATT?\r\n");//�����
    Delay(300);
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+CGATT: 1");//һ��Ҫ�ն�����
    if(errcount>100)     //��ֹ��ѭ��
    {
			errcount = 0;
      break;
    }
  }
  Delay(300);
  Clear_Buffer();	
  Delay(300);
}


void BC20_INITGNSS(void)//����GPS
{
  int errcount = 0;
  Uart2_SendStr("AT+QGNSSC=1\r\n");//����GPS Ҫ�ȴ��ܾ�����GNSS
  Delay(1000);
	Delay(1000);
	Delay(1000);
  Clear_Buffer();
  Uart2_SendStr("AT+QGNSSC?\r\n");//��ѯGPS�������
  Delay(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QGNSSC: 1");//�����ɹ�
  Clear_Buffer();
  while(strx == NULL)
  {
    errcount++;
    Clear_Buffer();
    Uart2_SendStr("AT+QGNSSC?\r\n");//��ѯ
    Delay(500);
    strx = strstr((const char*)buf_uart2.buf,(const char*)"+QGNSSC: 1");//�����ɹ�
    if(errcount>100)     //��ֹ��ѭ��
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
	Uart2_SendStr("AT+QGNSSRD=\"NMEA/RMC\"\r\n");//��ѯ����״̬
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"$GNRMC");//��1
		
	while(strx == NULL)
	{
		Clear_Buffer();	
		Uart2_SendStr("AT+QGNSSRD=\"NMEA/RMC\"\r\n");//��ȡ����״̬
		delay_ms(300);
		strx = strstr((const char*)buf_uart2.buf,(const char*)"$GNRMC");//����1,����ע���ɹ�
	}
	sprintf(GPRMCSTR,"%s",strx);
	
	Clear_Buffer();	//��ӡ�յ���GPS��Ϣ
	GPRMCSTR[2]=	'P';
		
	printf("============GETGPRMC==============\r\n%s",GPRMCSTR);		//��ӡGPRMC
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
�����ǽ������������
AT+QGNSSRD="NMEA/RMC"
+QGNSSRD: $GNRMC,091900.00,A,2603.9680,N,11912.4174,E,0.189,,201022,,,A,V*1A
OK
*****************************************************/
//��GPS������
// $GNRMC,091900.00,A,2603.9680,N,11912.4174,E,0.189,,201022,,,A,V*1A
int Get_GPSdata()
{
	int i = 0;
  strx = strstr((const char*)GPRMCSTR,(const char*)"A,");//��ȡγ�ȵ�λ��
  if(strx)
  {
		for(i = 0; i < 9; i++)
			latdata.Latitude[i] = strx[i + 2];//��ȡγ��ֵ2603.9576   
		strx = strstr((const char*)GPRMCSTR,(const char*)"N,");//��ȡ����ֵ
		if(strx)
		{
			for(i = 0; i < 10; i++)	//��ȡ���� 11912.4098
				latdata.longitude[i] = strx[i + 2];		 
		}  
		printf("latdata.Latitude ,%s \r\n",latdata.Latitude);
		printf("latdata.longitude ,%s \r\n",latdata.longitude);
    latdata.getstautus = 1;//                 
	}                        
	else			
		latdata.getstautus = 0;
	Getdata_Change(latdata.getstautus);//���ݻ���
	Clear_Buffer();
	return 0;
}

/*************��������γ������,Ȼ��ֱ���ύ����*******************/	

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
				+ (float)(latdata.longitudedd[6] - 0x30) / 10000) / 60.0;//��ȡ����������
       
		for(i = 0; i < 2; i++)
			latdata.Latitudess[i] = latdata.Latitude[i];
		for(i=2;i<9;i++)
			latdata.Latitudedd[i - 2] = latdata.Latitude[i];	
				 
		latdata.gpsdata[1] = (float)(latdata.Latitudess[0] - 0x30) * 10 + (latdata.Latitudess[1] - 0x30)\
					+((latdata.Latitudedd[0]-0x30)*10+(latdata.Latitudedd[1]-0x30)+(float)(latdata.Latitudedd[3]-0x30)/10\
					+(float)(latdata.Latitudedd[4] - 0x30) / 100 + (float)(latdata.Latitudedd[5] - 0x30) / 1000\
					+(float)(latdata.Latitudedd[6] - 0x30) / 10000) / 60.0;//��ȡ����������b

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
	Uart2_SendStr(ATSTR);//ѡ���ͻ�����·
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTOPEN: 0,0");//��OK
	errcount = 0;	
	while(strx == NULL)
	{
		errcount++;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTOPEN: 0,0");//����OK,���Ӱ����Ƴɹ�
		delay_ms(100);
		if(errcount > 100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
		{
			errcount = 0;
			break;
		}
	}
	Clear_Buffer();
	
	memset(ATSTR,0,BUFLEN);
	sprintf(ATSTR,"AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n",ClientID,Username,Password); //+QMTCONN: 0,0,0
	Uart2_SendStr(ATSTR);//ѡ���ͻ�����·
	delay_ms(1000);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//��OK
	errcount = 0;	
	while(strx == NULL)
	{
		errcount++;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTCONN: 0,0,0");//����OK,���Ӱ����Ƴɹ�
		delay_ms(100);
		if(errcount > 100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
		{
			errcount = 0;
			break;
		}
	}
	Clear_Buffer();
	
	Uart2_SendStr("AT+CCLK=?\r\n");//��ѯ����״̬
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"OK");//��OK
	errcount = 0;	
	while(strx==NULL)
	{
		errcount++;
		strx=strstr((const char*)buf_uart2.buf,(const char*)"OK");//����OK,���Ӱ����Ƴɹ�
		delay_ms(100);
		if(errcount > 100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
		{
			errcount = 0;
			break;
		}
	}
	Clear_Buffer();
	
	memset(ATSTR,0,BUFLEN);
	Clear_Buffer();	//��������֮ǰ���֮ǰ��ģ�鷴��������
  sprintf(ATSTR,"AT+QMTSUB=0,1,\"%s\",0\r\n",Topic);	//����ONENET����
  printf("atstr = %s \r\n",ATSTR);
  Uart2_SendStr(ATSTR);//mqtt�ύ
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTSUB: 0,1,0,0");//�����ĳɹ�
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTSUB: 0,1,0,0");//�����ĳɹ�
		delay_ms(10);
		if(errcount > 100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
    {
			errcount = 0;
      break;
    }
  }
  Clear_Buffer();
}

/*********************************************************************************
** �������� �� MakeOnenetPayload(char *Str,u8 Temp,u8 Humi)
** �������� �� ���¶���ʪ�ȷ�װ��Onenet��Ч�غɰ�
** ��    ��	�� *str��Ч�غɱ����Ŀ���ַ���, 
** ��    ��	�� Temp �¶�ֵ
** ��    ��	�� Humi ʪ��ֵ
** ��    ��	�� ��Ч�غ��ַ���
** ��    ��	�� ��Ч�غ��ַ�������
*********************************************************************************/
char JsonBuf[256];//��ʱ���ݻ��� 
u8 MakeOnenetPayload(u8 *Str,u8 Temp,u8 Humi,char* time)//���¶���ʪ�ȷ�װ��Onenet��Ч�غɰ�
{
  char Payload[] = "{\"dev\":\"%s\":[{\"time\":\"%s\"},{\"id\":\"Temp\",\"datapoints\":[{\"value\":%d}]},{\"id\":\"Humi\",\"datapoints\":[{\"value\":%d}]}]}";//onenet��Ч�غɹ̶���ʽ
	unsigned short   StrLen;//��Ч�غ����ݳ���
	
	memset(JsonBuf,0,256);
	sprintf(JsonBuf,Payload,ClientID,time,Temp,Humi);//�ϳ�������Ч�غɷ�װ
	//printf("\r\n��Ч�غ�(%d):[%s]\r\n",strlen(JsonBuf),(char *)JsonBuf);
	StrLen = strlen(JsonBuf)/sizeof(char);//������Ч�غɵ����ݳ���
	//printf("\r\n��Ч�غɳ���(%d)\r\n",StrLen);
	Str[0] = '\x01';//���ĵ�һ���ֽڵĹ̶���ʾ '\x01'
	//printf("\r\nStr[0]=0X(%02X)\r\n",Str[0]);
	//���ĵ�2���ֽں͵������ֽڱ�ʾ��Ч�غɵĳ��ȵĹ̶���ʾ
	Str[1] = (StrLen & 0XFF00) >>8;//��λ
	//printf("\r\nStr[1]=0X(%02X)\r\n",Str[1]);
	Str[2] =  StrLen & 0XFF;//��λ
	//printf("\r\nStr[2]=0X(%02X)\r\n",Str[2]);
	//������Ч�غ����ݵ�Str
	memcpy(Str+3,JsonBuf,StrLen);//�ӵ������ֽڿ�ʼ����
	return (StrLen+3);//���س���
}

u8 Send_Json[BUFLEN];//AT������ʱ����
void CSTX_4G_ONENETIOTSenddata(u8 temp,u8 humi)//�Ϸ����ݣ��Ϸ������ݸ���Ӧ�Ĳ���й�ϵ���û���Ҫע����Ȼ���Ӧ���ݼ���
{
	u8 JsonLen;//Json�����ݳ���
	memset((char *)Send_Json, 0, BUFLEN);//�������ݻ���   
  memset(atstr,0,BUFLEN);
	memset(time,0,BUFLEN);
	
	Clear_Buffer();	//��������֮ǰ���֮ǰ��ģ�鷴��������
	//��ȡʱ��
	Uart2_SendStr("AT+CCLK?\r\n");
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+CCLK: ");//ģ�鷴�����Է���������
	if(strx != NULL)
		memcpy(time, strx + strlen("+CCLK: "),19);
	
	Clear_Buffer();
  sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"%s\"\r\n",TopicPost);
  printf("atstr = %s \r\n",atstr);
  Uart2_SendStr(atstr);//mqtt�ύ
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)">");//׼����������
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
    strx = strstr((const char*)buf_uart2.buf,(const char*)">");//׼���������� ģ��ķ���
		delay_ms(30);
    if(errcount > 10)     //��ʱ�˳�
    {
			errcount = 0;
      break;
     }
  }
	//Control_Led();
	JsonLen = MakeOnenetPayload(Send_Json,temp,humi,time);	//�õ�json
	USART2_Send((char *)Send_Json,JsonLen);//����json ����
	UART2_send_byte(0x1A);
	delay_ms(300);
	errcount = 0;
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//��SEND OK +QMTPUB: 0,0,0
  while(strx==NULL)
  {
		errcount++;
		//Control_Led();
    strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//��SEND OK
		delay_ms(30);
    if(errcount>10)     //��ʱ�˳�
    {
			 errcount = 0;
       break;
     }
  }
  Clear_Buffer();
}

/*********************************************************************************
** �������� �� MakeOnenetPayload(char *Str,u8 Temp,u8 Humi)
** �������� �� ���¶���ʪ�ȷ�װ��Onenet��Ч�غɰ�
** ��    ��	�� *str��Ч�غɱ����Ŀ���ַ���, 
** ��    ��	�� Temp �¶�ֵ
** ��    ��	�� Humi ʪ��ֵ
** ��    ��	�� ��Ч�غ��ַ���
** ��    ��	�� ��Ч�غ��ַ�������
*********************************************************************************/

u8 MakeOnenetPayloadGPS(u8* Str,char *latstr,char * lonstr,char* time)//���¶���ʪ�ȷ�װ��Onenet��Ч�غɰ�
{
  char Payload[] = "{\"location\":[{\"dev\":\"%s\"},{\"time\":\"%s\"},{\"value\":{\"lon\":%s,\"lat\":%s}}]}";//onenet��Ч�غɹ̶���ʽ
	
	unsigned short StrLen;//��Ч�غ����ݳ���
	memset(JsonBuf,0,256);
	sprintf(JsonBuf,Payload,ClientID,time,lonstr,latstr);//�ϳ�������Ч�غɷ�װ
	printf("JSONEND= %s \r\n",JsonBuf);
	//printf("\r\n��Ч�غ�(%d):[%s]\r\n",strlen(JsonBuf),(char *)JsonBuf);
	StrLen = strlen(JsonBuf)/sizeof(char);//������Ч�غɵ����ݳ���
	//printf("\r\n��Ч�غɳ���(%d)\r\n",StrLen);
	Str[0] = '\x01';//���ĵ�һ���ֽڵĹ̶���ʾ
	//���ĵ�2���ֽں͵������ֽڱ�ʾ��Ч�غɵĳ��ȵĹ̶���ʾ
	Str[1] = (StrLen & 0XFF00) >>8;//��λ
	Str[2] =  StrLen & 0XFF;//��λ
	//������Ч�غ����ݵ�Str
	memcpy(Str+3,JsonBuf,StrLen);//�ӵ������ֽڿ�ʼ����
	return (StrLen+3);//���س���
}

void CSTX_4G_ONENETIOTSenddataGPS(char *latstr,char *lonstr)//�Ϸ����ݣ��Ϸ������ݸ���Ӧ�Ĳ���й�ϵ���û���Ҫע����Ȼ���Ӧ���ݼ���
{
	int JsonLen = 0;
	memset(Send_Json,0,BUFLEN);
  memset(atstr,0,BUFLEN);
	memset(time,0,BUFLEN);
	
	Clear_Buffer();	//��������֮ǰ���֮ǰ��ģ�鷴��������
	Uart2_SendStr("AT+CCLK?\r\n");//mqtt�ύ
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+CCLK: ");//ģ�鷴�����Է���������
	if(strx != NULL)
		memcpy(time, strx + strlen("+CCLK: "),19);
	
	Clear_Buffer();	//��������֮ǰ���֮ǰ��ģ�鷴��������
  sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"%s\"\r\n",TopicPost);	//����ONENET����
  printf("atstr = %s \r\n",atstr);
  Uart2_SendStr(atstr);//mqtt�ύ
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)">");//ģ�鷴�����Է���������
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		//Control_Led();
    strx = strstr((const char*)buf_uart2.buf,(const char*)">");//ģ�鷴�����Է���������
		delay_ms(30);
    if(errcount > 100)     //��ֹ��ѭ������
    {
			errcount = 0;
      break;
    }
  }
	//����ͳɹ������ȥ����������
	//Clear_Buffer();
	JsonLen = MakeOnenetPayloadGPS((u8*)Send_Json,latstr,lonstr,time);	//�齨CJSON����
	USART2_Send((char *)Send_Json,JsonLen);//����json ����
	UART2_send_byte(0x1A);
	delay_ms(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//�����ͳɹ�
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//�����ͳɹ�
		delay_ms(10);
		if(errcount > 100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
    {
			errcount = 0;
      break;
    }
  }
  Clear_Buffer();
}

u8 MakeOnenetPayloadPowerData(u8* Str,double *power_data,char* time,char *latstr,char * lonstr)//���¶���ʪ�ȷ�װ��Onenet��Ч�غɰ�
{
  char Payload[] = "{\"power_data\":[{\"dev\":\"%s\"},{\"time\":\"%s\"},{\"data\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f]},{\"location\":{\"lon\":%s,\"lat\":%s}}]}";										//onenet��Ч�غɹ̶���ʽ
	
	unsigned short StrLen;//��Ч�غ����ݳ���
	memset(JsonBuf,0,256);
	sprintf(JsonBuf,Payload,ClientID,time,power_data[0],power_data[1],power_data[2],power_data[3],power_data[4],power_data[5]\
					,power_data[6],power_data[7],power_data[8],power_data[9],power_data[10],lonstr,latstr);//�ϳ�������Ч�غɷ�װ
	printf("JSONEND = %s \r\n",JsonBuf);
	//printf("\r\n��Ч�غ�(%d):[%s]\r\n",strlen(JsonBuf),(char *)JsonBuf);
	StrLen = strlen(JsonBuf)/sizeof(char);//������Ч�غɵ����ݳ���
	//printf("\r\n��Ч�غɳ���(%d)\r\n",StrLen);
	//Str[0] = '\x01';//���ĵ�һ���ֽڵĹ̶���ʾ
	//���ĵ�2���ֽں͵������ֽڱ�ʾ��Ч�غɵĳ��ȵĹ̶���ʾ
	//Str[1] = (StrLen & 0XFF00) >> 8;//��λ
	//Str[2] =  StrLen & 0XFF;//��λ
	//������Ч�غ����ݵ�Str
	memcpy(Str,JsonBuf,StrLen);//�ӵ������ֽڿ�ʼ����
	return StrLen;//���س���
}


void CSTX_4G_ONENETIOTSenddataPowerData(double *power_data,char *latstr,char *lonstr)//�Ϸ����ݣ��Ϸ������ݸ���Ӧ�Ĳ���й�ϵ���û���Ҫע����Ȼ���Ӧ���ݼ���
{
	int JsonLen= 0;
	memset(Send_Json,0,BUFLEN);
  memset(atstr,0,BUFLEN);
	memset(time,0,BUFLEN);
	
	Clear_Buffer();	//��������֮ǰ���֮ǰ��ģ�鷴��������
	Uart2_SendStr("AT+CCLK?\r\n");//mqtt�ύ
	delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)"+CCLK: ");//ģ�鷴�����Է���������
	if(strx != NULL)
		memcpy(time, strx + strlen("+CCLK: "),19);

	Clear_Buffer();	//��������֮ǰ���֮ǰ��ģ�鷴��������
  sprintf(atstr,"AT+QMTPUB=0,0,0,0,\"%s\"\r\n",TopicPost);	//����ONENET����
  printf("atstr = %s \r\n",atstr);
  Uart2_SendStr(atstr);//mqtt�ύ
  delay_ms(300);
	strx = strstr((const char*)buf_uart2.buf,(const char*)">");//ģ�鷴�����Է���������
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		//Control_Led();
    strx = strstr((const char*)buf_uart2.buf,(const char*)">");//ģ�鷴�����Է���������
		delay_ms(30);
    if(errcount > 100)     //��ֹ��ѭ������
    {
			errcount = 0;
      break;
    }
  }
	//����ͳɹ������ȥ����������
	//Clear_Buffer();
	JsonLen = MakeOnenetPayloadPowerData((u8*)Send_Json,power_data,time,latstr,lonstr);	//�齨CJSON����
	USART2_Send((char *)Send_Json,JsonLen);//����json ����
	UART2_send_byte(0x1A);
	delay_ms(300);
  strx = strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//�����ͳɹ�
	errcount = 0;
  while(strx == NULL)
  {
		errcount++;
		//Control_Led();
		strx=strstr((const char*)buf_uart2.buf,(const char*)"+QMTPUB: 0,0,0");//�����ͳɹ�
		delay_ms(10);
		if(errcount > 100)     //��ʱ�˳���ѭ�� ��ʾ����������ʧ��
    {
			errcount = 0;
      break;
    }
  }
  Clear_Buffer();
}

void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
  IWDG_SetPrescaler(prer);  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
  IWDG_SetReload(rlr);  //����IWDG��װ��ֵ
  IWDG_ReloadCounter();  //����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
  IWDG_Enable();  //ʹ��IWDG
}
//ι�������Ź�
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}

void NBIOT_RESET(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);       //?��?��?��?? PC6??????��????��?��?��?? �̨���????y3��1�����
  delay_ms(500);
  GPIO_ResetBits(GPIOA,GPIO_Pin_1);       //?��?��?��?? PC6??????��????��?��?��?? �̨���????y3��1�����
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
		strx = strstr((const char*)buf_uart2.buf,(const char*)"ON");//����??+QMTRECV:��?����?��?����?��?��t???�¡���??��?��y?Y ?a��?1
    if(strx)
		{
			strx = 0;
			//LEDON;
			printf(buf_uart2.buf);  //?????��䨰��?D??��
			buf_uart2.index = 0;
			memset(buf_uart2.buf,0,BUFLEN);
		}
		strx = strstr((const char*)buf_uart2.buf,(const char*)"OFF");//����??+QMTRECV:��?����?��?����?��?��t???�¡���??��?��y?Y 1?��?��?1
    if(strx)
		{
			strx = 0;
			//LEDOFF;
			printf(buf_uart2.buf);  //?????��䨰��?D??��
			buf_uart2.index = 0;
			memset(buf_uart2.buf,0,BUFLEN);
		}
	}
}
