#include "BL0910.h"

Com_StatusTypeDef	BL0910_ComSta;
SPI_HandleTypeDef SPI1_Handler;  		//SPI���
Eng_Cal_StructDef Eng_Cal;					//���ܼ�����м���������ڲ�������
Eng_CFCnt_StructDef Eng_CFCnt;			//10ͨ����С��1�ȵ���������
Elect_StructDef BL0910_Elect;
B32_B8_TypeDef BL0910_Data;

uint8_t SPI_RST[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};		//the softRst frame of SPI interface (6 Bytes of 0xFF)
uint8_t flg_FrameStart = 0;

/*****************************/
/*BL0910 SPIҪ��*/
//��ģʽ����˫��ͨѶ�����ͨ������1.5M
//8-bit���ݴ��䡢MSB��ǰ��LSB�ں�
//CPOL = 0��CPHA = 1��������ʱSCKʱ��Ϊ�͵�ƽ������ʱ�̣�ż������
/*****************************/
void SPI1_Init(void)
{
	SPI1_Handler.Instance = SPI1;                         						//SPI1
  SPI1_Handler.Init.Mode = SPI_MODE_MASTER;             						//����SPI����ģʽ������Ϊ��ģʽ
  SPI1_Handler.Init.Direction = SPI_DIRECTION_2LINES;   						//����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
  SPI1_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       						//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
  SPI1_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;    							//����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
  SPI1_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;         						//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
  SPI1_Handler.Init.NSS = SPI_NSS_SOFT;                 						//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
  SPI1_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
  SPI1_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        						//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
  SPI1_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        						//�ر�TIģʽ
  SPI1_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;		//�ر�Ӳ��CRCУ��
  SPI1_Handler.Init.CRCPolynomial = 7;                  						//CRCֵ����Ķ���ʽ
  HAL_SPI_Init(&SPI1_Handler);																			//��ʼ��
    
  __HAL_SPI_ENABLE(&SPI1_Handler);                    							//ʹ��SPI1

  SPI1_ReadWriteByte(0Xff);                           							//��������
}

//SPI5�ײ�������ʱ��ʹ�ܣ���������
//�˺����ᱻHAL_SPI_Init()����
//hspi:SPI���
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef GPIO_Initure;
    
  __HAL_RCC_SPI1_CLK_ENABLE();        //ʹ��SPI1ʱ��
	__HAL_RCC_GPIOA_CLK_ENABLE();       //ʹ��GPIOCʱ��
   
	//PA7------>MOSI/SDI
	//PA6------>MISO/SDO
	//PA5------>SCK
	//PA4------>NSS/CS
  GPIO_Initure.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;              //�����������
  GPIO_Initure.Pull = GPIO_PULLUP;                  //����
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;             //����      
  HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_4;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  				//�������
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//SPI�ٶ����ú���
//SPI�ٶ�=fAPB1/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_2 256
//fAPB1ʱ��һ��Ϊ42Mhz��
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));	//�ж���Ч��
  __HAL_SPI_DISABLE(&SPI1_Handler);            										//�ر�SPI
  SPI1_Handler.Instance->CR1 &= 0XFFC7;          									//λ3-5���㣬�������ò�����
  SPI1_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;						//����SPI�ٶ�
  __HAL_SPI_ENABLE(&SPI1_Handler);             										//ʹ��SPI
}

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{
  u8 Rxdata;
  HAL_SPI_TransmitReceive(&SPI1_Handler,&TxData,&Rxdata,1,1000);       
 	return Rxdata;          		    //�����յ�������		
}

/////////////////////////////////////////////////////////////////
//calculate communication CheckSUM
//��У���
////////////////////////////////////////////////////////////////
uint8_t ChkSum_Cal(uint8_t Cmd,uint8_t REG_Addr,uint8_t *pData,uint8_t Size)
{
	uint8_t tmp;
	uint8_t i;
	tmp = Cmd + REG_Addr;
	for (i = 0; i < Size; ++i)
		tmp += pData[i];
	tmp = ~tmp;
	return tmp;
}

