#ifndef _RN8302_H_
#define _RN8302_H_

#include <stdio.h>
#include "stm32f10x.h"
#include "config.h"
#include "oled.h"

#ifdef TEST_8302B

#define SPI_SIMULATE_IO			1

#define SPI_PORT 					  GPIOA
#define SPI_MOSI_PIN    	  GPIO_Pin_4
#define SPI_CS_PIN				  GPIO_Pin_5
#define SPI_CLK_PIN   		  GPIO_Pin_6
#define SPI_MISO_PIN    	  GPIO_Pin_7			// 上拉输入

#define SPI_MOSI_HIGH()     GPIO_SetBits(SPI_PORT, SPI_MOSI_PIN)	// 输出MOSI
#define SPI_CS_HIGH()			  GPIO_SetBits(SPI_PORT, SPI_CS_PIN)
#define SPI_CLK_HIGH()   	  GPIO_SetBits(SPI_PORT, SPI_CLK_PIN)
#define SPI_MISO_HIGH()     GPIO_SetBits(SPI_PORT, SPI_MISO_PIN)	// 配置为输入,对应8302引脚DO

#define SPI_MOSI_LOW()      GPIO_ResetBits(SPI_PORT, SPI_MOSI_PIN)
#define SPI_CS_LOW() 			  GPIO_ResetBits(SPI_PORT, SPI_CS_PIN)
#define SPI_CLK_LOW()   	  GPIO_ResetBits(SPI_PORT, SPI_CLK_PIN)   		
#define SPI_MISO_LOW()      GPIO_ResetBits(SPI_PORT, SPI_MISO_PIN)

#define SPI_MISO_READ       GPIO_ReadInputDataBit(SPI_PORT,SPI_MISO_PIN)

/* ** */
#define PinRead_ADSDI()			SPI_MISO_READ
#define	PinMode_ADSDI(x)	

#define PinWrite_ADCS(x)  	((x) ? SPI_CS_HIGH() : SPI_CS_LOW())
#define	PinMode_ADCS(x)	

#define PinWrite_ADSCLK(x)  ((x) ? SPI_CLK_HIGH() : SPI_CLK_LOW())
#define PinMode_ADSCLK(x)		

#define PinWrite_ADSDO(x)  	((x) ? SPI_MOSI_HIGH() : SPI_MOSI_LOW())
#define	PinMode_ADSDO(x)		


/* RN8302配置寄存器地址定义 */
#define		HFCONST1 		0x0100 
#define		IStart_P 	  0x0102
#define		IStart_Q  	0x0103
#define		LostVoltT 	0x0104
#define		ZXOT   			0x0105
#define		PRTH1L   		0x0106
#define		PHSIA    	  0x010F    
#define	  PHSIB    	  0x0110
#define   PHSIC    	  0x0111
#define   PHSIN 		  0x0112
#define		GSUA        0x0113 
#define		GSUB        0x0114 
#define		GSUC        0x0115 
#define		GSIA        0x0116 
#define		GSIB        0x0117 
#define		GSIC        0x0118
#define		IA_OS       0x0124 
#define		IB_OS       0x0125 
#define		IC_OS       0x0126
#define		GPA         0x0128 
#define		GPB         0x0129 
#define		GPC         0x012A
#define   GQA         0x012B 
#define   GQB         0x012C 
#define   GQC         0x012D
#define   GSA         0x012E 
#define   GSB         0x012F 
#define   GSC         0x0130
#define		PA_PHSL     0x0131 
#define		PB_PHSL     0x0132 
#define		PC_PHSL     0x0133
#define		QA_PHSL     0x0134 
#define		QB_PHSL     0x0135 
#define		QC_PHSL     0x0136
#define   PA_OS       0x0137 
#define   PB_OS       0x0138 
#define   PC_OS       0x0139
#define		CFCFG       0x0160
#define		EMUCFG      0x0161
#define		EMUCON      0x0162
#define		WSAVECON    0x0163
#define		CHECKSUM1   0x016A
#define		WREN   			0x0180
#define		WMSW     		0x0181 
#define		SOFTRST  		0x0182
#define		ADCCFG   		0x0183
#define		MODSEL      0x0186
#define		DeviceID   	0x018f

/*  RN8302计量寄存器地址定义 */
#define		UA 					0x0007
#define		UB 					0x0008
#define		UC 					0x0009
#define		USUM 				0x000A
#define   IA 					0x000B
#define   IB 					0x000C
#define   IC 					0x000D
#define   IN 					0x000E
#define   PA 					0x0014
#define   PB 					0x0015
#define   PC 					0x0016
#define   PT 					0x0017
#define   QA 					0x0018
#define   QB 					0x0019
#define   QC 					0x001A
#define   QT 					0x001B
#define   SA 					0x001C
#define   SB 					0x001D
#define   SC 					0x001E
#define   STA 				0x001F
#define   PFA 				0x0020
#define   PFB 				0x0021
#define   PFC 				0x0022
#define   PFTA 				0x0023
#define   EPA 				0x0030
#define   EPB 				0x0031
#define   EPC 				0x0032
#define   EPT 				0x0033
#define	  UFEQ				0x0057

void Delay_us(unsigned int n);
void Delay_ms(unsigned int n);

void Write_SPI_OneByte(unsigned char data);
unsigned char Read_SPI_OneByte(void);
void rn8302_WriteSpi(unsigned short Address, unsigned int dwData,unsigned char Date_len);
unsigned int rn8302_ReadSpi(unsigned short Address,unsigned char Data_len);

void rn8302_SpiConfig(void);
void rn8302_EmuInit(void);
void rn8302_ReadData(void);
void rn8302_ReadId(void);
void oled_display_circulate(void);

extern double Power_Data[11];

typedef struct{
	float        Ua,     Ub,     Uc;
	float        Ia,     Ib,     Ic;
	float        Pa,     Pb,     Pc,     Pt;
	float        Qa,     Qb,     Qc,     Qt;
	float        Sa,     Sb,     Sc,     St;
  float        PFa,    PFb,    PFc;
	float        UFeq;
	float        Energe;
}RN_DATA;
extern RN_DATA rn;

#endif
#endif
