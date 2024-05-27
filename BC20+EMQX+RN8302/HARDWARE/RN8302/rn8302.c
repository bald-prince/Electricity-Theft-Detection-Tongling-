#include <math.h>
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "rn8302.h"
#include "gpio.h"	
#include "Dl645_Types.h"
#include "config.h"
#include "spi.h"
#include "stm32f10x.h"

#define  CAL_OK

unsigned int Kvv, Kii, Kpp;
float Kv = 1.18254E-05; 
float Ki = 9.93411E-07;
float Kp = 9.85448E-05;
unsigned int   EC        = 6400;
unsigned short HFConst   = 1100;
unsigned short uGSUA     = 0x122;
unsigned short uGSUB     = 0x122;
unsigned short uGSUC     = 0x122;
unsigned short uGSIA     = 0xffbc;
unsigned short uGSIB     = 0xffbc;
unsigned short uGSIC     = 0xffbc;
unsigned short uPA_PHSL  = 0x100;
unsigned short uPB_PHSL  = 0x100;
unsigned short uPC_PHSL  = 0x100;
unsigned short uQA_PHSL  = 0x100;
unsigned short uQB_PHSL  = 0x100;
unsigned short uQC_PHSL  = 0x100;
unsigned short uIStart_P = 3424;
unsigned short uIStart_Q = 3424;
unsigned int checkSum  = 0;

RN_DATA rn;

void Delay_us(unsigned int n)
{
  unsigned short j;
	
  while(n--)
	{
    for(j=0;j<20;j++)  // 可调节20以调试各种电路的spi
		;
	}
}

/*
SPI1时钟由APB2时钟分频而来，可以选择2、4、8、16、32、64、128、256这几个分频系数。
而手册规定STM32的SPI时钟最快是18MHz。
对于STM32F103的SPI1接口时钟，由72M的PCLK2分频得到，所以分配系数大于等于4（72M/4 = 18M）。
对于STM32F103的SPI2/SPI3接口时钟，由36M的PCLK1分频得到，所以分配系数大于等于2（36M/2 = 18M）。
*/
void rn8302_SpiConfig(void)
{		
	#if SPI_SIMULATE_IO
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN | SPI_CS_PIN | SPI_CLK_PIN ;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	#else
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

  GPIO_InitStructure.GPIO_Pin = SPI_CLK_PIN | SPI_MOSI_PIN | SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
								  
  GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  SPI_CS_HIGH();
						  
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
	
	
	SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_TXE);
  SPI_I2S_ClearFlag(SPI1,SPI_I2S_FLAG_RXNE);
  SPI_Cmd(SPI1, ENABLE); 	
	#endif
}

#if SPI_SIMULATE_IO
void Write_SPI_OneByte(unsigned char data)
{
	unsigned char i = 0, bTemp = 0x80;

	for(i = 0; i<8; i++)
	{
		SPI_CLK_HIGH();
		delay_us(10) ;
		if(data & bTemp)
		{
			SPI_MOSI_HIGH();
		}
		else
			SPI_MOSI_LOW();

		bTemp >>= 1;
		delay_us(10);
		SPI_CLK_LOW();
		delay_us(10);
	}

	SPI_MOSI_LOW();
	delay_us(10) ;
}

unsigned char Read_SPI_OneByte(void)
{
	unsigned char i, data = 0;

	for(i = 0; i < 8; i++)
	{
		data <<= 1;
		SPI_CLK_HIGH();
		delay_us(1);
		SPI_CLK_LOW();
		delay_us(1);

		if(SPI_MISO_READ)
			data |= 1;
		else
			;
		delay_us(1);
	}
	
	return data;
}
#else

unsigned char SPI_ReadWriteByte(unsigned char data)		
{
#if 0
	unsigned char n=0;
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
	{
		n++;
		delay_us(1);
		if(n>100)
		{
			n=0;
			break;
		}
	}
  SPI_I2S_SendData(SPI1, data);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
	{
		n++;
		delay_us(1);
		if(n>100)
			break;
	}
  return SPI_I2S_ReceiveData(SPI1);    
#else
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
  SPI_I2S_SendData(SPI1, data);
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
  return SPI_I2S_ReceiveData(SPI1);   
#endif	
}

