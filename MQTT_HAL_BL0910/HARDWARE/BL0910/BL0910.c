#include "BL0910.h"

Com_StatusTypeDef	BL0910_ComSta;
SPI_HandleTypeDef SPI1_Handler;  		//SPI句柄
Eng_Cal_StructDef Eng_Cal;					//电能计算的中间变量，用于参数传递
Eng_CFCnt_StructDef Eng_CFCnt;			//10通道的小于1度电的脉冲底数
Elect_StructDef BL0910_Elect;
B32_B8_TypeDef BL0910_Data;

uint8_t SPI_RST[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};		//the softRst frame of SPI interface (6 Bytes of 0xFF)
uint8_t flg_FrameStart = 0;

/*****************************/
/*BL0910 SPI要求*/
//从模式、半双工通讯、最大通信速率1.5M
//8-bit数据传输、MSB在前、LSB在后
//CPOL = 0、CPHA = 1，即空闲时SCK时钟为低电平，采样时刻：偶数边沿
/*****************************/
void SPI1_Init(void)
{
	SPI1_Handler.Instance = SPI1;                         						//SPI1
  SPI1_Handler.Init.Mode = SPI_MODE_MASTER;             						//设置SPI工作模式，设置为主模式
  SPI1_Handler.Init.Direction = SPI_DIRECTION_2LINES;   						//设置SPI单向或者双向的数据模式:SPI设置为双线模式
  SPI1_Handler.Init.DataSize = SPI_DATASIZE_8BIT;       						//设置SPI的数据大小:SPI发送接收8位帧结构
  SPI1_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;    							//串行同步时钟的空闲状态为低电平
  SPI1_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;         						//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
  SPI1_Handler.Init.NSS = SPI_NSS_SOFT;                 						//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
  SPI1_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;	//定义波特率预分频的值:波特率预分频值为256
  SPI1_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;        						//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
  SPI1_Handler.Init.TIMode = SPI_TIMODE_DISABLE;        						//关闭TI模式
  SPI1_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;		//关闭硬件CRC校验
  SPI1_Handler.Init.CRCPolynomial = 7;                  						//CRC值计算的多项式
  HAL_SPI_Init(&SPI1_Handler);																			//初始化
    
  __HAL_SPI_ENABLE(&SPI1_Handler);                    							//使能SPI1

  SPI1_ReadWriteByte(0Xff);                           							//启动传输
}

//SPI5底层驱动，时钟使能，引脚配置
//此函数会被HAL_SPI_Init()调用
//hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef GPIO_Initure;
    
  __HAL_RCC_SPI1_CLK_ENABLE();        //使能SPI1时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();       //使能GPIOC时钟
   
	//PA7------>MOSI/SDI
	//PA6------>MISO/SDO
	//PA5------>SCK
	//PA4------>NSS/CS
  GPIO_Initure.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_Initure.Mode = GPIO_MODE_AF_PP;              //复用推挽输出
  GPIO_Initure.Pull = GPIO_PULLUP;                  //上拉
  GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;             //快速      
  HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_4;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  				//推挽输出
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2 ~ SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为42Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));	//判断有效性
  __HAL_SPI_DISABLE(&SPI1_Handler);            										//关闭SPI
  SPI1_Handler.Instance->CR1 &= 0XFFC7;          									//位3-5清零，用来设置波特率
  SPI1_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;						//设置SPI速度
  __HAL_SPI_ENABLE(&SPI1_Handler);             										//使能SPI
}

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{
  u8 Rxdata;
  HAL_SPI_TransmitReceive(&SPI1_Handler,&TxData,&Rxdata,1,1000);       
 	return Rxdata;          		    //返回收到的数据		
}

/////////////////////////////////////////////////////////////////
//calculate communication CheckSUM
//求校验和
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
//Master send bytes：Read Command Byte(0x82)，Register Address Byte(Reg_Addr)；
//主机发送0x82+寄存器地址，返回3个字节数据+1个字节的校验和
//Return data bytes: Data_H，Data_M，Data_L,Chksum；
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
			pData->B8[2] = lb_RecData[0];			//高字节	
			pData->B8[1] = lb_RecData[1];			//中字节
			pData->B8[0] = lb_RecData[2];			//低字节
			pData->B8[3] = lb_RecData[3];			//校验和字节，注意仅用于数据上传到上位机时需要返回4字节带校验和数据
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
//	Master send bytes：Write Command Byte(0x81)，Register Address Byte；Data_H，Data_M，Data_L,Chksum
//	Chksum=~((0x81+Addr+Data_H+Data_M+Data_L)&0xFF)
/////////////////////////////////////////////////////////////
void BL0910_SPI_Write(uint8_t REG_Addr,B32_B8_TypeDef *pData)
{
	uint8_t lb_SendData[6];		
	lb_SendData[0] = pData->B8[0];				//小端模式0x123456，[0]=0x56,[1]=0x34,[2]=0x12
	lb_SendData[1] = pData->B8[1];
	lb_SendData[2] = pData->B8[2];	
	lb_SendData[5] = ChkSum_Cal(0x81,REG_Addr,lb_SendData,3);
	lb_SendData[0] = 0x81;
	lb_SendData[1] = REG_Addr;
	lb_SendData[2] = pData->B8[2];				//小端模式0x123456，[0]=0x56,[1]=0x34,[2]=0x12
	lb_SendData[3] = pData->B8[1];
	lb_SendData[4] = pData->B8[0];	
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_RESET);
	//HAL_SPI_Transmit(&hspi1, SPI_RST, 6, 100);		//SPI RESET
	HAL_SPI_Transmit(&SPI1_Handler,lb_SendData,6,100);
	HAL_GPIO_WritePin(CS_GPIO_Port,CS_Pin,GPIO_PIN_SET);
}

