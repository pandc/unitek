#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bsp.h"

#include "freertos.h"
#include "task.h"

#include "com.h"
#include "debug.h"
#include "tools.h"
#include "i2c.h"
#include "eeprom.h"
#include "eepmap.h"
#include "ad5934.h"
#include "meas.h"

#define PI		3.1415926535897932384626433832795

#define ADG715_SLAVE				0x4b
#define MEAS_SAMPLES				10

#define MIN_MIN_VALID_SAMPLES		(meas_samples/10)
#define DEFAULT_MIN_VALID_SAMPLES	16

#define DEFAULT_MEAS_SAMPLES		10

#define DEFAULT_FREQ				8000

#define AD5934_MCLK					16000000.0

#define MAX_RVAL_DELTA				20
#define MAX_IVAL_DELTA				20

static const char calib_items[] = "HMLP";

const float calib_resist[6] = {
	360000.0,
	16900.0,
	787.0,
	36.0,
	150.0,
	100.0
};

const float calib_condutt[4] = {
	1.0 / (360000.0 + RES_ADD),
	1.0 / (16900.0 + RES_ADD),
	1.0 / (787.0 + RES_ADD),
	1.0 / (36.0 + RES_ADD)
};


static const uint8_t adg715_cmd[CALIB_Items] = {
	0x24,
	0x82,
	0x12,
	0x01
};

static float real_values,imm_values,final_resist,kcell_factor,gcable;
static uint32_t ad5934_freq;
static int meas_samples = DEFAULT_MEAS_SAMPLES,curr_chan = -1;
static int16_t rsamples[MEAS_SAMPLES],isamples[MEAS_SAMPLES];
uint8_t mcpok;

struct meas_calpar_st mcp[CALIB_Items];

static uint32_t get_start_freq(uint32_t freq)
{
float f;

	f = (float)freq / (AD5934_MCLK/16);
	f *= (float)(1 << 27);
	return (uint32_t)f;
}

int set_meas_chan(int chan)
{
uint8_t data;

	if (chan >= CALIB_Items)
		return FALSE;
	if (chan == curr_chan)
		return TRUE;
	data = adg715_cmd[chan];
	if (!I2C_Write(ADG715_SLAVE,&data,1))
		return FALSE;
	vTaskDelay(kDec);
	return TRUE;
}

int meas_printInit(void)
{
int i;

	if (!mcpok)
		return FALSE;
	for (i = 0; i < 4; i++)
	{
		COM_Printf("\r\n*MEASINIT: %c,%.7e,%.7e,%.7e,%.7e,%.7e",calib_items[i],
			mcp[i].nch,mcp[i].ncl,mcp[i].gf,mcp[i].nos,mcp[i].sp);
	}
	COM_Printf("\r\n*MEASINIT: K,%.7e",kcell_factor);
	COM_Printf("\r\n*MEASINIT: F,%lu",ad5934_freq);
	COM_Printf("\r\n*MEASINIT: G,%.7e",gcable);
	COM_Puts("\r\n");
	return TRUE;
}

float meas_getkcell(void)
{
	if (mcpok)
		return kcell_factor;
	else
		return 1.0;
}

int meas_init(void)
{
int i;
float f;

	mcpok = FALSE;
	for (i = 0; i < 4; i++)
	{
		EEP_Read(EEP_HNch_addr+i*20,&f,4);
		if (isnan(f))
			return FALSE;
		EEP_Read(EEP_HNcl_addr+i*20,&f,4);
		if (isnan(f))
			return FALSE;
		EEP_Read(EEP_HGF_addr+i*20,&f,4);
		if (isnan(f))
			return FALSE;
		EEP_Read(EEP_HNos_addr+i*20,&f,4);
		if (isnan(f))
			return FALSE;
		EEP_Read(EEP_HPhase_addr+i*20,&f,4);
		if (isnan(f))
			return FALSE;
	}
	for (i = 0; i < 4; i++)
	{
		EEP_Read(EEP_HNch_addr+i*20,&mcp[i].nch,4);

		EEP_Read(EEP_HNcl_addr+i*20,&mcp[i].ncl,4);

		EEP_Read(EEP_HGF_addr+i*20,&mcp[i].gf,4);

		EEP_Read(EEP_HNos_addr+i*20,&mcp[i].nos,4);

		EEP_Read(EEP_HPhase_addr+i*20,&mcp[i].sp,4);
	}
	EEP_Read(EEP_KCell_addr,&kcell_factor,4);
	if (isnan(kcell_factor))
		kcell_factor = 1.0;

	EEP_Read(EEP_Freq_addr,&ad5934_freq,4);
	if (ad5934_freq == 0xffffffff)
		ad5934_freq = DEFAULT_FREQ;

	EEP_Read(EEP_GCable_addr,&gcable,4);
	if (isnan(gcable))
		gcable = 0.0;
	mcpok = TRUE;
	return TRUE;
}