void Write_SPI_OneByte(unsigned char data)
{
  SPI_ReadWriteByte(data);
}

unsigned char Read_SPI_OneByte(void)
{
	return SPI_ReadWriteByte(0xFF);
}
#endif

unsigned int rn8302_ReadSpi(unsigned short Address,unsigned char Data_len)
{  
   unsigned char i, DTemp,CheckSumR;
   unsigned int drData;

   delay_us(2) ;
   SPI_CS_LOW();                        // 开启SPI传输
   
   DTemp = (unsigned char)(Address&0x00FF);
   CheckSumR = DTemp;
   Write_SPI_OneByte(DTemp);
   DTemp = (((unsigned char)(Address >> 4)) & 0xf0) ;
   CheckSumR += DTemp;
   Write_SPI_OneByte(DTemp);

   delay_us(5);
   drData = 0x00000000;                 // Read 24bit

   for(i=0;i<Data_len;i++)
   {
      DTemp = Read_SPI_OneByte();
      drData <<= 8;
      drData += DTemp;      
      CheckSumR += DTemp;
   }

   CheckSumR = ~CheckSumR;
   
   if (CheckSumR != Read_SPI_OneByte()) // 比较校验和
   {
      drData = 0xFFFFFFFF;
   }
   
   SPI_CS_HIGH();	                      // 关闭SPI传输
   delay_us(2);
   return drData;
}

void rn8302_WriteSpi(unsigned short Address, unsigned int dwData,unsigned char Date_len)
{
   unsigned char i,CheckSumR, DTemp;
	
   //SPI_CS_HIGH();
   //SPI_CLK_LOW();
   SPI_CS_LOW();	 										// 开启SPI传输

   DTemp = (unsigned char)(Address&0x00FF);
   CheckSumR = DTemp;
   Write_SPI_OneByte(DTemp);					// 写地址
   DTemp = (((unsigned char)(Address >> 4)) & 0xf0)+0x80 ;
   CheckSumR += DTemp;
   Write_SPI_OneByte(DTemp);					// 写命令

   for (i = 0 ;i < Date_len;i++)
   {
      DTemp = (unsigned char)(dwData>>(Date_len-1-i)*8);
      Write_SPI_OneByte(DTemp);				// 写命令
      CheckSumR += DTemp;     
   }

   CheckSumR = ~CheckSumR;
   Write_SPI_OneByte(CheckSumR);			// 写命令
   SPI_CS_HIGH();	    								// 结束传输
   delay_us(2);
}


void rn8302_ReadId(void)
{
	unsigned int tmp32 = 0;

	tmp32 = rn8302_ReadSpi(DeviceID,3); // 芯片ID
	if(tmp32 != 0x00830200)             
	{
		printf("rn8302 dev id erro\n");
		return;
	}
	else{
		printf("rn8302 dev id ok\n");
	}
}

