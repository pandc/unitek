#include "bsp.h"

#include "freertos.h"
#include "task.h"
#include "semphr.h"

#include "types.h"
#include "io.h"
#include "mspi.h"
#include "ssd1322.h"
#include "display_interface.h"

#define COMMAND_SELECT()	GPIO_ResetBits(DISP_CMDn_PORT,DISP_CMDn_PIN)
#define DATA_SELECT()		GPIO_SetBits(DISP_CMDn_PORT,DISP_CMDn_PIN)

static inline void CHIP_SELECT(void)
{
	MSPI_Lock(SPISETUP_Display);
	GPIO_ResetBits(DISP_CSn_PORT,DISP_CSn_PIN);
}

static inline void CHIP_UNSELECT(void)
{
	GPIO_SetBits(DISP_CSn_PORT,DISP_CSn_PIN);
	MSPI_Unlock();
}

void LCD_Init_Spi(void)
{
	GpioInit(DISP_SCK_PORT,DISP_SCK_PIN,GPIO_Mode_AF_PP,0);
	//GpioInit(DISP_MISO_PORT,DISP_MISO_PIN,GPIO_Mode_AF_PP,0);
	GpioInit(DISP_MOSI_PORT,DISP_MOSI_PIN,GPIO_Mode_AF_PP,0);

	GpioInit(DISP_CSn_PORT,DISP_CSn_PIN,GPIO_Mode_Out_PP,1);		// unselected
	GpioInit(DISP_CMDn_PORT,DISP_CMDn_PIN,GPIO_Mode_Out_PP,1);
	GpioInit(DISP_RSTn_PORT,DISP_RSTn_PIN,GPIO_Mode_Out_PP,0);		// chip reset
	GpioInit(DISP_VCC_ON_PORT,DISP_VCC_ON_PIN,GPIO_Mode_Out_PP,0);	// Display vcc
}

void LCD_Send_Command_Bytes(const uint8_t *d,uint16_t len)
{
	CHIP_SELECT();
	COMMAND_SELECT();
	if (len == 1)
		MSPI_Write(*d);
	else if (len > 1)		
		MSPI_Write_Dma(d,NULL,len,SPIDMAINC_TxInc);
	CHIP_UNSELECT();
}

void LCD_Send_Command(uint8_t cmd,uint8_t data)
{
	LCD_Send_Reg(cmd);
	LCD_Send_Data(&data,1,TRUE);
}

void LCD_Send_Reg(uint8_t r)
{
	CHIP_SELECT();
	COMMAND_SELECT();
	MSPI_Write(r);
	CHIP_UNSELECT();
}

void LCD_Send_Data(const uint8_t *d,uint16_t c,uint8_t inc)
{
	CHIP_SELECT();
	DATA_SELECT();
	if (c == 1)
		MSPI_Write(*d);
	else if (c > 1)
		MSPI_Write_Dma(d,NULL,c,(inc)? SPIDMAINC_TxInc: 0);
	//COMMAND_SELECT();
	CHIP_UNSELECT();
}
