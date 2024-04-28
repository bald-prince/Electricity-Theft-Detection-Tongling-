#ifndef _BL0910_H
#define _BL0910_H
#include "stdint.h"
#include "main.h"
#include "usart.h"

#define BL0910_SPI	1

//复位模式定义
#define Sys_Rst			0
#define CalReg_RST	1
//BL0910单相10路充电桩模式对应寄存器地址
#define Addr_VERSION						0x00
#define	Addr_1_WAVE							0x01
#define	Addr_2_WAVE							0x02
#define	Addr_3_WAVE							0x03
#define	Addr_4_WAVE							0x04
#define	Addr_5_WAVE							0x05
#define	Addr_6_WAVE							0x06
#define	Addr_7_WAVE							0x07
#define	Addr_8_WAVE							0x08
#define	Addr_9_WAVE							0x09
#define	Addr_10_WAVE						0x0A
#define	Addr_V_WAVE							0x0B
#define Addr_1_RMS							0x0C
#define Addr_2_RMS							0x0D
#define Addr_3_RMS							0x0E
#define Addr_4_RMS							0x0F
#define Addr_5_RMS							0x10
#define Addr_6_RMS							0x11
#define Addr_7_RMS							0x12
#define Addr_8_RMS							0x13
#define Addr_9_RMS							0x14
#define Addr_10_RMS							0x15
#define Addr_V_RMS							0x16
#define Addr_1_FAST_RMS					0x17
#define Addr_2_FAST_RMS					0x18
#define Addr_3_FAST_RMS					0x19
#define Addr_4_FAST_RMS					0x1A
#define Addr_5_FAST_RMS					0x1B
#define Addr_6_FAST_RMS					0x1C
#define Addr_7_FAST_RMS					0x1D
#define Addr_8_FAST_RMS					0x1E
#define Addr_9_FAST_RMS					0x1F
#define Addr_10_FAST_RMS				0x20
#define Addr_V_FAST_RMS					0x21
#define Addr_WATT1							0x22
#define Addr_WATT2							0x23
#define Addr_WATT3							0x24
#define Addr_WATT4							0x25
#define Addr_WATT5							0x26
#define Addr_WATT6							0x27
#define Addr_WATT7							0x28
#define Addr_WATT8							0x29
#define Addr_WATT9							0x2A
#define Addr_WATT10							0x2B
#define Addr_WATT								0x2C
#define Addr_VAR								0x2D
#define Addr_VA									0x2E
#define Addr_CF1_CNT						0x2F
#define Addr_CF2_CNT						0x30
#define Addr_CF3_CNT						0x31
#define Addr_CF4_CNT						0x32
#define Addr_CF5_CNT						0x33
#define Addr_CF6_CNT						0x34
#define Addr_CF7_CNT						0x35
#define Addr_CF8_CNT						0x36
#define Addr_CF9_CNT						0x37
#define Addr_CF10_CNT						0x38
#define Addr_CF_CNT							0x39
#define Addr_CFQ_CNT						0x3A
#define Addr_CFS_CNT						0x3B
#define Addr_ANGLE1							0x3C
#define Addr_ANGLE2							0x3D
#define Addr_ANGLE3							0x3E
#define Addr_ANGLE4							0x3F
#define Addr_ANGLE5							0x40
#define Addr_ANGLE6							0x41
#define Addr_ANGLE7							0x42
#define Addr_ANGLE8							0x43
#define Addr_ANGLE9							0x44
#define Addr_ANGLE10						0x45
#define Addr_1_FAST_RMS_HOLD		0x46
#define Addr_2_FAST_RMS_HOLD		0x47
#define Addr_3_FAST_RMS_HOLD		0x48
#define Addr_4_FAST_RMS_HOLD		0x49
#define Addr_PF									0x4A
#define Addr_LINE_WATTHR				0x4B
#define Addr_LINE_VARHR					0x4C
#define Addr_SIGN								0x4D
#define Addr_PERIOD							0x4E

#define Addr_STATUS1						0x54
#define Addr_STATUS2						0x55
#define Addr_STATUS3						0x56
#define Addr_5_FAST_RMS_HOLD		0x57
#define Addr_6_FAST_RMS_HOLD		0x58
#define Addr_7_FAST_RMS_HOLD		0x59
#define Addr_8_FAST_RMS_HOLD		0x5A
#define Addr_9_FAST_RMS_HOLD		0x5B
#define Addr_10_FAST_RMS_HOLD		0x5C
#define Addr_VAR1								0x5D		//可选通道（全波）无功功率	
#define Addr_TPS1								0x5E
#define Addr_TPS2								0x5F
//用户操作寄存器
#define Addr_GAIN1							0x60
#define Addr_GAIN2							0x61
	