void rn8302_EmuInit(void)
{
	unsigned short tmp16 = 0;
	unsigned int tmp32 = 0;
	
	Kvv = 1 / Kv; 
	Kii = 1 / Ki; 
	Kpp = 1 / Kp;     // 小数转换为整数
	
	tmp32 = rn8302_ReadSpi(DeviceID,3); // 芯片ID
	if(tmp32 != 0x00830200)             
	{
		printf("rn8302 dev id erro\n");
		return;
	}
	else{
		printf("rn8302 dev id ok\n");
	}

	rn8302_WriteSpi(WREN,0xE5,1);        // 使能写操作
	rn8302_WriteSpi(WMSW,0xA2,1); 			 // 工作模式EMM
	rn8302_WriteSpi(SOFTRST,0xFA,1);     // 复位
	delay_us(1000);
	rn8302_WriteSpi(WREN,0xE5,1);        // 使能写操作
	rn8302_WriteSpi(WMSW,0xA2,1); 			 // 工作模式EMM
	
	rn8302_WriteSpi(HFCONST1,HFConst,2); // HFCONST1
	printf("HFCONST1=%d\n",  rn8302_ReadSpi(HFCONST1,2));
	
	/*电压增益*/ 
	rn8302_WriteSpi(GSUA,uGSUA,2); 
	rn8302_WriteSpi(GSUB,uGSUB,2); 
	rn8302_WriteSpi(GSUC,uGSUC,2); 
	
	/*电流增益*/ 
	rn8302_WriteSpi(GSIA,uGSIA,2); 
	rn8302_WriteSpi(GSIB,uGSIB,2); 
	rn8302_WriteSpi(GSIC,uGSIC,2); 
	
	/*P功率增益*/ 
	rn8302_WriteSpi(GPA,uPA_PHSL,2); 
	rn8302_WriteSpi(GPB,uPB_PHSL,2); 
	rn8302_WriteSpi(GPC,uPC_PHSL,2); 
	/*Q功率增益*/ 
	rn8302_WriteSpi(GQA,uQA_PHSL,2); 
	rn8302_WriteSpi(GQB,uQB_PHSL,2); 
	rn8302_WriteSpi(GQC,uQC_PHSL,2); 	
	
	/*小电流offset*/ 
	//rn8302_WriteSpi(IA_OS,0x0D6F,2); 
	//rn8302_WriteSpi(IB_OS,0x0DCA,2); 
	//rn8302_WriteSpi(IC_OS,0x0D4E,2); 
	/*失压阈值寄存器*/ 
	//rn8302_WriteSpi(LostVoltT,0x0400,2);
	/*过零阈值寄存器*/ 
	//rn8302_WriteSpi(ZXOT,0x002c,2);
	/*CF管脚配置*/ 
	//rn8302_WriteSpi(CFCFG,0x043210,3);
	/*写缓存*/ 
	//rn8302_WriteSpi(WSAVECON,0x10,1);
	/*计量单元配置寄存器*/ 
	tmp32 = (1<<22) | (1<<20); // 有功累加方式，（读后不清0和读后清0都是OK的，根据个人喜好设置）
	rn8302_WriteSpi(EMUCFG,tmp32,3);
	
	/* 通道模拟增益,＝2’b00，1 倍增益；＝2’b01，2 倍增益；＝2’b10，4 倍增益；＝2’b11，8 倍增益；默认为 2’b00。
	           PGAUA    PGAUB    PGAUC    PGAIA    PGAIB    PGAIC     PGAIN                                */
	//tmp16 = (1<<0) | (1<<2) | (1<<4) | (1<<6) | (1<<8) | (1<<10) | (1<<12);
	tmp16 = 0;
	rn8302_WriteSpi(ADCCFG,tmp16,2); 
	printf("tmp16=0x%x, REGCFG=0x%x\n", tmp16, rn8302_ReadSpi(ADCCFG,2));

	/*有功启动电流阈值*/ 
	rn8302_WriteSpi(IStart_P,uIStart_P,2);
	/*无功启动电流阈值*/ 
	rn8302_WriteSpi(IStart_Q,uIStart_Q,2);
	
	/*三相四线*/ 
	rn8302_WriteSpi(MODSEL,0,1);
	/*计量控制位*/ 
	rn8302_WriteSpi(EMUCON,0x777777,3);
	/* 关闭写操作 */
	rn8302_WriteSpi(WREN,0xDC,1); 			    
	
  checkSum = rn8302_ReadSpi(CHECKSUM1,3);
	printf("CHECKSUM1=0x%x\n", checkSum);   // 3a215c
}