int domeas(float *val,float *pphase)
{
int i,j,res = FALSE;
uint8_t buf[10],vbuf[10];
uint8_t cregh,cregl;
uint32_t stf = 0;

	cregh = ADGAIN_x1 + ADOV_2Vpp;		// pga gain = 1, out voltage range 2.0vpp
	cregl = ADCLK_Ext;					// External clock
	AD_WriteByte(ADREG_CntrlH,cregh | ADCMD_StandBy);	// stand by mode
	AD_WriteByte(ADREG_CntrlL,cregl | ADCREG_Reset);	// reset

	vTaskDelay(kCen);
	AD_WriteByte(ADREG_CntrlL,cregl);		// Reset off

	Dprintf(DBGLVL_Meas,"waiting for stand-by mode");
	for (i = 0; i < 10; i++)
	{
		*buf = 0;
		AD_Read(ADREG_CntrlH,buf,1);
		if (*buf == (cregh | ADCMD_StandBy))
			break;
		Dprintf(DBGLVL_Meas,"bad control-reg %02X",*buf);
		vTaskDelay(kDec);
	}
	if (i >= 10)
	{
		Dprintf(DBGLVL_Meas,"unable to talk with analog device");
		goto exi;
	}
	Dprintf(DBGLVL_Meas,"device in standby mode");

	*vbuf = 0;

	stf = get_start_freq(ad5934_freq);
	Dprintf(DBGLVL_Meas,"domeas: freq=%luHz start_freq=0x%lX",ad5934_freq,stf);
	
	store_dword(buf,stf,GS_BigEndian);		// 8KHz with 16MHz crystal
	AD_Write(ADREG_StartFreq,buf+1,3);			// start freq
	AD_Read(ADREG_StartFreq,vbuf+1,3);
	if (memcmp(buf+1,vbuf+1,3))
	{
		Dprintf(DBGLVL_Meas,"error verifying start freq (%lu)",get_dword(vbuf,GS_BigEndian));
		goto exi;
	}

	store_dword(buf,0,GS_BigEndian);
	AD_Write(ADREG_FreqInc,buf+1,3);	// freq incr
	AD_Read(ADREG_FreqInc,vbuf+1,3);
	if (memcmp(buf+1,vbuf+1,3))
	{
		Dprintf(DBGLVL_Meas,"error verifying freq incr (%lu)",get_dword(vbuf,GS_BigEndian));
		goto exi;
	}

	store_word(buf,1,GS_BigEndian);
	AD_Write(ADREG_IncNum,buf,2);	// number of incr
	AD_Read(ADREG_IncNum,vbuf,2);
	if (memcmp(buf,vbuf,2) != 0)
	{
		Dprintf(DBGLVL_Meas,"error verifying number of incr (%u)",get_word(vbuf,GS_BigEndian));
		goto exi;
	}

	store_word(buf,1,GS_BigEndian);
	AD_Write(ADREG_SettlingCycles,buf,2);		// number of settling time
	AD_Read(ADREG_SettlingCycles,vbuf,2);
	if (memcmp(buf,vbuf,2) != 0)
	{
		Dprintf(DBGLVL_Meas,"error verifying number of settling time (%u)",get_word(vbuf,GS_BigEndian));
		goto exi;
	}

	vTaskDelay(kCen);

	AD_WriteByte(ADREG_CntrlH,cregh | ADCMD_Init);
	
	vTaskDelay(kCen);
	//I2C_RandRead(AD5933_SLAVE,0x92,1,buf,2);		// temperature
	//Dprintf(DBGLVL_Meas,"temp=%u",get_word(buf,GS_BigEndian));

	memset(rsamples,0,sizeof(rsamples));
	memset(isamples,0,sizeof(isamples));
	imm_values = real_values = 0;
	for (j = i = 0; j < meas_samples; j++)
	{
		vTaskDelay(kCen);	// 93);
		AD_WriteByte(ADREG_CntrlH,cregh | ((j)? ADCMD_Repeat: ADCMD_Start));

		for (;;)
		{
			vTaskDelay(5);
			AD_Read(ADREG_Status,buf,1);
			if (*buf & ADST_ValidRIVal)
				break;
			AD_Read(ADREG_CntrlH,buf+1,1);
			Dprintf(DBGLVL_Meas,"bad status %02X (%02X)",*buf,buf[1]);
			vTaskDelay(kDec);
		}
		AD_Read(ADREG_RealData,buf,4);		// read real & immaginary values
		int16_t vr = get_word(buf,GS_BigEndian);
		int16_t vi = get_word(buf+2,GS_BigEndian);
		{
			rsamples[j] = vr;
			isamples[j] = vi;
			Dprintf(DBGLVL_Meas,"%3d. %6d %6d",j+1,vr,vi);
			real_values += (float)vr;
			imm_values += (float)vi;
		}
	}
	AD_WriteByte(ADREG_CntrlH,cregh | ADCMD_PowerDown);

	real_values /= (float)meas_samples;
	imm_values /= (float)meas_samples;
 	if (pphase)
	{       
                *pphase = atan((double)imm_values / (double)real_values);
		if ((real_values >= 0) && (imm_values < 0))
			*pphase = *pphase + 2 * PI ;
		else if (real_values < 0) 
			*pphase = *pphase + PI; 
		Dprintf(DBGLVL_Meas,"System phase: %.10e",*pphase);
	}
       

	res = TRUE;

	final_resist = sqrt(imm_values*imm_values+real_values*real_values);
	Dprintf(DBGLVL_Meas,"Resistance value: %.3f",final_resist);
exi:
	if (res)
	{
		if (val)
			*val = final_resist;
		Dprintf(DBGLVL_Meas,"domeas: measure ok");
	}
	else
		Dprintf(DBGLVL_Meas,"domeas: measure error");
	return res;
}

