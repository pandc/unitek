#include "bsp.h"

#include "freertos.h"
#include "task.h"

#include "i2c.h"
#include "ad5934.h"

#define AD5934_SLAVE	0x0d

#define CMD_BLOCKREAD	0xa1
#define CMD_BLOCKWRITE	0xa0
#define CMD_SETADDR		0xb0

int AD_Write(uint8_t addr,void *data,uint8_t len)
{
uint8_t buf[2];
uint16_t wbuf;

	if (len == 0)
		return FALSE;
	if (len == 1)
		return AD_WriteByte(addr,*((uint8_t *)data));

	*buf = CMD_SETADDR;
	buf[1] = addr;
	if (!I2C_Write(AD5934_SLAVE,buf,2))
		return FALSE;
	wbuf = CMD_BLOCKWRITE << 8;
	wbuf |= len;
	if (!I2C_RandWrite(AD5934_SLAVE,wbuf,2,data,len))
		return FALSE;
	return TRUE;
}

int AD_WriteByte(uint8_t addr,uint8_t data)
{
uint8_t buf[2];

	*buf = addr;
	buf[1] = data;
	if (!I2C_Write(AD5934_SLAVE,buf,2))
		return FALSE;
	return TRUE;
}

int AD_Read(uint8_t addr,void *data,uint8_t len)
{
uint8_t buf[2];
uint16_t wbuf;

	if (len == 0)
		return FALSE;

	*buf = CMD_SETADDR;
	buf[1] = addr;
	if (!I2C_Write(AD5934_SLAVE,buf,2))
		return FALSE;

	if (len == 1)
	{
		if (!I2C_Read(AD5934_SLAVE,data,1))
			return FALSE;
	}
	else
	{
		wbuf = CMD_BLOCKREAD << 8;
		wbuf |= len;
		if (!I2C_RandRead(AD5934_SLAVE,wbuf,2,data,len))
			return FALSE;
	}
	return TRUE;
}