///////////////////////////////////////////////////
//BL0910 softRst
//先打开写保护，再写入软复位指令
//注意有两种复位模式
//////////////////////////////////////////////////
void BL0910_SoftRst(uint8_t	Rst_Type)
{
	BL0910_Data.B32 = 0x5555;
	BL0910_SPI_Write(Addr_USR_WRPROT,&BL0910_Data);			//开写保护
	HAL_Delay(1);
	BL0910_SPI_Read(Addr_USR_WRPROT,&BL0910_Data);
	BL0910_Data.B32 = 0x5A5A5A;
	BL0910_SPI_Write(Addr_SOFT_RESET,&BL0910_Data);
	if(Rst_Type == Sys_Rst)
	{
		BL0910_Data.B32 = 0x5A5A5A;
		BL0910_SPI_Write(Addr_SOFT_RESET,&BL0910_Data);
	}
	else			//用户读写寄存器复位
	{
		BL0910_Data.B32 = 0x55AA55;
		BL0910_SPI_Write(Addr_SOFT_RESET,&BL0910_Data);
	}		
	HAL_Delay(1);
}

/*************************************
初始化内容：
1)脉冲输出选择；
2)电流、电压通道增益；
3)电能脉冲读后清零；
4)如果需要，设置防潜动阈值；
5)电参数清零，准备进入运行状态；
**************************************/
BL09XX_Status BL0910_Init(void)
{
	BL0910_Data.B8[0] = 0x55;		//L	//注意串口下传的数据是低字节在前，高字节在后；SPI发送到0950要求数据高字节在前，低字节在后
	BL0910_Data.B8[1] = 0x55;		//M
	BL0910_Data.B8[2] = 0x00;		//H		
	BL0910_SPI_Write(Addr_USR_WRPROT, &BL0910_Data);		
	HAL_Delay(1);
	if(BL09XX_OK != (BL0910_LoadPara()))			//判断装载校准参数是否正常
		return BL09XX_FAIL;
		
	//脉冲输出选择
	BL0910_Data.B32 = 0x010600;			//1通道，无功选全波
	//BL0910_Data.B32=0x011E00;			//7通道，无功选全波
	//BL0910_Data.B32=0x012A00;			//10通道，无功选全波
	BL0910_SPI_Write(Addr_MODE3, &BL0910_Data);
	//电流通道16倍增益，电压通道1倍增益
	BL0910_Data.B32 = 0x000000;							
	BL0910_SPI_Write(Addr_GAIN1, &BL0910_Data);
	BL0910_Data.B32 = 0x000000;
	BL0910_SPI_Write(Addr_GAIN2, &BL0910_Data);
	//关闭功率防潜动
	BL0910_Data.B32 = 0x000000;
	BL0910_SPI_Write(Addr_VAR_WATT_CREEP, &BL0910_Data);
//	BL0910_Data.B32 = Current_K * 8 / 100;					//Creep=RMS			
//	BL0910_SPI_Write(Addr_RMS_CREEP, &BL0910_Data);
//	BL0910_Data.B32 = Power_K / 10;							//Creep=Watt/2
//	BL0910_SPI_Write(Addr_VAR_WATT_CREEP, &BL0910_Data);
	//快速有效值经过高通
	BL0910_Data.B32 = 0x400000;										//bit[22]=1
	BL0910_SPI_Write(Addr_MODE1, &BL0910_Data);	
	//关闭过流指示
	BL0910_Data.B32 = 0xFFFFF;
	BL0910_SPI_Write(Addr_FAST_RMS_CTRL,&BL0910_Data);
			
	//电能脉冲读后清零设置,Reg3B~2F设置为读后清零
	BL0910_Data.B32 = 0x1FFF;
	BL0910_SPI_Write(Addr_RST_ENG, &BL0910_Data);	
	//电参数运算系统复位，Reg3B~2F清零
	BL0910_Data.B32 = 0x5A5A5A;
	BL0910_SPI_Write(Addr_SOFT_RESET, &BL0910_Data);	
	HAL_Delay(2);

	//实际电能从0开始累积
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
读取BL0910相关电参数寄存器，转换为实际电参数
数据运算为整数除法运算；转换后的数据为整数，
实际电参数解析：
电流需要/1000 安培，0.001A/LSB；电压需要/100 伏，0.01V/LSB，频率需要/100 Hz，0.01Hz/LSB，功率需要/10 瓦，0.1瓦/LSB
电能=BL0910_Elect.Energy_1+Eng_CFCnt.Cnt1/Energy_K 度电，0.001度电/LSB 
SPI方式通信
*******************************************/
//double Irms_buff[8] = {0.0};
//BL09XX_Status BL0910_Elect_Proc(void)
//{
//	B32_B8_TypeDef tmp_D;
//	//读写保护寄存器，确认是否出现异常复位情况。如果有则重新初始化,
//	BL0910_SPI_Read(Addr_USR_WRPROT,&BL0910_Data);
//	BL0910_Data.B8[3] = 0; 
//	if(BL0910_Data.B32 != 0x005555)
//		BL0910_Init();
//	else
//	{	
//		//读取电流有效值
//		//实际电流=BL0910_Elect.RMS_I/100 安
//		BL0910_SPI_Read(Addr_1_RMS, &BL0910_Data);
//		tmp_D.B32 = BL0910_Data.B32;
//		tmp_D.B8[3] = 0;									//去除校验字节;
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

//取出超过1度的脉冲，累积到对应的电量变量
void Energy_Cal(uint32_t Cnt)
{
	if(Cnt >= Energy_K)
	{
		Eng_Cal.Energy001 = Cnt / Energy_K;		//超过1度的电量保存在临时变量中，准备加到对应的电量变量中
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
	//判断芯片内部校准参数装载是否正常
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