//////////////////////////////////////////////////////////////
//BL0910 SPI Read
//Master send bytes��Read Command Byte(0x82)��Register Address Byte(Reg_Addr)��
//��������0x82+�Ĵ�����ַ������3���ֽ�����+1���ֽڵ�У���
//Return data bytes: Data_H��Data_M��Data_L,Chksum��
//Chksum=~*((0x58+Addr+Data_H+Data_M+Data_L)&0xFF)
//Communication Rate<1M bps
/////////////////////////////////////////////////////////////
void BL0910_SPI_Read(uint8_t REG_Addr,B32_B8_TypeDef *pData)
{
	HAL_StatusTypeDef	sta_SPIRec;
	uint8_t	lb_SendData[2];
	uint8_t lb_RecData[4];	
	uint8_t lb_ChkSumCal;
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);	// CS_LOW
//	HAL_SPI_Transmit(&hspi1, SPI_RST, 6, 100);		//SPI RESET
	lb_SendData[0] = 0x82;
	lb_SendData[1] = REG_Addr;	
	HAL_SPI_Transmit(&SPI1_Handler, lb_SendData, 2, 100);
	sta_SPIRec = HAL_SPI_Receive(&SPI1_Handler, lb_RecData, 4, 100);
	if(sta_SPIRec == HAL_OK)
	{
		lb_ChkSumCal = ChkSum_Cal(0x82,REG_Addr, lb_RecData,3);
		if(lb_ChkSumCal == lb_RecData[3])
		{
			BL0910_ComSta = COM_OK;
			pData->B8[2] = lb_RecData[0];			//���ֽ�	
			pData->B8[1] = lb_RecData[1];			//���ֽ�
			pData->B8[0] = lb_RecData[2];			//���ֽ�
			pData->B8[3] = lb_RecData[3];			//У����ֽڣ�ע������������ϴ�����λ��ʱ��Ҫ����4�ֽڴ�У�������
		}
		else
			BL0910_ComSta = COM_CHKERR;
	}
	else
		BL0910_ComSta = COM_ERROR;
	
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);	//CS_HIGH
}

//////////////////////////////////////////////////////////////
//	BL0910 SPI Write
//	Master send bytes��Write Command Byte(0x81)��Register Address Byte��Data_H��Data_M��Data_L,Chksum
//	Chksum=~((0x81+Addr+Data_H+Data_M+Data_L)&0xFF)
/////////////////////////////////////////////////////////////
void BL0910_SPI_Write(uint8_t REG_Addr,B32_B8_TypeDef *pData)
{
	uint8_t lb_SendData[6];		
	lb_SendData[0] = pData->B8[0];				//С��ģʽ0x123456��[0]=0x56,[1]=0x34,[2]=0x12
	lb_SendData[1] = pData->B8[1];
	lb_SendData[2] = pData->B8[2];	
	lb_SendData[5] = ChkSum_Cal(0x81,REG_Addr,lb_SendData,3);
	lb_SendData[0] = 0x81;
	lb_SendData[1] = REG_Addr;
	lb_SendData[2] = pData->B8[2];				//С��ģʽ0x123456��[0]=0x56,[1]=0x34,[2]=0x12
	lb_SendData[3] = pData->B8[1];
	lb_SendData[4] = pData->B8[0];	
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
	//HAL_SPI_Transmit(&hspi1, SPI_RST, 6, 100);		//SPI RESET
	HAL_SPI_Transmit(&SPI1_Handler,lb_SendData,6,100);
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
}

///////////////////////////////////////////////////
//BL0910 softRst
//�ȴ�д��������д����λָ��
//ע�������ָ�λģʽ
//////////////////////////////////////////////////
void BL0910_SoftRst(uint8_t	Rst_Type)
{
	BL0910_Data.B32 = 0x5555;
	BL0910_SPI_Write(Addr_USR_WRPROT,&BL0910_Data);			//��д����
	HAL_Delay(1);
	BL0910_SPI_Read(Addr_USR_WRPROT,&BL0910_Data);
	BL0910_Data.B32 = 0x5A5A5A;
	BL0910_SPI_Write(Addr_SOFT_RESET,&BL0910_Data);
	if(Rst_Type == Sys_Rst)
	{
		BL0910_Data.B32 = 0x5A5A5A;
		BL0910_SPI_Write(Addr_SOFT_RESET,&BL0910_Data);
	}
	else			//�û���д�Ĵ�����λ
	{
		BL0910_Data.B32 = 0x55AA55;
		BL0910_SPI_Write(Addr_SOFT_RESET,&BL0910_Data);
	}		
	HAL_Delay(1);
}

