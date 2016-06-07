#ifndef __AD5934_H
#define __AD5934_H

int AD_Write(uint8_t addr,void *data,uint8_t len);
int AD_WriteByte(uint8_t addr,uint8_t data);
int AD_Read(uint8_t addr,void *data,uint8_t len);

//#define gain_factor			1.36399e-6
#define M_PI					3.1415926535897932384626433832795

#define PHASE_ANGLE_TOLERANCE	1.5

#define ADOV_2Vpp		0
#define ADOV_200mVpp	(1 << 1)
#define ADOV_400mVpp	(2 << 1)
#define ADOV_1Vpp		(3 << 1)

#define ADGAIN_x5		0
#define ADGAIN_x1		1

#define ADCMD_Nop		0
#define ADCMD_Init		(1 << 4)
#define ADCMD_Start		(2 << 4)
#define ADCMD_Inc		(3 << 4)
#define ADCMD_Repeat	(4 << 4)
#define ADCMD_Temp		(9 << 4)
#define ADCMD_PowerDown	(0xa << 4)
#define ADCMD_StandBy	(0xb << 4)

#define ADCLK_Int		0
#define ADCLK_Ext		8

#define ADCREG_Reset	0x10

#define ADST_ValidTemp		1
#define ADST_ValidRIVal		2
#define ADST_FreqSweepDone	4

enum ADREG_Enum {
	ADREG_FirstReg = 0x80,
	ADREG_CntrlH = ADREG_FirstReg,
	ADREG_CntrlL,
	ADREG_StartFreq,
	ADREG_StartFreqM,
	ADREG_StartFreqL,
	ADREG_FreqInc,
	ADREG_FreqIncM,
	ADREG_FreqIncL,
	ADREG_IncNum,
	ADREG_IncNumL,
	ADREG_SettlingCycles,
	ADREG_SettlingCyclesL,
	ADREG_Dummy8C,
	ADREG_Dummy8D,
	ADREG_Dummy8E,
	ADREG_Status,
	ADREG_Dummy90,
	ADREG_Dummy91,
	ADREG_Temp,
	ADREG_TempL,
	ADREG_RealData,
	ADREG_RealDataL,
	ADREG_ImmData,
	ADREG_ImmDataH,
	ADREG_LastReg=ADREG_ImmDataH
};
#define ADREG_Items		(ADREG_LastReg-ADREG_FirstReg+1)

#endif
