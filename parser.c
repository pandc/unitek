#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "bsp.h"
#include "types.h"
#include "com.h"
#include "tools.h"

#include "ffile.h"
#include "config.h"

#include "freertos.h"
#include "task.h"

#include "power.h"
#include "backup.h"
#include "datetime.h"
#include "debug.h"
#include "io.h"
#include "i2c.h"
#include "ad5934.h"
#include "meas.h"
#include "eeprom.h"
#include "eepmap.h"
#include "display_128x64.h"
#include "fontstypes.h"
#include "ymodem.h"
#include "version.h"
#include "parser.h"

#if !defined(COM_NONE)

enum Port_Enum {
	PortA,
	PortB,
	PortC,
	PortD,
	PortE,
	PortLast
};

static GPIO_TypeDef *PortRegs[] = { GPIOA,GPIOB,GPIOC,GPIOD,GPIOE };

static GPIOMode_TypeDef PortModes[] = {
	GPIO_Mode_IN_FLOATING,
	GPIO_Mode_IPD,
	GPIO_Mode_IPU,
	GPIO_Mode_Out_OD,
	GPIO_Mode_Out_PP
};

static const char *PortModesStr[] = {
	"IFL",
	"IPD",
	"IPU",
	"OOD",
	"OPP",
	NULL
};

#define MAX_PWD_FAILS		6
#define DEFAULT_PASSWORD	"unitek"
uint8_t parser_status;
static uint8_t pwd_fails;

typedef int (*CommandCallback)(char *arg);
struct CommandSt {
	const char *prefix;
	int args;
	int reserved;
	CommandCallback callback;
};

static uint8_t answer,busy;

enum CALLBACKRET_Enum {
	CALLBACKRET_Ok,
	CALLBACKRET_Error,
	CALLBACKRET_Ignore
};

#define PBUF_SIZE		750
static char *pbuf;

/* Includes and define -------------------------------------------------------*/
// Defined Command interpreter 
#define	RETURN			'\r'
#define BACK_SPACE  	'\b'
#define ESCAPE      	0x1b
#define CTRL_E     		0x05
#define	CTRL_G			0x07
#define	CTRL_S			0x13
#define	CTRL_T			0x14
#define	CTRL_R			0x12
#define	CTRL_V			0x16
#define	CTRL_Z			0x1A
#define	SPACE			0x20
#define	ACK				0x06
#define	NAK				0x15

static void parser_printbanner(void);
static void parser_end(uint8_t se,uint8_t ans);

static int parser_ad_write(char *arg);
static int parser_ad_read(char *arg);
static int parser_backup(char *arg);
static int parser_calib(char *arg);
static int parser_date(char *arg);
static int parser_debug(char *arg);

static int parser_dcat(char *arg);
static int parser_dcopy(char *arg);
static int parser_ddir(char *arg);
static int parser_ddel(char *arg);
static int parser_dformat(char *arg);
static int parser_dmove(char *arg);
static int parser_dwrite(char *arg);
static int parser_file_check(char *arg);

static int parser_devid(char *arg);
static int parser_devsz(char *arg);
static int parser_display(char *arg);
static int parser_eeprom_read(char *arg);
static int parser_eeprom_write(char *arg);
static int parser_heap(char *arg);
static int parser_help(char *arg);
static int parser_i2cread(char *arg);
static int parser_i2cwrite(char *arg);
static int parser_ini(char *arg);
static int parser_login(char *arg);
static int parser_logout(char *arg);
static int parser_meas(char *arg);
static int parser_pinconf(char *arg);
static int parser_pinread(char *arg);
static int parser_pinwrite(char *arg);
static int parser_reset(char *arg);
static int parser_resetp(char *arg);
static int parser_sn(char *arg);
static int parser_sn32(char *arg);
static int parser_tasks(char *arg);
static int parser_time(char *arg);
static int parser_tz(char *arg);
static int parser_utime(char *arg);
static int parser_version(char *arg);
static int parser_ymodem(char *arg);

const struct CommandSt commandsList[] = {
	{ "AR=", TRUE, TRUE, parser_ad_read },
	{ "AW=", TRUE, TRUE, parser_ad_write },
	{ "BACKUP", TRUE, TRUE, parser_backup },
	{ "CAL", TRUE, TRUE, parser_calib },
	{ "DATE", TRUE, TRUE, parser_date },
	{ "DBG", TRUE, TRUE, parser_debug },

	{ "DCAT=", TRUE, TRUE, parser_dcat },
	{ "DCOPY=", TRUE, TRUE, parser_dcopy },
	{ "DDIR", FALSE, TRUE, parser_ddir },
	{ "DDEL=", TRUE, TRUE, parser_ddel },
	{ "DFORMAT", FALSE, TRUE, parser_dformat },
	{ "DFILECHK=", TRUE, TRUE, parser_file_check },
	{ "DMOVE=", TRUE, TRUE, parser_dmove },
	{ "DWRITE=", TRUE, TRUE, parser_dwrite },

	{ "DEVID", FALSE, TRUE, parser_devid },
	{ "DEVSZ", FALSE, TRUE, parser_devsz },
	{ "DISPLAY", TRUE, TRUE, parser_display },
	{ "ER=", TRUE, TRUE, parser_eeprom_read },
	{ "EW=", TRUE, TRUE, parser_eeprom_write },
	{ "HEAP", FALSE, TRUE, parser_heap },
	{ "HELP", FALSE, FALSE, parser_help },
	{ "IR=", TRUE, TRUE, parser_i2cread },
	{ "IW=", TRUE, TRUE, parser_i2cwrite },
	{ "INI=", TRUE, TRUE, parser_ini },
	{ "LOGIN=", TRUE, FALSE, parser_login },
	{ "LOGOUT", FALSE, FALSE, parser_logout },
	{ "MEAS", TRUE, TRUE, parser_meas },
	{ "PC", TRUE, TRUE, parser_pinconf },
	{ "PR", TRUE, TRUE, parser_pinread },
	{ "PW", TRUE, TRUE, parser_pinwrite },
	{ "RESET=", TRUE, TRUE, parser_resetp },
	{ "RESET", FALSE, TRUE, parser_reset },
	{ "SN", FALSE, TRUE, parser_sn },
	{ "SN32", FALSE, TRUE, parser_sn32 },
#if (configUSE_TRACE_FACILITY)
	{ "TASKS", FALSE, TRUE, parser_tasks },
#endif
	{ "TIME", FALSE, TRUE, parser_time },
	{ "TZ", FALSE, TRUE, parser_tz },
	{ "UTIME", TRUE, TRUE, parser_utime },
	{ "VER", FALSE, FALSE, parser_version },
	{ "YMODEM=", TRUE, TRUE, parser_ymodem },
	{ NULL }
};