#define Addr_PHASE1_2						0x64
#define Addr_PHASE3_4						0x65
#define Addr_PHASE5_6						0x66
#define Addr_PHASE7_8						0x67
#define Addr_PHASE9_10					0x68
#define Addr_PHASE11						0x69
#define Addr_VAR_PHCAL_I				0x6A
#define Addr_VAR_PHCAL_V				0x6B
#define Addr_RMSGN1							0x6C
#define Addr_RMSGN2							0x6D
#define Addr_RMSGN3							0x6E
#define Addr_RMSGN4							0x6F
#define Addr_RMSGN5							0x70
#define Addr_RMSGN6							0x71
#define Addr_RMSGN7							0x72
#define Addr_RMSGN8							0x73
#define Addr_RMSGN9							0x74
#define Addr_RMSGN10						0x75
#define Addr_RMSGN11						0x76
#define Addr_RMSOS1							0x77
#define Addr_RMSOS2							0x78
#define Addr_RMSOS3							0x79
#define Addr_RMSOS4							0x7A
#define Addr_RMSOS5							0x7B
#define Addr_RMSOS6							0x7C
#define Addr_RMSOS7							0x7D
#define Addr_RMSOS8							0x7E
#define Addr_RMSOS9							0x7F
#define Addr_RMSOS10						0x80
#define Addr_RMSOS11						0x81
#define Addr_WA_LOS1_2					0x82
#define Addr_WA_LOS3_4					0x83
#define Addr_WA_LOS5_6					0x84
#define Addr_WA_LOS7_8					0x85
#define Addr_WA_LOS9_10					0x86
#define Addr_VAR_LOS						0x87
#define Addr_VAR_WATT_CREEP			0x88
#define Addr_WA_CREEP2					0x89
#define	Addr_RMS_CREEP					0x8A
#define Addr_FAST_RMS_CTRL			0x8B
#define Addr_I_V_PKLVL					0x8C

#define Addr_SAGCYC_ZXTOUT			0x8E
#define Addr_SAGLVL_LINECYC			0x8F
#define Addr_FLAG_CTRL					0x90
#define Addr_FLAG_CTRL1					0x91
#define Addr_FLAG_CTRL2					0x92
#define Addr_ADC_PD							0x93
#define Addr_TPS_CTRL						0x94
#define Addr_TPS2_A_B						0x95
#define Addr_MODE1							0x96
#define Addr_MODE2							0x97
#define Addr_MODE3							0x98

#define Addr_MASK1							0x9A
#define Addr_MASK2							0x9B

#define Addr_RST_ENG						0x9D
#define Addr_USR_WRPROT					0x9E
#define Addr_SOFT_RESET					0x9F
//校表寄存器，OTP保存
#define Addr_CHGN1							0xA0
#define Addr_CHGN2							0xA1
#define Addr_CHGN3							0xA2
#define Addr_CHGN4							0xA3
#define Addr_CHGN5							0xA4
#define Addr_CHGN6							0xA5
#define Addr_CHGN7							0xA6
#define Addr_CHGN8							0xA7
#define Addr_CHGN9							0xA8
#define Addr_CHGN10							0xA9
#define Addr_CHGN11							0xAA
#define Addr_CHOS1							0xAB
#define Addr_CHOS2							0xAC
#define Addr_CHOS3							0xAD
#define Addr_CHOS4							0xAE
#define Addr_CHOS5							0xAF
#define Addr_CHOS6							0xB0
#define Addr_CHOS7							0xB1
#define Addr_CHOS8							0xB2
#define Addr_CHOS9							0xB3
#define Addr_CHOS10							0xB4
#define Addr_CHOS11							0xB5
#define Addr_WATTGN1						0xB6
#define Addr_WATTGN2						0xB7
#define Addr_WATTGN3						0xB8
#define Addr_WATTGN4						0xB9
#define Addr_WATTGN5						0xBA
#define Addr_WATTGN6						0xBB
#define Addr_WATTGN7						0xBC
#define Addr_WATTGN8						0xBD
#define Addr_WATTGN9						0xBE
#define Addr_WATTGN10						0xBF
#define Addr_WATTOS1						0xC0
#define Addr_WATTOS2						0xC1
#define Addr_WATTOS3						0xC2
#define Addr_WATTOS4						0xC3
#define Addr_WATTOS5						0xC4
#define Addr_WATTOS6						0xC5
#define Addr_WATTOS7						0xC6
#define Addr_WATTOS8						0xC7
#define Addr_WATTOS9						0xC8
#define Addr_WATTOS10						0xC9
#define Addr_VARGN							0xCA
#define Addr_VAROS							0xCB
#define Addr_VAGN								0xCC
#define Addr_VAOS								0xCD
#define Addr_CFDIV							0xCE