/*************************************
��ʼ�����ݣ�
1)�������ѡ��
2)��������ѹͨ�����棻
3)��������������㣻
4)�����Ҫ�����÷�Ǳ����ֵ��
5)��������㣬׼����������״̬��
**************************************/
BL09XX_Status BL0910_Init(void)
{
	BL0910_Data.B8[0] = 0x55;		//L	//ע�⴮���´��������ǵ��ֽ���ǰ�����ֽ��ں�SPI���͵�0950Ҫ�����ݸ��ֽ���ǰ�����ֽ��ں�
	BL0910_Data.B8[1] = 0x55;		//M
	BL0910_Data.B8[2] = 0x00;		//H		
	BL0910_SPI_Write(Addr_USR_WRPROT, &BL0910_Data);		
	HAL_Delay(1);
	if(BL09XX_OK != (BL0910_LoadPara()))			//�ж�װ��У׼�����Ƿ�����
		return BL09XX_FAIL;
		
	//�������ѡ��
	BL0910_Data.B32 = 0x010600;			//1ͨ�����޹�ѡȫ��
	//BL0910_Data.B32=0x011E00;			//7ͨ�����޹�ѡȫ��
	//BL0910_Data.B32=0x012A00;			//10ͨ�����޹�ѡȫ��
	BL0910_SPI_Write(Addr_MODE3, &BL0910_Data);
	//����ͨ��16�����棬��ѹͨ��1������
	BL0910_Data.B32 = 0x000000;							
	BL0910_SPI_Write(Addr_GAIN1, &BL0910_Data);
	BL0910_Data.B32 = 0x000000;
	BL0910_SPI_Write(Addr_GAIN2, &BL0910_Data);
	//�رչ��ʷ�Ǳ��
	BL0910_Data.B32 = 0x000000;
	BL0910_SPI_Write(Addr_VAR_WATT_CREEP, &BL0910_Data);
//	BL0910_Data.B32 = Current_K * 8 / 100;					//Creep=RMS			
//	BL0910_SPI_Write(Addr_RMS_CREEP, &BL0910_Data);
//	BL0910_Data.B32 = Power_K / 10;							//Creep=Watt/2
//	BL0910_SPI_Write(Addr_VAR_WATT_CREEP, &BL0910_Data);
	//������Чֵ������ͨ
	BL0910_Data.B32 = 0x400000;										//bit[22]=1
	BL0910_SPI_Write(Addr_MODE1, &BL0910_Data);	
	//�رչ���ָʾ
	BL0910_Data.B32 = 0xFFFFF;
	BL0910_SPI_Write(Addr_FAST_RMS_CTRL,&BL0910_Data);
			
	//�������������������,Reg3B~2F����Ϊ��������
	BL0910_Data.B32 = 0x1FFF;
	BL0910_SPI_Write(Addr_RST_ENG, &BL0910_Data);	
	//���������ϵͳ��λ��Reg3B~2F����
	BL0910_Data.B32 = 0x5A5A5A;
	BL0910_SPI_Write(Addr_SOFT_RESET, &BL0910_Data);	
	HAL_Delay(2);

	//ʵ�ʵ��ܴ�0��ʼ�ۻ�
	BL0910_Elect.Energy_1 = 0x0000;
	BL0910_Elect.Energy_2 = 0x0000;
	BL0910_Elect.Energy_3 = 0x0000;
	BL0910_Elect.Energy_4 = 0x0000;
	BL0910_Elect.Energy_5 = 0x0000;
	BL0910_Elect.Energy_6 = 0x0000;
	BL0910_Elect.Energy_7 = 0x0000;
	BL0910_Elect.Energy_8 = 0x0000;
	BL0910_Elect.Energy_9 = 0x0000;
	BL0910_Elect.Energy_10 = 0x0000;
	BL0910_Elect.Energy_Sum = 0x0000;
	Eng_CFCnt.Cnt1 = 0x0000;
	Eng_CFCnt.Cnt2 = 0x0000;
	Eng_CFCnt.Cnt3 = 0x0000;
	Eng_CFCnt.Cnt4 = 0x0000;
	Eng_CFCnt.Cnt5 = 0x0000;
	Eng_CFCnt.Cnt6 = 0x0000;
	Eng_CFCnt.Cnt7 = 0x0000;
	Eng_CFCnt.Cnt8 = 0x0000;
	Eng_CFCnt.Cnt9 = 0x0000;
	Eng_CFCnt.Cnt10 = 0x0000;
	Eng_CFCnt.Cnt_Sum = 0x0000;
	return BL09XX_OK;
}

