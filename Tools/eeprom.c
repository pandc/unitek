/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : eeprom.c
* Author             : MCD Application Team
* Version            : V2.0.0
* Date               : 06/16/2008
* Description        : This file provides all the EEPROM emulation firmware functions.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "bsp.h"
#include "tools.h"
#include "ffile.h"

#include "config.h"
#include "eeprom.h"

#define SECTION_NAME	"parameters"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

const char *str_voiceName[EEP_Last] = {
	"mbNode",
	"inpEdges",
	"Persist1",
	"Persist2",
	"Persist3",
	"Persist4",
	"Persist5",
	"Persist6",
	"Persist7",
	"Persist8",
	"Baud",
	"mbGateway",
	"mbPort",
	"Rfid",
	"mbPlcTout",
	"PlcRetr",
	"PlcFlags"
};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EE_Init
* Description    : Restore the pages to a known good state in case of pages'
*                  status corruption after a power loss.
* Input          : None.
* Output         : None.
* Return         : - 0: on failure
*                  - 1: on success
*******************************************************************************/
u16 EE_Init(void)
{
	if (ffindFile(EEPROM_FILE) < 0)
	{
		FFILE *fp = ffopen(EEPROM_FILE,'w');
		if (!fp)
			return 0;
		ffputs(fp,"[" SECTION_NAME "]\r\n");
		ffclose(fp);
	}
	return 1;
}

/*******************************************************************************
* Function Name  : EE_ReadVariable
* Description    : Returns the last stored variable data, if found, which
*                  correspond to the passed virtual address
* Input          : - VirtIdx: Index of Variable virtual address
*                  - Data: Global variable contains the read variable value
* Output         : None
* Return         : - Success or error status:
*                      - 1: if variable was found
*                      - 0: if the variable was not found
*******************************************************************************/
u16 EE_ReadVariable(u16 VirtIdx, u16* Data)
{
char tbuf[20];

	*Data = 0;
	if (!getConfigVoice(EEPROM_FILE, SECTION_NAME, str_voiceName[VirtIdx], tbuf,sizeof(tbuf)))
		return 0;
	if (!getHexWord(tbuf,Data,0))
		return 0;
	return 1;
}

/*******************************************************************************
* Function Name  : EE_WriteVariable
* Description    : Writes/upadtes variable data in EEPROM.
* Input          : - VirtIdx: Index of Variable virtual address
*                  - Data: 16 bit data to be written
* Output         : None
* Return         : - Success or error status:
*                      - 1: on success,
*                      - 0: on failure
*******************************************************************************/
u16 EE_WriteVariable(u16 VirtIdx, u16 Data)
{
char tbuf[20];

	if ((ffindFile(EEPROM_FILE) < 0) && !EE_Init())
		return 0;
	sprintf(tbuf,"%04X",Data);
	setConfigVoice(EEPROM_FILE,SECTION_NAME,str_voiceName[VirtIdx],tbuf);
	return 1;
}