//电参数转换系数
//电压通道电阻分压采用300K*5+1K进行分压，电阻精度0.5%
//电流通道采用1毫欧电阻进行电流采样，采用美隆的1毫欧采样电阻，实际大约1.02毫欧，系数进行修正
//16倍    957703
//1倍     59856   2000：1
#define Voltage_k	    11998					//13162*1*1000/(1.097*(300*5+1))=7789
#define Current_K	    59856				//206000*0.946/1.097=173100		运算时系数缩小10倍
#define Power_K		    2740					//646.6*0.946*1*1000/(1.097*1.097*(300*5+1))=321.5 运算时放大10倍
#define Energy_K	    8421					//1度电对应的脉冲数      		3600000*16*321.5/(4194304*0.032768*16)=8421
#define Energy_Sum_K 	2351980						//Energy_K/16

#define  CS_GPIO_Port  	GPIOA		
#define  CS_Pin   			GPIO_PIN_4

typedef union
{
	uint8_t 	B8[4];
	uint32_t	B32;	
}B32_B8_TypeDef;

typedef enum
{
	COM_OK			= 0x00U,
	COM_ERROR		= 0x01U,
	COM_CHKERR	= 0x02U
}Com_StatusTypeDef;

typedef enum
{
	OTP_SUCCESS	= 0x01U,
	OTP_Fail = 0x00U	
}OTP_ProgStatusTpyeDef;

typedef enum StatusTyeDef
{
	BL09XX_OK 	= 0x00U,
	BL09XX_FAIL = 0x01U
}BL09XX_Status;

typedef struct
{
	uint16_t RMS_1;			//XX.XXX	A		0.001安/LSB
	uint16_t RMS_2;
	uint16_t RMS_3;
	uint16_t RMS_4;
	uint16_t RMS_5;
	uint16_t RMS_6;
	uint16_t RMS_7;
	uint16_t RMS_8;
	uint16_t RMS_9;
	uint16_t RMS_10;
	uint16_t RMS_V;			//XXX.X	V			0.1伏/LSB
	uint16_t WATT_1;		//XXXX.X	W		0.1瓦/LSB
	uint16_t WATT_2;
	uint16_t WATT_3;
	uint16_t WATT_4;
	uint16_t WATT_5;
	uint16_t WATT_6;
	uint16_t WATT_7;
	uint16_t WATT_8;
	uint16_t WATT_9;
	uint16_t WATT_10;
	uint16_t Energy_1;		//XXXXXX	kWh   1度电/LSB，最大65535度电
	uint16_t Energy_2;
	uint16_t Energy_3;
	uint16_t Energy_4;
	uint16_t Energy_5;
	uint16_t Energy_6;
	uint16_t Energy_7;
	uint16_t Energy_8;
	uint16_t Energy_9;
	uint16_t Energy_10;
	uint16_t Energy_Sum;
	uint16_t Period;		//XX.XX	Hz		  0.01Hz/LSB
	uint16_t TPS1;			//XX.XX ℃			0.01℃/LSB
	uint16_t TPS2;	
}Elect_StructDef;


//存放小于1度电的脉冲底数
typedef struct
{
	uint16_t	Cnt1;
	uint16_t	Cnt2;
	uint16_t	Cnt3;
	uint16_t	Cnt4;
	uint16_t	Cnt5;
	uint16_t	Cnt6;
	uint16_t	Cnt7;
	uint16_t	Cnt8;
	uint16_t	Cnt9;
	uint16_t	Cnt10;
	uint16_t  Cnt_Sum;	
}Eng_CFCnt_StructDef;
//用于电能计算的中间数据
typedef struct 
{
	uint16_t Energy001;		//1度以上的电累积				
	uint16_t cnt_remainder;	//<1度电的脉冲底数
}Eng_Cal_StructDef;

extern Eng_Cal_StructDef Eng_Cal;					//电能计算的中间变量，用于参数传递
extern Eng_CFCnt_StructDef Eng_CFCnt;			//10通道的小于1度电的脉冲底数
extern Elect_StructDef BL0910_Elect;
extern B32_B8_TypeDef BL0910_Data;
extern uint8_t flg_FrameStart;
extern double Irms_buff[8];
extern double Energy_buff[9];
extern double Freq_Electricity_consumption[10];

uint8_t ChkSum_Cal(uint8_t Cmd,uint8_t REG_Addr,uint8_t *pData,uint8_t Size);
void BL0910_SPI_Write(uint8_t REG_Addr,B32_B8_TypeDef *pData);
void BL0910_SPI_Read(uint8_t REG_Addr,B32_B8_TypeDef *pData);
void BL0910_SoftRst(uint8_t	Rst_Type);
BL09XX_Status BL0910_Init(void);
BL09XX_Status BL0910_Elect_Proc(void);
void Energy_Cal(uint32_t Cnt);
BL09XX_Status BL0910_LoadPara(void);
void SPI1_Init(void);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI1_ReadWriteByte(u8 TxData);

#endif