double Power_Data[11] = {0.0};
void rn8302_ReadData(void)
{
  unsigned int tmp32 = 0;

	unsigned int Ua_REG, Ub_REG, Uc_REG;
	//float        Ua,     Ub,     Uc;
	unsigned int Ia_REG, Ib_REG, Ic_REG;
	//float        Ia,     Ib,     Ic;
	unsigned int Pa_REG, Pb_REG, Pc_REG, Pt_REG; // Pt: P total
	//float        Pa,     Pb,     Pc,     Pt;
	unsigned int Qa_REG, Qb_REG, Qc_REG, Qt_REG;
	//float        Qa,     Qb,     Qc,     Qt;
	unsigned int Sa_REG, Sb_REG, Sc_REG, St_REG;
	//float        Sa,     Sb,     Sc,     St;
	unsigned int PFa_REG,PFb_REG,PFc_REG,PFt_REG;
  //float        PFa,    PFb,    PFc;
	unsigned int UFeq_REG; 
	//float        UFeq;
	unsigned int Energe_REG;
	//float        Energe = 0.0;
	
	tmp32 = rn8302_ReadSpi(CHECKSUM1,3);//校表寄存器效验和1
	if(tmp32 != checkSum)
	{
		rn8302_EmuInit();
		printf("checkSum erro\n");
		return;
	}
	delay_us(10);

	/* 读取电压值,单位V */
	Ua_REG = rn8302_ReadSpi(UA,4);
	Ub_REG = rn8302_ReadSpi(UB,4);
	Uc_REG = rn8302_ReadSpi(UC,4);
	printf("Ua_REG=0x%x  Ub_REG=0x%x  Uc_REG=0x%x\r\n", Ua_REG, Ub_REG, Uc_REG);
	rn.Ua = Ua_REG*Kv;  
	rn.Ub = Ub_REG*Kv;  
	rn.Uc = Uc_REG*Kv; 
	printf("Ua=%3.4f  Ub=%3.4f  Uc=%3.4f\r\n", rn.Ua, rn.Ub, rn.Uc);
	
	/* 读取电流值，单位A */ 
	Ia_REG = rn8302_ReadSpi(IA,4);
	Ib_REG = rn8302_ReadSpi(IB,4);
	Ic_REG = rn8302_ReadSpi(IC,4);
	printf("Ia_REG=0x%x  Ib_REG=0x%x  Ic_REG=0x%x\r\n", Ia_REG, Ib_REG, Ic_REG);
	rn.Ia = Ia_REG*Ki;  
	rn.Ib = Ib_REG*Ki;  
	rn.Ic = Ic_REG*Ki; 
	printf("Ia=%3.4f  Ib=%3.4f  Ic=%3.4f\r\n", rn.Ia, rn.Ib, rn.Ic);

	/* 读取有功功率，单位W */ 
	Pa_REG = rn8302_ReadSpi(PA,4);  
	Pb_REG = rn8302_ReadSpi(PB,4);  
	Pc_REG = rn8302_ReadSpi(PC,4); 
  Pt_REG = rn8302_ReadSpi(PT,4); 
	printf("Pa_REG=0x%x  Pb_REG=0x%x  Pc_REG=0x%x  Pt_REG=0x%x\r\n", Pa_REG, Pb_REG, Pc_REG, Pt_REG);	
	if (Pa_REG & 0x80000000)
		Pa_REG = 0xFFFFFFFF-Pa_REG;//Pa_REG = Pa_REG*(-1);
	if (Pb_REG &0x80000000)
		Pb_REG = 0xFFFFFFFF-Pb_REG;//Pb_REG = Pb_REG*(-1);
	if (Pc_REG &0x80000000)
		Pc_REG = 0xFFFFFFFF-Pc_REG;//Pc_REG = Pc_REG*(-1);
	if (Pt_REG &0x80000000)
		Pt_REG = 0xFFFFFFFF-Pt_REG;//Pt_REG = Pt_REG*(-1);	
	rn.Pa = Pa_REG*Kp;  
	rn.Pb = Pb_REG*Kp;  
	rn.Pc = Pc_REG*Kp; 
	rn.Pt = Pt_REG*Kp; 
	printf("Pa=%f  Pb=%f  Pc=%f  Pt=%f\r\n", rn.Pa, rn.Pb, rn.Pc, rn.Pt);
	
	/* 读取无功功率，单位Var */ 
	Qa_REG = rn8302_ReadSpi(QA,4);  
	Qb_REG = rn8302_ReadSpi(QB,4);  
	Qc_REG = rn8302_ReadSpi(QC,4); 
	Qt_REG = rn8302_ReadSpi(QT,4);  
	printf("Qa_REG=0x%x  Qb_REG=0x%x  Qc_REG=0x%x  Qt_REG=0x%x\r\n", Qa_REG, Qb_REG, Qc_REG, Qt_REG);	
	if (Qa_REG &0x80000000) Qa_REG = 0xFFFFFFFF-Qa_REG;
	if (Qb_REG &0x80000000) Qb_REG = 0xFFFFFFFF-Qb_REG;
	if (Qc_REG &0x80000000) Qc_REG = 0xFFFFFFFF-Qc_REG;
	if (Qt_REG &0x80000000) Qt_REG = 0xFFFFFFFF-Qt_REG;
	rn.Qa = Qa_REG*Kp;  
	rn.Qb = Qb_REG*Kp;  
	rn.Qc = Qc_REG*Kp; 
	rn.Qt = Qt_REG*Kp; 
	printf("Qa=%6.2f  Qb=%6.2f  Qc=%6.2f  Qt=%6.2f\r\n", rn.Qa, rn.Qb, rn.Qc, rn.Qt);
	
	/* 读取视在功率 符号位始终为0 */ 
	Sa_REG = rn8302_ReadSpi(SA,4);  
	Sb_REG = rn8302_ReadSpi(SB,4);  
	Sc_REG = rn8302_ReadSpi(SC,4); 
	St_REG = rn8302_ReadSpi(STA,4); 
	printf("Sa_REG=0x%x  Sb_REG=0x%x  Sc_REG=0x%x  St_REG=0x%x\r\n", Sa_REG, Sb_REG, Sc_REG, St_REG);	
	rn.Sa = Sa_REG*Kp;
	rn.Sb = Sb_REG*Kp;
	rn.Sc = Sc_REG*Kp;
	rn.St = St_REG*Kp;
	printf("Sa=%6.2f  Sb=%6.2f  Sc=%6.2f St=%6.2f\r\n", rn.Sa, rn.Sb, rn.Sc, rn.St);

	/* 读取功率因数 */ 
	PFa_REG = rn8302_ReadSpi(PFA,3);  
	PFb_REG = rn8302_ReadSpi(PFB,3);  
	PFc_REG = rn8302_ReadSpi(PFC,3); 
	PFt_REG = rn8302_ReadSpi(PFTA,3);
	printf("PFa_REG=0x%x  PFb_REG=0x%x  PFc_REG=0x%x  PFt_REG=0x%x\r\n", PFa_REG, PFb_REG, PFc_REG, PFt_REG);
	if (PFa_REG & 0x800000) PFa_REG = 0x00FFFFFF-PFa_REG;
	if (PFb_REG & 0x800000) PFb_REG = 0x00FFFFFF-PFb_REG;
	if (PFc_REG & 0x800000) PFc_REG = 0x00FFFFFF-PFc_REG;
	if (PFt_REG & 0x800000) PFt_REG = 0x00FFFFFF-PFt_REG;     
	rn.PFa = PFa_REG/8388608.0;  
	rn.PFb = PFb_REG/8388608.0; 
	rn.PFc = PFc_REG/8388608.0;
	printf("PFa=%f  PFb=%f  PFc=%f\n", rn.PFa, rn.PFb, rn.PFc);
	
	/* 电压线频率*/
	UFeq_REG = rn8302_ReadSpi(UFEQ,3);
	rn.UFeq = 8192000.0*8/UFeq_REG;
	rn.UFeq = (rn.UFeq>60 || rn.UFeq<40 ) ? 50 : rn.UFeq;
	printf("UFeq=%2.4f\r\n", rn.UFeq);
	
	/* 读有功电量 */
	Energe_REG = rn8302_ReadSpi(EPT,3);
	rn.Energe = Energe_REG*1.0/EC;
	printf("Energe_REG=%d  Energe=%2.4f \n", Energe_REG, rn.Energe);
	
	Power_Data[0] = rn.Ua;
	Power_Data[1] = rn.Ub;
	Power_Data[2] = rn.Uc;
	Power_Data[3] = rn.Ia;
	Power_Data[4] = rn.Ib;
	Power_Data[5] = rn.Ic;
	Power_Data[6] = rn.Pa;
	Power_Data[7] = rn.Pb;
	Power_Data[8] = rn.Pc;
	Power_Data[9] = rn.UFeq;
	Power_Data[10] = rn.Energe;
}