/******************************************
��ȡBL0910��ص�����Ĵ�����ת��Ϊʵ�ʵ����
��������Ϊ�����������㣻ת���������Ϊ������
ʵ�ʵ����������
������Ҫ/1000 ���࣬0.001A/LSB����ѹ��Ҫ/100 ����0.01V/LSB��Ƶ����Ҫ/100 Hz��0.01Hz/LSB��������Ҫ/10 �ߣ�0.1��/LSB
����=BL0910_Elect.Energy_1+Eng_CFCnt.Cnt1/Energy_K �ȵ磬0.001�ȵ�/LSB 
SPI��ʽͨ��
*******************************************/
//double Irms_buff[8] = {0.0};
//BL09XX_Status BL0910_Elect_Proc(void)
//{
//	B32_B8_TypeDef tmp_D;
//	//��д�����Ĵ�����ȷ���Ƿ�����쳣��λ���������������³�ʼ��,
//	BL0910_SPI_Read(Addr_USR_WRPROT,&BL0910_Data);
//	BL0910_Data.B8[3] = 0; 
//	if(BL0910_Data.B32 != 0x005555)
//		BL0910_Init();
//	else
//	{	
//		//��ȡ������Чֵ
//		//ʵ�ʵ���=BL0910_Elect.RMS_I/100 ��
//		BL0910_SPI_Read(Addr_1_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;									//ȥ��У���ֽ�;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		Irms_buff[0] = tmp_D.B32;
//		//printf("%f\r\n",Irms_buff[0]);
//		BL0910_Elect.RMS_1 = tmp_D.B32;
//		//Irms_buff[0] = BL0910_Elect.RMS_1;
//		
//		BL0910_SPI_Read(Addr_2_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_2 = tmp_D.B32;
//		Irms_buff[1] = (float)BL0910_Elect.RMS_2 / 100;
//		
//		BL0910_SPI_Read(Addr_3_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_3 = tmp_D.B32;
//		Irms_buff[2] = (float)BL0910_Elect.RMS_3 / 100;
//		
//		BL0910_SPI_Read(Addr_4_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_4 = tmp_D.B32;
//		Irms_buff[3] = (float)BL0910_Elect.RMS_4 / 100;
//		
////		BL0910_SPI_Read(Addr_5_RMS, &BL0910_Data);
////		tmp_D.B32 = BL0910_Data.B32;
////		tmp_D.B8[3] = 0;
////		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
////		BL0910_Elect.RMS_5 = tmp_D.B32;
////		
////		BL0910_SPI_Read(Addr_6_RMS, &BL0910_Data);
////		tmp_D.B32 = BL0910_Data.B32;
////		tmp_D.B8[3] = 0;
////		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
////		BL0910_Elect.RMS_6 = tmp_D.B32;
//		
//		BL0910_SPI_Read(Addr_7_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_7 = tmp_D.B32;
//		Irms_buff[4] = (float)BL0910_Elect.RMS_7 / 100;
//		
//		BL0910_SPI_Read(Addr_8_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_8 = tmp_D.B32;
//		Irms_buff[5] = (float)BL0910_Elect.RMS_8 / 100;
//		
//		BL0910_SPI_Read(Addr_9_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_9 = tmp_D.B32;
//		Irms_buff[6] = (float)BL0910_Elect.RMS_9 / 100;
//		
//		BL0910_SPI_Read(Addr_10_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;
//		//printf("%d\r\n",tmp_D.B32);
//		tmp_D.B32 = tmp_D.B32 * 100 / Current_K;
//		BL0910_Elect.RMS_10 = tmp_D.B32;
//		Irms_buff[7] = (float)BL0910_Elect.RMS_10 / 100;
//		
//		//printf("----------\r\n");
//		
//	}
//	return BL09XX_OK;
//}

//ȡ������1�ȵ����壬�ۻ�����Ӧ�ĵ�������
void Energy_Cal(uint32_t Cnt)
{
	if(Cnt >= Energy_K)
	{
		Eng_Cal.Energy001 = Cnt / Energy_K;		//����1�ȵĵ�����������ʱ�����У�׼���ӵ���Ӧ�ĵ���������
		Eng_Cal.cnt_remainder = Cnt % Energy_K;
	}
	else
	{
		Eng_Cal.Energy001 = 0;
		Eng_Cal.cnt_remainder = Cnt;
	}
}

BL09XX_Status BL0910_LoadPara(void)
{
	//�ж�оƬ�ڲ�У׼����װ���Ƿ�����
	BL0910_SPI_Read(0xE5,&BL0910_Data);
	BL0910_Data.B8[3] = 0;
	if(BL0910_Data.B32 == 0x21)
	{
		BL0910_SPI_Read(0xE6, &BL0910_Data);
		BL0910_Data.B8[3] = 0;
		if(BL0910_Data.B32 == 0x00)
			return BL09XX_OK;	
	}
	else
		return BL09XX_FAIL;
	return BL09XX_FAIL;
}