static uint8_t manageIni(char *filename,char *input)
{
#define MAX_INI_VOICES	8
char *header;
char *tags[MAX_INI_VOICES];
char *values[MAX_INI_VOICES];
char *p,*q,i;

    if (*input != '[')
        return FALSE;
    header = input+1;
    if ((input = strchr(input,']')) == NULL)
        return FALSE;
    *input++ = 0;
    for (i = 0; (i < MAX_INI_VOICES) && ((p = strtok(input,",")) != NULL); i++,input = NULL)
    {
        tags[i] = p;
        if ((q = strchr(p,'=')) != NULL)
        {
            *q = 0;
            values[i] = q+1;
        }
        else
            values[i] = NULL;
    }
    return setConfigMultiVoice(filename,header,(const char **)tags, (const char **)values,i);
}

static int spb_file_check(const char *fn)
{
FFILE *fp;
uint32_t flen,fidx,fcrc,crc;
int res = FALSE;
uint16_t i;

	fp = ffopen(fn,'r');
	if (!fp)
		goto exi;

	flen = ffsize(fp);
	if (flen < (128+256))
		goto exi;
	ffread(fp,&fidx,sizeof(fidx));
	if ((fidx+128) != flen)
		goto exi;
	ffread(fp,&fcrc,sizeof(fcrc));

	ffseek(fp,128,FSEEK_SET);
	flen -= 128;
	for (crc = fidx = 0; fidx < flen;)
	{
		ffread(fp,pbuf,PBUF_SIZE);
		for (i = 0; (fidx < flen) && (i < PBUF_SIZE); i++,fidx++)
			crc += pbuf[i];
	}
	if (crc == fcrc)
		res = TRUE;
exi:
	if (fp)
		ffclose(fp);
	return res;
}

