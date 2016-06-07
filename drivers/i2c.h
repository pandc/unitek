#ifndef __I2C_H
#define	__I2C_H

#define MIN_I2C_FREQ		1000
#define MAX_I2C_FREQ		400000

void I2C_Open(uint32_t freq);
int I2C_RandWrite(uint8_t slave,uint16_t addr,uint8_t addrlen,void *pbuffer, uint16_t len);
int I2C_RandRead(uint8_t slave,uint16_t addr,uint8_t addrlen,void *pbuffer,uint16_t len);
int I2C_Write(uint8_t addr,void *pbuffer, uint16_t len);
int I2C_Read(uint8_t addr,void *pbuffer,uint16_t len);

#endif
