/***************************************************************************//**
 * @file
 * @brief EEPROM driver for 24AA024 (2Kbit) EEPROM device on the DVK.
 * @author Energy Micro AS
 * @version 1.4.2
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include <stddef.h>

#include "bsp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "i2c.h"
#include "eeprom.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define I2C_EEPSLAVE_ADDR	0x50
#define PAGE_SIZE			4

static xSemaphoreHandle	xSemaphoreMutex;

/*******************************************************************************
 ***************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

 void EEP_Init(void)
 {
 	if (!xSemaphoreMutex)
		xSemaphoreMutex = xSemaphoreCreateMutex();
}

void EEP_Lock(void)
{
	xSemaphoreTake(xSemaphoreMutex,portMAX_DELAY);
}
 
/***************************************************************************//**
 * @brief
 *   Read data from EEPROM.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] addr
 *   I2C address for EEPROM, in 8 bit format, where LSB is reserved
 *   for R/W bit.
 *
 * @param[in] offset
 *   Offset in EEPROM to start reading from.
 *
 * @param[out] data
 *   Location to place read data, must be at least @p len long.
 *
 * @param[in] len
 *   Number of bytes to read.
 *
 * @return
 *   Returns number of bytes read. Less than specified number of bytes is
 *   returned if reading beyond end of EEPROM. Negative value is returned
 *   is some sort of error occurred during read.
 ******************************************************************************/
int EEP_Read(uint16_t offset,void *data,uint16_t len)
{
int res;

	xSemaphoreTake(xSemaphoreMutex,portMAX_DELAY);
	res = I2C_RandRead(I2C_EEPSLAVE_ADDR,offset,2,data,len);
	xSemaphoreGive(xSemaphoreMutex);
	return res;
}

/***************************************************************************//**
 * @brief
 *   Write data to EEPROM.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] addr
 *   I2C address for EEPROM, in 8 bit format, where LSB is reserved
 *   for R/W bit.
 *
 * @param[in] offset
 *   Offset in EEPROM to start writing to.
 *
 * @param[out] data
 *   Location holding data to write, must be at least @p len long.
 *
 * @param[in] len
 *   Number of bytes to write.
 *
 * @return
 *   Returns number of bytes written. Less than specified number of bytes is
 *   returned if writing beyond end of EEPROM. Negative value is returned
 *   is some sort of error occurred during write.
 ******************************************************************************/
#define MAX_EEP_WRITE_ACK_ATTEMPT	10
int EEP_Write(uint16_t offset,const void *data,uint16_t len)
{
uint8_t retry,*pb = (uint8_t *)data;
uint16_t slice;
int res = TRUE;

	xSemaphoreTake(xSemaphoreMutex,portMAX_DELAY);
	while (len > 0)
	{
		slice = PAGE_SIZE - (offset & (PAGE_SIZE-1));
		if (slice > len)
			slice = len;

		if (!I2C_RandWrite(I2C_EEPSLAVE_ADDR,offset,2,pb,slice))
		{
			res = FALSE;
			break;
		}
		
		for (retry = 0; retry < MAX_EEP_WRITE_ACK_ATTEMPT; retry++)
		{
			vTaskDelay(5);
			if (I2C_RandWrite(I2C_EEPSLAVE_ADDR,offset,2,NULL,0))
				break;
		}
		if (retry >= MAX_EEP_WRITE_ACK_ATTEMPT)
		{
			res = FALSE;
			break;
		}
		offset += slice;
		len -= slice;
		pb += slice;
	}
	xSemaphoreGive(xSemaphoreMutex);
	return res;
}