int parser_display(char* arg)
{
	if (strcmpNoCase(arg,"=INIT"))
	{
		LCD_Display_Setup();
		FontPointerInit();
		SelectFont(CALIBRI_10);
		return CALLBACKRET_Ok;
	}
	else if (strcmpNoCase(arg,"=CLEAR"))
	{
		LCD_Clear();
		return CALLBACKRET_Ok;
	}
	else if (strcmpNoCase(arg,"=FULL"))
	{
		LCD_Fill_GRAM(0xff);
		return CALLBACKRET_Ok;
	}
	else if ((*arg == '=') && arg[1])
	{
		char *s = strtok(arg+1,",");
		uint16_t x,y;
		if (!s || !*s || !isdigits(s,0))
			return CALLBACKRET_Error;
		x = atoi(s);
		if (x >= 128)
			return CALLBACKRET_Error;
		s = strtok(NULL,",");
		if (!s || !*s || !isdigits(s,0))
			return CALLBACKRET_Error;
		y = atoi(s);
		s = strtok(NULL,",");
		if (!s || !*s)
			return CALLBACKRET_Error;
		LCDPrintString(s,x,y);
		LCD_CopyScreen();
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_file_check(char *arg)
{
	if (spb_file_check(arg))
		return CALLBACKRET_Ok;
	else
		return CALLBACKRET_Error;
}

static int parser_ad_read(char *arg)
{
char *s;
uint16_t addr,len,i;

	s = strtok(arg,",");
	if (!s || !*s || !getHexWord(s,&addr,0) || (addr < ADREG_FirstReg) || (addr > ADREG_LastReg))
		return CALLBACKRET_Error;
	s = strtok(NULL,",");
	if (!s || !*s || !getHexWord(s,&len,0) || !len || (len > ADREG_Items))
		return CALLBACKRET_Error;
	if ((addr+len) > (ADREG_LastReg+1))
		return CALLBACKRET_Error;
	if (strtok(NULL,","))
		return CALLBACKRET_Error;

	if (!AD_Read((uint8_t)addr,pbuf,(uint8_t)len))
		return CALLBACKRET_Error;

	for (i = 0; i < len; i++)
	{
		if ((i & 15) == 0)
			COM_Printf("\r\n%03X ",i+addr);
		COM_Printf(" %02X",pbuf[i]);
	}
	COM_Puts("\r\n");
	return CALLBACKRET_Ok;
}

static int parser_ad_write(char *arg)
{
char *s;
uint16_t addr,len;

	s = strtok(arg,",");
	if (!s || !*s || !getHexWord(s,&addr,0) || (addr < ADREG_FirstReg) || (addr > ADREG_LastReg))
		return CALLBACKRET_Error;
	len = 0;
	while (s = strtok(NULL,","))
	{
		if (!s || !*s || !getHexByte(s,(uint8_t *)pbuf+len,0))
			break;
		len++;
	}
	if (s || !len || (len > ADREG_Items) || ((addr+len) > (ADREG_LastReg+1)))
		return CALLBACKRET_Error;
	s = pbuf;
	while ((addr <= ADREG_CntrlL) && len)
	{
		if (!AD_WriteByte((uint8_t)addr,*s))
			return CALLBACKRET_Error;
		addr++;
		len--;
		s++;
	}
	if (len && !AD_Write(addr,s,len))
		return CALLBACKRET_Error;
	return CALLBACKRET_Ok;
}

static int parser_backup(char *arg)
{
uint16_t i;

	if ((*arg == '?') && !arg[1])
	{
		for (i = 0; i < BKP_DR_NUMBER; i++)
		{
			if (!(i & 15))
				COM_Printf("\r\n%02X ",i);
			COM_Printf(" %04X",BKP_getWord(i));
		}
		COM_Puts("\r\n");
		return CALLBACKRET_Ok;
	}
	else if ((*arg == '=') && arg[1])
	{
		char *s;
		uint16_t w;
		arg++;
		s = strsep(&arg,',');
		if (!s || !getHexWord(s,&i,strlen(s)) || (i >= BKP_DR_NUMBER))
			return CALLBACKRET_Error;
		while (((s = strsep(&arg,',')) != NULL) && getHexWord(s,&w,strlen(s)))
		{
			BKP_storeWord(i,w);
			if (++i >= BKP_DR_NUMBER)
				i = 0;
		}
		if (!s)
			return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_calib(char *arg)
{
int item,i;
float res[2],sp;
uint8_t c;
uint32_t freq = 0;

	if (*arg == '=')
	{
		switch(toupper(arg[1]))
		{
		case 'H':
			item = CALIB_High;
			break;
		case 'M':
			item = CALIB_Medium;
			break;
		case 'L':
			item = CALIB_Low;
			break;
		case 'P':
			item = CALIB_Ptc;
			break;
		case 'G':
			if (!mcpok)
			{
				COM_Puts("\r\nInvalid calibration parameters\r\n");
				return CALLBACKRET_Error;
			}
			item = CALIB_GCable;
			break;
		case 'R':
			if (!mcpok)
			{
				COM_Puts("\r\nInvalid calibration parameters\r\n");
				return CALLBACKRET_Error;
			}
			item = CALIB_RCable;
			break;
		case 'K':
			COM_Puts("\r\nEnter kcell: ");
			i = 0;
			for (;;)
			{
				if (!COM_Getch(&c))
					return CALLBACKRET_Error;
				if ((c == '.') || isdigit(c) && (i < 50))
				{
					pbuf[i++] = c;
					COM_Echo(c);
				}
				else if ((c == BACK_SPACE) && i)
				{
					i--;
					COM_Echo(BACK_SPACE);
					COM_Echo(' ');
					COM_Echo(BACK_SPACE);
				}
				else if (c == '\r')
					break;
			}
			if (!i)
				return CALLBACKRET_Error;
			pbuf[i] = 0;
			res[0] = atof(pbuf);
			if (res[0] == 0.0)
				return CALLBACKRET_Error;
			EEP_Write(EEP_KCell_addr,res,4);
			return CALLBACKRET_Ok;
		case 'F':
			COM_Puts("\r\nEnter ad5934 freq (Hz): ");
			i = 0;
			for (;;)
			{
				if (!COM_Getch(&c))
					return CALLBACKRET_Error;
				if (isdigit(c) && (i < 50))
				{
					pbuf[i++] = c;
					COM_Echo(c);
				}
				else if ((c == BACK_SPACE) && i)
				{
					i--;
					COM_Echo(BACK_SPACE);
					COM_Echo(' ');
					COM_Echo(BACK_SPACE);
				}
				else if (c == '\r')
					break;
			}
			if (!i)
				return CALLBACKRET_Error;
			pbuf[i] = 0;
			freq = atodw(pbuf);
			if (freq  == 0)
				return CALLBACKRET_Error;
			EEP_Write(EEP_Freq_addr,&freq,4);
			return CALLBACKRET_Ok;
		default:
			return CALLBACKRET_Error;
			break;
		}
		COM_Puts("\r\n");
		if (item == CALIB_RCable)
		{
			COM_Puts(">>> Connect reference resistance of 50 Ohm and press enter (ESC to abort)\r\n");
			for (;;)
			{
				if (COM_Getch(&c))
				{
					if ((c == '\r') || (c == 27))
						break;
				}
				else
					return CALLBACKRET_Error;
			}
			if (c != '\r')
				return CALLBACKRET_Error;
			if (!domeas(CALIB_Low,res,&sp))
			{
				COM_Puts("Measure error\r\n");
				return CALLBACKRET_Error;
			}
			sp -= mcp[CALIB_Low].sp;
			float gt = ((res[0] - mcp[CALIB_Low].nos) * mcp[CALIB_Low].gf) * cos(sp);
			float rc = (1.0 / gt) - RES_ADD - 50;
			if (rc < 0.0)
				rc = 0.0;
			COM_Printf("GT=%.10e RC%.10e=\r\n",gt,rc);
			EEP_Write(EEP_RCable_addr,&rc,4);
			return CALLBACKRET_Ok;
		}
		if (item == CALIB_GCable)
		{
			COM_Puts(">>> Connect reference resistance of 100KOhm and press enter (ESC to abort)\r\n");
			for (;;)
			{
				if (COM_Getch(&c))
				{
					if ((c == '\r') || (c == 27))
						break;
				}
				else
					return CALLBACKRET_Error;
			}
			if (c != '\r')
				return CALLBACKRET_Error;
			if (!domeas(CALIB_High,res,&sp))
			{
				COM_Puts("Measure error\r\n");
				return CALLBACKRET_Error;
			}
			sp -= mcp[CALIB_High].sp;
			float gx = ((res[0] - mcp[CALIB_High].nos) * mcp[CALIB_High].gf) * cos(sp);
			float gc =gx-(1.0 / 100300.0);
			COM_Printf("gx=%.10e G(100300Ohm)=%.10e GCable=%.10e\r\n",gx,1.0 / 100300.0,gc);
			EEP_Write(EEP_GCable_addr,&gc,4);
			return CALLBACKRET_Ok;
		}
		if (item == CALIB_Ptc)
		{
			COM_Puts(">>> The 2 jumpers are numbered starting from the inner one <<<\r\n\r\n");
			for (i = 0; i < 2; i++)
			{
				COM_Printf("Connect jumper #%d (%.1fOhm) and press enter (ESC to abort)\r\n",i+1,calib_resist[item+i+1]);
				for (;;)
				{
					if (COM_Getch(&c))
					{
						if ((c == '\r') || (c == 27))
							break;
					}
					else
						return CALLBACKRET_Error;
				}
				if (c != '\r')
					return CALLBACKRET_Error;
				if (!domeas(CALIB_Ptc,res+i,(i == 1)? NULL:&sp))
				{
					COM_Puts("Measure error\r\n");
					return CALLBACKRET_Error;
				}
				if (i == 1)
				{
					float rh = calib_resist[item+1];
					float rl = calib_resist[item+2];
					float nh = res[0];
					float nl = res[1];
					float gf = (rh - rl) / (nh - nl);
					float nos = nh - (rh / gf);
					COM_Printf("Nch=%.3f Ncl=%.3f GF=%.10e Nos=%.10e sp=%.10e\r\n",
						nh,
						nl,
						gf,
						nos,
						sp);
					EEP_Write(EEP_PNch_addr,&nh,sizeof(float));
					EEP_Write(EEP_PNcl_addr,&nl,sizeof(float));
					EEP_Write(EEP_PGF_addr,&gf,sizeof(float));
					EEP_Write(EEP_PNos_addr,&nos,sizeof(float));
					EEP_Write(EEP_PPhase_addr,&sp,sizeof(float));
					return CALLBACKRET_Ok;
				}
			}
			return CALLBACKRET_Error;
		}
		COM_Puts(">>> The 4 jumpers are numbered starting from the inner one <<<\r\n\r\n");
		for (i = 0; i < 2; i++)
		{
			COM_Printf("Connect jumper #%d (%.1fOhm) and press enter (ESC to abort)\r\n",item+i+1,calib_resist[item+i]);
			for (;;)
			{
				if (COM_Getch(&c))
				{
					if ((c == '\r') || (c == 27))
						break;
				}
			}
			if (c != '\r')
				return CALLBACKRET_Error;
			if (!domeas(item,res+i,(i == 1)? &sp: NULL))
			{
				COM_Puts("Measure error\r\n");
				return CALLBACKRET_Error;
			}
			if (i == 1)
			{
				float yh = calib_condutt[item+1];
				float yl = calib_condutt[item];
				float nh = res[1];
				float nl = res[0];
				float gf = (yh - yl) / (nh - nl);
				float nos = nh - (yh / gf);
				COM_Printf("Nch=%.3f Ncl=%.3f GF=%.10e Nos=%.10e sp=%.10e\r\n",
					nh,
					nl,
					gf,
					nos,
					sp);
				EEP_Write(EEP_HNch_addr+(item*20),&nh,sizeof(float));
				EEP_Write(EEP_HNcl_addr+(item*20),&nl,sizeof(float));
				EEP_Write(EEP_HGF_addr+(item*20),&gf,sizeof(float));
				EEP_Write(EEP_HNos_addr+(item*20),&nos,sizeof(float));
				EEP_Write(EEP_HPhase_addr+(item*20),&sp,sizeof(float));
				return CALLBACKRET_Ok;
			}
		}
		
	}
	return CALLBACKRET_Error;
}

static int parser_i2cread(char *arg)
{
char *s;
uint8_t slave,len;

	s = strtok(arg,",");
	if (!s || !*s || !getHexByte(s,&slave,0) || !slave || (slave & 0x80))
		return CALLBACKRET_Error;
// 	s = strtok(NULL,",");
// 	if (!s || !*s || !getHexByte(s,&addr,0))
// 		return CALLBACKRET_Error;
	s = strtok(NULL,",");
	if (!s || !*s || !getHexByte(s,&len,0) || !len || (len > 128))
		return CALLBACKRET_Error;
	
	//if (!I2C_RandRead(slave,addr,1,(uint8_t *)pbuf,len))
	if (!I2C_Read(slave,(uint8_t *)pbuf,len))
	{
		COM_Puts("\r\nOperation failure\r\n");
		return CALLBACKRET_Error;
	}

	for (slave = 0; slave < len; slave++)
	{
		if ((slave & 15) == 0)
			COM_Printf("\r\n%02X ",slave);
		COM_Printf(" %02X",pbuf[slave]);
	}
	COM_Puts("\r\n");
	return CALLBACKRET_Ok;
}

static int parser_i2cwrite(char *arg)
{
char *s;
uint8_t slave,len;

	s = strtok(arg,",");
	if (!s || !*s || !getHexByte(s,&slave,0) || !slave || (slave & 0x80))
		return CALLBACKRET_Error;
// 	s = strtok(NULL,",");
// 	if (!s || !*s || !getHexByte(s,&addr,0))
// 		return CALLBACKRET_Error;
	len = 0;
	while (s = strtok(NULL,","))
	{
		if (!s || !*s || !getHexByte(s,(uint8_t *)pbuf+len,0))
			break;
		len++;
	}
	if (s || !len)
		return CALLBACKRET_Error;
	if (!I2C_Write(slave,(uint8_t *)pbuf,len))
	{
		COM_Puts("\r\nOperation failure\r\n");
		return CALLBACKRET_Error;
	}
	return CALLBACKRET_Ok;
}

static int parser_ini(char *arg)
{
char *s,*fn = arg;
FFILE *fp;

	// get the name of the ini file
	s = strchr(fn,',');
	if (s)
		*s++ = 0;
	if (ffindFile(fn) < 0)
		return CALLBACKRET_Error;
	if (*s == 0)
	{
		if ((fp = ffopen(fn,'r')) == 0)
			return CALLBACKRET_Error;
		COM_Puts("\r\n");
		while (!ffeof(fp))
			if (ffreadln(fp,pbuf,PBUF_SIZE))
				COM_Puts(pbuf);
		ffclose(fp);
		return CALLBACKRET_Ok;
	}
	if (*s != '[')
		return CALLBACKRET_Error;
	if (manageIni(fn,s))
		return CALLBACKRET_Ok;
	return CALLBACKRET_Error;
}

static int parser_devid(char *arg)
{
uint16_t i,*pwr = (uint16_t *)0x1ffff7e8;

	COM_Puts("\r\n*DEVID: ");
	for (i = 0; i < 6; i++)
	{
		COM_Printf("%04X",pwr[i]);
		if (i < 5)
			COM_Putc(',');
	}
	COM_Puts("\r\n");
	return CALLBACKRET_Ok;
}

static int parser_devsz(char *arg)
{
uint16_t *pwr = (uint16_t *)0x1ffff7e0;

	COM_Printf("\r\n*DEVSZ: %uKB\r\n",*pwr);
	return CALLBACKRET_Ok;
}

static int parser_debug(char *arg)
{
	if ((*arg == '?') && !arg[1])
	{
		COM_Printf("\r\n*DBG: %08lX\r\n",DBG_GetLvl());
		return CALLBACKRET_Ok;
	}
	else if ((*arg == '=') && (arg[1] == '?') && !arg[2])
	{
		uint16_t i;

		for (i = 0; i < DBGLVL_NumBits; i++)
			COM_Printf("\r\n*DBG: \"%02X %s\"",1 << i,str_dbgBitsName[i]);
		COM_Puts("\r\n");
		return CALLBACKRET_Ok;
	}
#if 0
	else if (strcmpNoCase(arg,"=RESTORE"))
	{
		uint32_t lvl;
		EEP_Read(EEP_dbglvl_addr,&lvl,4);
		if (lvl != 0xffffffff)
			DBG_SetLvl(lvl);
		return CALLBACKRET_Ok;
	}
#endif
	else if ((*arg == '=') && isxdigits(arg+1,0))
	{
		uint32_t lvl;

		if (!getHexDWord(arg+1,&lvl,0))
			return CALLBACKRET_Error;

		DBG_SetLvl(lvl);
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_date(char *arg)
{
	if ((*arg == '?') && !arg[1])
	{
		struct tm *tm;
		time_t t = time(NULL);
		tm = gmtime(&t);

		COM_Printf("\r\n*DATE: %d/%02d/%02d,%02d:%02d:%02d,%d,%s,%d\r\n",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
			tm->tm_hour,tm->tm_min,tm->tm_sec,tm->tm_wday,WeekDayNames[tm->tm_wday],tm->tm_isdst);
		return CALLBACKRET_Ok;
	}
	else if ((*arg == '!') && !arg[1])
	{
		struct tm *tm;
		time_t t = time(NULL);
		tm = localtime(&t);

		COM_Printf("\r\n*DATE: %d/%02d/%02d,%02d:%02d:%02d,%d,%s,%d\r\n",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
			tm->tm_hour,tm->tm_min,tm->tm_sec,tm->tm_wday,WeekDayNames[tm->tm_wday],tm->tm_isdst);
		return CALLBACKRET_Ok;
	}
	else if ((*arg == '=') && (strlen(arg+1) == 12) && isdigits(arg+1,12))
	{
		// YYMMDDHHMMSS  //anno deve essere offset da 1900
		struct tm tm;
		uint32_t t;
		char *s = arg+1;
		memset(&tm,0,sizeof(tm));
		tm.tm_year = convint(s,2) + 100;
		tm.tm_mon = convint(s+2,2);
		if ((tm.tm_mon < 1) || (tm.tm_mon > 12))
			return CALLBACKRET_Error;
		tm.tm_mon--;
		tm.tm_mday = convint(s+4,2);
		if ((tm.tm_mday < 1) || (tm.tm_mday > 31))
			return CALLBACKRET_Error;
		tm.tm_hour = convint(s+6,2);
		if ((tm.tm_hour < 0) || (tm.tm_hour > 23))
			return CALLBACKRET_Error;
		tm.tm_min = convint(s+8,2);
		if ((tm.tm_min < 0) || (tm.tm_min > 59))
			return CALLBACKRET_Error;
		tm.tm_sec = convint(s+10,2);
		if ((tm.tm_sec < 0) || (tm.tm_sec > 59))
			return CALLBACKRET_Error;
		tm.tm_isdst = -1;//indica che non so se ora legale o solare
		t = mktime(&tm);
		RTC_SetCounter(t);
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_eeprom_read(char *arg)
{
char *s;
uint32_t addr;
uint16_t len,i;

	s = strtok(arg,",");
	if (!s || !*s || !getHexDWord(s,&addr,0) || (addr >= EEPROM_SIZE))
		return CALLBACKRET_Error;
	s = strtok(NULL,",");
	if (!s || !*s || !getHexWord(s,&len,0) || !len || (len > PBUF_SIZE))
		return CALLBACKRET_Error;
	if (strtok(NULL,","))
		return CALLBACKRET_Error;

	if (!EEP_Read(addr,(uint8_t *)pbuf,len))
		return CALLBACKRET_Error;

	for (i = 0; i < len; i++)
	{
		if ((i & 15) == 0)
			COM_Printf("\r\n%05lX ",i+addr);
		COM_Printf(" %02X",pbuf[i]);
	}
	COM_Puts("\r\n");
	return CALLBACKRET_Ok;
}

static int parser_eeprom_write(char *arg)
{
char *s;
uint32_t addr;
uint16_t len;

	s = strtok(arg,",");
	if (!s || !*s || !getHexDWord(s,&addr,0) || (addr >= EEPROM_SIZE))
		return CALLBACKRET_Error;
	len = 0;
	while (s = strtok(NULL,","))
	{
		if (!s || !*s || !getHexByte(s,(uint8_t *)pbuf+len,0))
			break;
		len++;
	}
	if (s || !len)
		return CALLBACKRET_Error;
	if (!EEP_Write(addr,(uint8_t *)pbuf,len))
		return CALLBACKRET_Error;
	return CALLBACKRET_Ok;
}

static int parser_ddir(char *arg)
{
DIR *pdir;

	if (!ffready(0))
		return CALLBACKRET_Error;
	pdir = opendir();
	if (pdir)
	{
		char *s;
		COM_Puts("\r\n");
		while(( s = readwdir( pdir)) != NULL)
			COM_Puts(s);
		closedir( pdir) ;
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_ddel(char *arg)
{
	if (!ffready(0))
		return CALLBACKRET_Error;
	if (*arg && ffdelete(arg))
		return CALLBACKRET_Ok;
	return CALLBACKRET_Error;
}

static int parser_dformat(char *arg)
{
	if (!ffready(0))
		return CALLBACKRET_Error;
	fformat();
	return CALLBACKRET_Ok;
}

static int parser_dcopy(char *arg)
{
char *s;
char *fno,*fnn;
FFILE *fpo,*fpn;
	
	if (!ffready(0))
		return CALLBACKRET_Error;
	s = strtok(arg,",");
	if (!s)
		return CALLBACKRET_Error;
	fno = s;
	s = strtok(NULL,",");
	if (!s)
		return CALLBACKRET_Error;
	fnn = s;
	if (strtok(NULL,","))
		return CALLBACKRET_Error;
	strtrim(fno);
	strtrim(fnn);

	if (!strcmp(fno,fnn))
		return CALLBACKRET_Error;
	
	if (ffindFile(fno) < 0)
		return CALLBACKRET_Error;
	
	fpo = ffopen(fno,'r');
	if (!fpo)
		return CALLBACKRET_Error;
	fpn = ffopen(fnn,'w');
	if (!fpn)
	{
		ffclose(fpo);
		return CALLBACKRET_Error;
	}
	
	do {
		uint16_t b;
		b = ffread(fpo,pbuf,PBUF_SIZE);
		if (b)
			ffwrite(fpn,pbuf,b);
	} while (!ffeof(fpo));
	ffclose(fpo);
	ffclose(fpn);
	return CALLBACKRET_Ok;
}

static int parser_dmove(char *arg)
{
char *s;
char *fno,*fnn;
	
	if (!ffready(0))
		return CALLBACKRET_Error;
	s = strtok(arg,",");
	if (!s)
		return CALLBACKRET_Error;
	fno = s;
	s = strtok(NULL,",");
	if (!s)
		return CALLBACKRET_Error;
	fnn = s;
	if (strtok(NULL,","))
		return CALLBACKRET_Error;
	strtrim(fno);
	strtrim(fnn);

	if (!strcmp(fno,fnn))
		return CALLBACKRET_Error;
	
	if ((ffindFile(fno) < 0) || (ffindFile(fnn) >= 0))
		return CALLBACKRET_Error;
		
	if (ffmove(fno,fnn))
		return CALLBACKRET_Ok;
	return CALLBACKRET_Error;
}

static int parser_dcat(char *arg)
{
FFILE *fp;
uint16_t b,i;

	if (!ffready(0))
		return CALLBACKRET_Error;
	strtrim(arg);
	fp = ffopen(arg,'r');
	if (!fp)
		return CALLBACKRET_Error;
	COM_Printf("\r\n");
	while (!ffeof(fp))
	{
		if ((b = ffread(fp,pbuf,PBUF_SIZE)) > 0)
		{
			for (i = 0; i < b; i++)
			{
				if ((pbuf[i] >= ' ') && (pbuf[i] < 0x7f))
					COM_Putc(pbuf[i]);
				else if (pbuf[i] == '\n')
					COM_Puts("\r\n");
				else if (pbuf[i] != '\r')
					COM_Putc('.');
			}
		}
	}
	ffclose(fp);
	return CALLBACKRET_Ok;
}

static int parser_dwrite(char *arg)
{
FFILE *fp;
uint16_t b;
	
	if (!ffready(0))
		return CALLBACKRET_Error;
	strtrim(arg);
	fp = ffopen(arg,'w');
	if (!fp)
		return CALLBACKRET_Error;
	COM_Printf("\r\nPress Ctrl-Z to stop\r\n");
	b = 0;
	for (;;)
	{
		uint8_t c;
		if (!COM_Getch(&c))
			continue;
		if ((c == 0x1a) || (c == '\r'))
		{
			if (c == '\r')
			{
				COM_Puts("\r\n");
				strcpy(pbuf+b,"\r\n");
				b += 2;
			}
			if (b)
			{
				ffwrite(fp,pbuf,b);
				b = 0;
			}
			if (c == 0x1a)
				break;
		}
		else if ((c >= ' ') && (c < 0x7f) && (b < (PBUF_SIZE-2)))
		{
			pbuf[b++] = c;
			COM_Putc(c);
		}
		else if ((c == BACK_SPACE) && (b > 0))
		{
			b--;
			COM_Putc(c);
		}
	}
	ffclose(fp);
	return CALLBACKRET_Ok;
}

static int parser_sn(char *arg)
{
uint64_t sn;

	if (get_sn(&sn))
	{
		COM_Printf("\r\n*SN: %10llu\r\n",sn);
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_sn32(char *arg)
{
uint32_t sn;

	if (get_sn32(&sn))
	{
		COM_Printf("\r\n*SN32: %08lX,%lu\r\n",sn,sn);
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_heap(char *arg)
{
extern void heap_status(void);

	COM_Puts("\r\n");
	heap_status();
	return CALLBACKRET_Ok;
}

static int parser_login(char *arg)
{
char *pwd = pbuf+127;

	*pwd++ = 0;
	if (!getConfigVoice("config.ini","system","pwd",pwd,20) || !*pwd)
		strcpy(pwd,DEFAULT_PASSWORD);
	if (!strcmp(arg,pwd))
	{
		parser_status = TRUE;
		pwd_fails = 0;
		return CALLBACKRET_Ok;
	}
	else
	{
		parser_status = 0;
		if (pwd_fails < MAX_PWD_FAILS)
			pwd_fails++;
		vTaskDelay((kSec*5)*pwd_fails);
	}
	return CALLBACKRET_Error;
}

static int parser_logout(char *arg)
{
	parser_status = FALSE;
	return CALLBACKRET_Ok;
}

static int parser_meas(char *arg)
{
float resist,condut,conduc;

	if (strcmpNoCase(arg,"=INIT"))
	{
		if (meas_loadParams())
		{
			meas_printParams();
			return CALLBACKRET_Ok;
		}
		return CALLBACKRET_Error;
	}
	else if (strcmpNoCase(arg,"=PAUSE"))
	{
		meas_pause();
		return CALLBACKRET_Ok;
	}
	else if (strcmpNoCase(arg,"=GO"))
	{
		meas_go();
		return CALLBACKRET_Ok;
	}
	else if (strcmpNoCase(arg,"=PTC"))
	{
		if (meas_temp(&resist))
		{
			COM_Printf("\r\n*MEAS: %.3fOhm\r\n",resist);
			return CALLBACKRET_Ok;
		}
		else
			return CALLBACKRET_Error;
	}
	else if (!strcmpNoCase(arg,"=COND"))
		return CALLBACKRET_Error;

	if (meas(&resist,&condut,&conduc))
	{
		COM_Printf("\r\n*MEAS: %.10eS,%.10eOhm,%.10eS/cm2\r\n",condut,resist,conduc);
		return CALLBACKRET_Ok;
	}
	else
		return CALLBACKRET_Error;
}

static int parser_pinconf(char *arg)
{
	if ((toupper(*arg) >= 'A') && (toupper(*arg) <= 'E') && isxdigit(arg[1]))
	{
		int port = toupper(*arg)-'A';
		int pin;
		if (isdigit(arg[1]))
			pin = arg[1] - '0';
		else
			pin = 10+(toupper(arg[1])-'A');
		if (arg[2] == '=')
		{
			char *s = arg+3;
			int i;
			strupr(s);
			for (i = 0; PortModesStr[i]; i++)
			{
				if (!strcmp(s,PortModesStr[i]))
					break;
			}
			if (!PortModesStr[i])
				return CALLBACKRET_Error;
			GpioInit(PortRegs[port],(1 << pin),PortModes[i],0);
			return CALLBACKRET_Ok;
		}
	}
	return CALLBACKRET_Error;
}

static int parser_pinread(char *arg)
{
	if ((toupper(*arg) >= 'A') && (toupper(*arg) <= 'E') && isxdigit(arg[2]) && !arg[3])
	{
		int port = toupper(arg[1])-'A';
		int pin;
		if (isdigit(arg[2]))
			pin = arg[2] - '0';
		else
			pin = 10+(toupper(pbuf[6])-'A');
		COM_Printf("\r\n%u\r\n",GPIO_ReadInputDataBit(PortRegs[port], (1 << pin)));
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_pinwrite(char *arg)
{
	// AT*PWB
	if ((toupper(*arg) >= 'A') && (toupper(*arg) <= 'E') && isxdigit(arg[1]) && (arg[2] == '=') && ((arg[3] | 1) == '1') && !arg[4])
	{
		int port = toupper(*arg)-'A';
		int pin;
		int val = arg[3];
		if (isdigit(arg[1]))
			pin = arg[1] - '0';
		else
			pin = 10+(toupper(arg[1])-'A');
		GPIO_WriteBit(PortRegs[port], (1 << pin), (val == '1')? Bit_SET: Bit_RESET);
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_resetp(char *arg)
{
uint64_t cmd = 0;

	if ((*arg == '?') && !arg[1])
	{
		COM_Puts("\r\n*RESET: PROG|PARSER|LAUNCH|NONE\r\n");
		return CALLBACKRET_Ok;
	}
	else if (strcmpNoCase(arg,"PROG"))
		cmd = MAGIC_WORD_PROG;
	else if (strcmpNoCase(arg,"PARSER"))
		cmd = MAGIC_WORD_PARSER;
	else if (strcmpNoCase(arg,"LAUNCH"))
		cmd = MAGIC_WORD_LAUNCH;
	else if (!strcmpNoCase(arg,"NONE"))
		return CALLBACKRET_Error;
	ffsync();
	parser_end(CALLBACKRET_Ok,answer);
	vTaskDelay(kHalfSec);
	system_setcmd(cmd);
	system_reset();
	return CALLBACKRET_Error;	// should never reach this line
}

static int parser_reset(char *arg)
{
	ffsync();
	parser_end(CALLBACKRET_Ok,answer);
	vTaskDelay(kSec);
	system_reset();
	return CALLBACKRET_Error;
}

static int parser_time(char *arg)
{
	COM_Printf("\r\n*TIME: %lu\r\n",xTaskGetTickCount());
	return CALLBACKRET_Ok;
}

static int parser_tz(char *arg)
{
	COM_Printf("\r\n*TZ: \"%s\"\r\n",__getzone());
	return CALLBACKRET_Ok;
}

static int parser_utime(char *arg)
{
	if (!*arg || ((*arg == '?') && !arg[1]))
	{
		COM_Printf("\r\n*UTIME: %lu\r\n",RTC_GetCounter());
		return CALLBACKRET_Ok;
	}
	else if ((*arg == '=') && arg[1] && isdigits(arg+1,0))
	{
		uint32_t t;
		t = atodw(arg+1);
		RTC_SetCounter(t);
		return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

static int parser_ymodem(char *arg)
{
	if (((toupper(*arg) == 'P') || (toupper(*arg) == 'V')) && !arg[1])
	{
		COM_Puts("\r\nREADY\r\n");
		if (PRSR_YmodemRx(toupper(*arg) == 'P',TRUE))
			return CALLBACKRET_Ok;
	}
	return CALLBACKRET_Error;
}

#if (configUSE_TRACE_FACILITY)
/**
 *  \brief parser_tasks, get freertos tasks informations
 *  
 *  \param [in] arg parameters of the command
 *  \return CALLBACKRET_Enum type to notify the result of the command
 *  
 */
static int parser_tasks(char *arg)
{
uint16_t w = uxTaskGetNumberOfTasks();
char *s = (char *)pvPortMalloc(w * 40);

	COM_Printf("\r\n*TASKS: %u\r\n",w);
	if (s)
	{
		vTaskList(s);
		COM_Puts(s);
		vPortFree(s);
	}
	else
		COM_Puts("\r\nNot enough memory\r\n");
	return CALLBACKRET_Ok;
}
#endif

static int parser_version(char *arg)
{
	parser_printbanner();
	return CALLBACKRET_Ok;
}

static int parser_help(char *arg)
{
const struct CommandSt *cst;
int i;

	COM_Puts("\r\n");
	for (i = 0,cst = commandsList; cst->prefix; cst++)
	{
		if (i >= 60)
		{
			COM_Puts(arg);
			COM_Puts(",\r\n");
			i = 0;
		}
		if (!parser_status && cst->reserved)
			continue;
		if (i)
			i += sprintf(arg+i,",");
		i += sprintf(arg+i,"%s",cst->prefix);
		if (cst->args)
			i += sprintf(arg+i,"@");
	}
	COM_Puts(arg);
	COM_Puts("\r\n");
	return CALLBACKRET_Ok;
}

static void parser_end(uint8_t se,uint8_t ans)
{
	if (se != CALLBACKRET_Error)
	{
		if (ans && (se != CALLBACKRET_Ignore))
			COM_Puts("\r\nOK\r\n");
	}
	else
		COM_Puts("\r\nERROR\r\n");
}

static void parser_printbanner(void)
{
	COM_Printf("\r\n*VER: %s (%04X)\r\n",gBanner,fw_version.chksum);
}

void parser_task(void *pvParameters)
{
uint8_t echo;
char *cmdbuf;
uint16_t cmdidx;
int synErr;

	cmdbuf = (char *)pvPortMalloc(PBUF_SIZE);
	pbuf = cmdbuf;
	answer = echo = TRUE;
	parser_printbanner();
	parser_status = TRUE;
	cmdidx = 0;
	for (;;)
	{
		if (cmdidx)
		{
			parser_end(synErr,answer);
			cmdidx = 0;
		}
		busy = FALSE;
		synErr = CALLBACKRET_Ok;
		do {
			uint8_t c;
			if (!COM_Getch(&c))
				continue;
			switch(cmdidx)
			{
			case 0:
				if (toupper(c) == 'A')
				{
					*cmdbuf = c;
					cmdidx++;
				}
				else if (c == 'U')
					COM_Puts("\r\nOK\r\n");
				break;
			case 1:
				if (((*cmdbuf == 'A') && (c == 'T')) || ((*cmdbuf == 'a') && (c == 't')))
				{
					cmdbuf[1] = c;
					if (echo)
					{
						cmdbuf[2] = 0;
						COM_Echo(cmdbuf[0]);
						COM_Echo(cmdbuf[1]);
					}
					cmdidx = 2;
				}
				else
				{
					if (c == 'U')
						COM_Puts("\r\nOK\r\n");
					cmdidx = 0;
				}
				break;
			default:
				switch(c)
				{
				case ESCAPE:
					cmdidx = 0;
					c = 0;
					break;
				case BACK_SPACE:
					if (cmdidx > 2)
						cmdidx--;
					else
						c = 0;
					break;
				case RETURN:
					c = 0;
					if (echo)
						COM_Echo('\r');
					cmdbuf[cmdidx] = 0;
					synErr = CALLBACKRET_Error;
					break;
				default:
					if ((c >= ' ') && (cmdidx < (PBUF_SIZE-1)))
						cmdbuf[cmdidx++] = c;
					else
						c = 0;
					break;
				}
				if (echo && c)
					COM_Echo(c);
				break;
			}
		} while (synErr == CALLBACKRET_Ok);
		answer = TRUE;
		busy = TRUE;
		switch(toupper(cmdbuf[2]))
		{
		case 0:
			synErr = CALLBACKRET_Ok;
			break;
		case '#':
			answer = FALSE;
		case '*':
			if (cmdbuf[3])
			{
				const struct CommandSt *cst;
				char *s = cmdbuf+3;
				for (cst = commandsList; cst->prefix; cst++)
				{
					if ((parser_status || !cst->reserved) && strmatchNoCase(s,cst->prefix))
					{
						s += strlen(cst->prefix);
						if ((cst->args && *s) || (!cst->args && !*s))
						{
							synErr = cst->callback(s);
							break;
						}
					}
				}
			}
			else
				synErr = CALLBACKRET_Ok;
			break;
		case 'E':
			if (((cmdbuf[3] | 1) == '1') && (cmdbuf[4] == 0))
			{
				echo = cmdbuf[3] == '1';
				synErr = CALLBACKRET_Ok;
			}
			break;
		}
	}
}

void parser_create_task(void)
{
	xTaskCreate(parser_task, "parser", 256, NULL, tskIDLE_PRIORITY, NULL);
}

int parser_busy(void)
{
	return busy != 0;
}

#else
#pragma message("parser not included")
void parser_create_task(void)
{
	// No task created
}

#endif	// !defined(COM_NONE)
