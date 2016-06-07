#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "bsp.h"

#include "freertos.h"
#include "task.h"

#include "com.h"
#include "crc16.h"
#include "ffile.h"
#include "ymodem.h"

#define	YMDM_INTERC_TOUT	kSec
#define TBUF_SIZE			512

#define SYNC	'C'
#define SOH		0x01
#define STX		0x02
#define EOT		0x04
#define ACK		0x06
#define NAK		0x15
#define CAN		0x18

#define MAX_INITIAL_TOUTS	7

static void PRSR_YmodemRx_5(const char *msg)
{
	COM_Puts("\x18\x18");
	vTaskDelay(kSec*3);
	COM_Puts(msg);
	COM_Puts("\r\n");
}

int PRSR_YmodemRx(int yprog,int ycrctt)
{
int ch,res = FALSE;
uint8_t c,*ybuf = NULL,firstpacket,st,indsh,cntouts = 0;
uint16_t ymdmPacketSize = 0,gind;
long decLen,fp;
uint32_t tout;
FFILE *pFile = NULL;

	fp = decLen = 0;
	firstpacket = TRUE;
	tout = kDec;
	st = 1;
	for (;;)
	{
		if (COM_TGetch(&c,tout))
			ch = c;
		else
			ch = -1;
		switch(st)
		{
		case 1:
		case 3:
			switch(ch)
			{
			case -1:
				if ((st == 3) || (++cntouts >= MAX_INITIAL_TOUTS))
				{
					PRSR_YmodemRx_5("ERROR timeout waiting for data");
					goto exi;
				}
				COM_Putc((ycrctt)? SYNC: NAK);
				tout = kSec*5;
				break;
			case SOH:	// 128 packet data
				if (ymdmPacketSize && (ymdmPacketSize != 128))
				{
					ymdmPacketSize = 0;
					vPortFree(ybuf);
				}
				if (!ymdmPacketSize)
					ybuf = pvPortMalloc(128+ycrctt+4);
				ymdmPacketSize = 128;
				gind = 1;
				ybuf[0] = SOH;
				tout = YMDM_INTERC_TOUT;
				st = 2;
				break;
			case STX:	// 1024 packet data
				if (ymdmPacketSize && (ymdmPacketSize != 1024))
				{
					ymdmPacketSize = 0;
					vPortFree(ybuf);
				}
				if (!ymdmPacketSize)
					ybuf = pvPortMalloc(1024+ycrctt+4);
				ymdmPacketSize = 1024;
				gind = 1;
				ybuf[0] = STX;
				tout = YMDM_INTERC_TOUT;
				st = 2;
				break;
			case CAN:		// Transfer abort
			case NAK:
				PRSR_YmodemRx_5("Transfer aborted");
				goto exi;
			case EOT:	// End of transfer
				if (fp >= decLen)
				{
					if (pFile)
					{
						ffclose(pFile);
						pFile = NULL;
					}
					COM_Putc(ACK);
					vTaskDelay(kSec);
					firstpacket = TRUE;
					st = 1;
					cntouts = 0;
				}
				else
				{
					PRSR_YmodemRx_5("ERROR file incomplete");
					goto exi;
				}
				break;
			}
			break;
		case 2:
			if (ch < 0)
			{
				gind = 0;
				COM_Putc(NAK);
				if (!fp)
				{
					tout = kSec*5;
					st = 1;
					cntouts = 0;
				}
				else
				{
					tout = kMin;
					st = 3;
				}
			}
			else
			{
				ybuf[gind++] = c;
				if (gind == (ymdmPacketSize+ycrctt+4))	// 132/133
				{
					uint16_t i,crc;
					st = 3;
					if (ycrctt)
					{
						crc = crc16_calc(ybuf+3,ymdmPacketSize);
						if (((crc >> 8) != ybuf[3+ymdmPacketSize]) || ((crc & 0xff) != ybuf[4+ymdmPacketSize]))
						{
							COM_Putc(NAK);
							break;
						}
					}
					else
					{
						for (crc = 0, i = 0; i < ymdmPacketSize; i++)
							crc = (crc + ybuf[3+i]) & 0xff;
						if (crc != ybuf[3+ymdmPacketSize])
						{
							COM_Putc(NAK);
							break;
						}
					}
					if (firstpacket || (((indsh+1) & 0xff) == ybuf[1]))
					{
						if ((decLen > 0) && (fp >= decLen))
						{
							COM_Putc(ACK);
							vTaskDelay(kSec*3);
							COM_Puts("\r\n");
							res = TRUE;
							goto exi;
						}
						indsh = ybuf[1];
						if (firstpacket && (indsh == 0))
						{
							indsh = 0;
							COM_Putc(ACK);
							if (ybuf[3] != 0)
							{
								char *fn = (char *)(ybuf+3);
								if (strlen(fn) > FAT_FILE_NAME_LEN)
								{
									PRSR_YmodemRx_5("Invalid file name");
									goto exi;
								}
								if ((pFile = ffopen(fn,(yprog)? 'w': 'r')) == NULL)
								{
									sprintf((char *)ybuf,"Unable to open file \"%s\"",fn);
									PRSR_YmodemRx_5((char *)ybuf);
									goto exi;
								}
							
								// get name and length
								decLen = atol((char *)&ybuf[3+strlen((char *)&ybuf[3])+1]);
								if (!yprog && (ffsize(pFile) != decLen))
								{
									PRSR_YmodemRx_5("Bad file length");
									goto exi;
								}
								fp = 0;
								tout = kHalfSec;
								st = 1;
								cntouts = 0;
							}
							else
							{
								COM_Puts("\r\n");
								goto exi;
							}
						}
						else
						{
							if (firstpacket)
							{
								PRSR_YmodemRx_5("Protocol error");
								goto exi;
							}
							// program data in flash
							if (fftell(pFile) >= decLen)
							{
								PRSR_YmodemRx_5("ERROR pattern exceeds declared length");
								goto exi;
							}
							else
							{
								long slice = decLen - fftell(pFile);
								if (slice > ymdmPacketSize)
									slice = ymdmPacketSize;
								if (yprog)
								{
									if (ffwrite(pFile,ybuf+3,slice) != slice)
									{
										PRSR_YmodemRx_5("ERROR writing data in file");
										goto exi;
									}
								}
								else
								{
									long l;
									for (l = 0; l < slice; l++)
									{
										if (ybuf[3+l] != ffget(pFile))
										{
											PRSR_YmodemRx_5("ERROR verifying");
											goto exi;
										}
									}
								}
								fp += slice;
								COM_Putc(ACK);
							}
						}
						firstpacket = FALSE;
					}
					else if (indsh == ybuf[1])
						COM_Putc(ACK);	// ack
					else
						COM_Putc(NAK);	// nack
				}
				else
				{
					if ((gind == 3) && (ybuf[1] != (ybuf[2] ^ 0xff)))
					{
						tout = YMDM_INTERC_TOUT;
						st = 4;
					}
				}
			}
			break;
		case 4:
			if (ch < 0)
			{
				COM_Putc(NAK);
				if (!fp)
				{
					tout = kSec*10;
					st = 1;
					cntouts = 0;
				}
				else
					st = 3;
			}
			break;
		}
	}
exi:
	if (ybuf)
		vPortFree(ybuf);
	if (pFile)
		ffclose(pFile);
	return res;
}