int meas(float *resist,float *condut,float *conduc)
{
int i;
float val,tphase;

	if (!mcpok)
		return FALSE;
	for (i = CALIB_Low; i >= CALIB_High; i--)
	{
		set_meas_chan(i);
		if (!domeas(&val,&tphase))
			return FALSE;
		Dprintf(DBGLVL_Meas,"meas: ncl=%.10e nch=%.10e nx=%.10e sp=%.10e tp=%.10e",mcp[i].ncl,mcp[i].nch,val,mcp[i].sp,tphase);
		if (val > mcp[i].ncl)
			break;
	}
	if (i == CALIB_Low)
	{
		if (val > mcp[i].nch)
		{
			Dprintf(DBGLVL_Meas,"meas: nx clamped to nch");
			val = mcp[i].nch;
		}
	}
	else if (i < CALIB_High)
	{
		i = CALIB_High;
		val = mcp[i].ncl;
		Dprintf(DBGLVL_Meas,"meas: nx clamped to ncl");
	}
	tphase -= mcp[i].sp;
	*condut=(((val - mcp[i].nos) * mcp[i].gf) * cos(tphase));
	*condut=*condut - gcable;
	*resist=(1.0 / *condut)-RES_ADD;
//	*resist = (1.0 / (((val - mcp[i].nos) * mcp[i].gf) * cos(tphase) - gcable)) - RES_ADD;
	*condut = 1.0 / *resist;
	*conduc = *condut / kcell_factor;
	return TRUE;
}

int meas_temp(float *resist)
{
float tphase,val;

	if (!mcpok)
		return FALSE;
	set_meas_chan(CALIB_Ptc);
	if (!domeas(&val,&tphase))
		return FALSE;
	Dprintf(DBGLVL_Meas,"meas_temp: nch=%.10e ncl=%.10e gf=%.10e nos=%.10e sp=%.10e",mcp[CALIB_Ptc].nch,mcp[CALIB_Ptc].ncl,mcp[CALIB_Ptc].gf,mcp[CALIB_Ptc].nos);
	tphase -= mcp[CALIB_Ptc].sp;
	//*resist = (1.0 / (((val - mcp[CALIB_Ptc].nos) * mcp[CALIB_Ptc].gf) * cos(tphase) - gcable)) - RES_ADD;
        *resist=((val-mcp[CALIB_Ptc].nos)*mcp[CALIB_Ptc].gf)*cos(tphase);
	return TRUE;
}
