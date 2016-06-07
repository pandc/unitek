#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "bsp.h"

#include <freertos.h>
#include <semphr.h>

#include "com.h"
#include "debug.h"

#if !defined(COM_NONE)

// TRACE_MODE
// 0	no fs used
// 1	debug.log open and closed for every dprintf
// 2	debug.log kept open for the whole run
#define TRACE_MODE		0

#define DEBUG_FILENAME_LOG	"debug.log"		//!< File name used 
#define DEBUG_FILENAME_BAK	"debug.bak"

#define DEBUG_MAX_FILESIZE	100000

#if TRACE_MODE != 0
#include "ffile.h"
#if TRACE_MODE == 2
static FFILE *dfp;
#endif
#endif

static uint32_t dbglvl_level;
#define PRNTFBUF_SIZE		192
static char *prntfBuf;

const char *str_dbgBitsName[DBGLVL_NumBits] = {
	"Meas",
	"Keyboard",
};

int DBG_CheckLevel(uint32_t lvl)
{
	if (dbglvl_level & lvl)
		return TRUE;
	else
		return FALSE;
}

uint32_t DBG_GetLvl(void)
{
	return dbglvl_level;
}

void DBG_SetLvl(uint32_t lvl)
{
#if TRACE_MODE == 2
	if ((lvl ^ dbglvl_level) & DBGLVL_Logit)
	{
		if (lvl & DBGLVL_Logit)
		{
			// starting trace
			dfp = ffopen(DEBUG_FILENAME_LOG,'a');
		}
		else
		{
			// stopping trace
			dbglvl_level ^= DBGLVL_Logit;	// clear DBGLVL_Logit
			if (dfp)
			{
				ffclose(dfp);
				dfp = NULL;
			}
			dbglvl_level ^= DBGLVL_Logit;	// clear DBGLVL_Logit
		}
	}
#endif
#if TRACE_MODE != 0
	if (((lvl ^ dbglvl_level) & DBGLVL_Logit) && !(lvl & DBGLVL_Logit))
	{
		// stopping trace
		dbglvl_level ^= DBGLVL_Logit;	// clear flag
		FFILE *fp = ffopen(DEBUG_FILENAME_LOG,'r');
		if (fp)
		{
			if (ffsize(fp) > DEBUG_MAX_FILESIZE)
			{
				ffclose(fp);
				Dprintf(DBGLVL_Info,"power_close: archiving debug log file");
				ffdelete(DEBUG_FILENAME_BAK);
				ffmove(DEBUG_FILENAME_LOG,DEBUG_FILENAME_BAK);
			}
			else
				ffclose(fp);
		}
	}
#endif
	dbglvl_level = lvl;
}

// 012345678901234567890123456789
// 2010-08-27 11:05:00 ........
//      <000>Multibus:

static SemaphoreHandle_t xSemaphoreDbg;

void DBG_Init(void)
{
	if (xSemaphoreDbg)
		return;
	xSemaphoreDbg = xSemaphoreCreateMutex();
	prntfBuf = (char *)pvPortMalloc(PRNTFBUF_SIZE);
}

void DBG_GetMutex(void)
{
	xSemaphoreTake(xSemaphoreDbg,portMAX_DELAY);	// get mutex
}

void DBG_RelMutex(void)
{
	xSemaphoreGive(xSemaphoreDbg);		// release Mutex
}

void Dprintf(uint32_t dbg,const char *fmt,...)
{
va_list argptr;
int len;
time_t t;
struct tm *tm;

	if ((dbg && !(dbg & dbglvl_level)) && (dbg != DBGLVL_Error))
		return;

	t = time(NULL);
	tm = gmtime(&t);

	DBG_GetMutex();
	len = sprintf(prntfBuf,"%02d/%02d/%02d %02d:%02d:%02d ",tm->tm_year % 100,tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

    va_start(argptr, fmt);
    vsnprintf(prntfBuf+len, PRNTFBUF_SIZE-len, fmt, argptr);
    va_end(argptr);
	prntfBuf[PRNTFBUF_SIZE-1] = 0;
	len = strlen(prntfBuf);
	if (len <= (PRNTFBUF_SIZE-3))
	{
		strcat(prntfBuf,"\r\n");
		COM_Puts(prntfBuf);
#if TRACE_MODE != 0
		if ((dbglvl_level & DBGLVL_Logit) && ffready(0))
		{
#if TRACE_MODE == 1
			FFILE *dfp = ffopen(DEBUG_FILENAME_LOG,'a');
			if (!dfp)
				COM_Puts("\r\nDprintf: error opening debug.log file\r\n");
			else
#else
			if (dfp)
#endif
			{
				ffputs(dfp,prntfBuf);
#if TRACE_MODE == 1
				ffclose(dfp);
#endif
			}
		}
#endif
	}
	else
		COM_Printf("debug message too long (%d)\r\n",len);
	DBG_RelMutex();
}

#else	// !defined(COM_NONE)
#pragma message("debug messages not included")
#endif
