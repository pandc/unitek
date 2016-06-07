#include <string.h>

#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"

#include "display_interface.h"
#include "ssd1322.h"
#include "display_128x64.h"

uint8_t screen_image[1024];

//***************************************************************************************
void LCD_Set_Gray_Scale_Table()
{
	LCD_Send_Reg(0xB8);			    //   Set Gray Scale Table
	LCD_Send_Data("\x01\x0d\x19\x25\x31\x3d\x49\x55\x61\x6d\x79\x85\x91\x9d\xa9\xb4",16,TRUE);
#if 0
	LCD_Send_Data(0x01);			//   Gray Scale Level 1
	LCD_Send_Data(0x0D);			//   Gray Scale Level 2
	LCD_Send_Data(0x19);			//   Gray Scale Level 3
	LCD_Send_Data(0x25);			//   Gray Scale Level 4
	LCD_Send_Data(0x31);			//   Gray Scale Level 5
	LCD_Send_Data(0x3D);			//   Gray Scale Level 6
	LCD_Send_Data(0x49);			//   Gray Scale Level 7
	LCD_Send_Data(0x55);			//   Gray Scale Level 8
	LCD_Send_Data(0x61);			//   Gray Scale Level 9
	LCD_Send_Data(0x6D);			//   Gray Scale Level 10
	LCD_Send_Data(0x79);			//   Gray Scale Level 11
	LCD_Send_Data(0x85);			//   Gray Scale Level 12
	LCD_Send_Data(0x91);			//   Gray Scale Level 13
	LCD_Send_Data(0x9D);			//   Gray Scale Level 14
	LCD_Send_Data(0xA9);			//   Gray Scale Level 15
	LCD_Send_Data(0xB4);			//   Gray Scale Level 15
#endif
	LCD_Send_Reg(0x00);             //   Enable Gray Scale
}
//***************************************************************************************

void LCD_Init(void)
{
	CHIP_RESET();
	vTaskDelay(pdMS_TO_TICKS(150));

	memset(screen_image,0,sizeof(screen_image));

	CHIP_UNRESET();
	vTaskDelay(pdMS_TO_TICKS(10));

	LCD_Send_Reg(CMD_DISPOFF);
	//Set Display clock
	LCD_Send_Command(CMD_SETCLKFREQ,0x91);
	//Set Multiplex ratio
	LCD_Send_Command		(CMD_VDDSEL,0x01);
	LCD_Send_Command		(CMD_DISPOFFSET,0x00);
	LCD_Send_Command		(CMD_SETSTART,0x00);
	LCD_Send_Command		(CMD_COMLOCK,0x12);
	LCD_Set_Column_Address(FIRST_COLUMN,LAST_COLUMN);
	LCD_Set_ReMap			(DISPLAY_REVERSED,0x11);
	LCD_Send_Command		(CMD_CONTRSTCUR,0xAF);
	LCD_Send_Command		(CMD_MSTCONTRST,0x0F);
	LCD_Send_Command		(CMD_MUXRATIO,0x3F);
	//LCD_Set_Gray_Scale_Table();
	LCD_Set_Row_Address		(FIRST_ROW,LAST_ROW);
	LCD_Send_Command		(CMD_PHASELEN,0xFF);
	LCD_Set_VSL				(0xA0,0xFD);
	LCD_Send_Command		(CMD_SETGPIO,0x00);
	LCD_Send_Command		(CMD_SECPRECHRG,0x08);
	LCD_Send_Command		(CMD_PRECHRGVOL,0x1F);
	LCD_Send_Command		(CMD_SETVCOMH,0x07);
	LCD_Set_DisplayEnh		(0x82,0x20);
	LCD_Send_Reg			(CMD_DISPNORM);
	LCD_Send_Reg			(CMD_EXIT_PARTIAL);
	//LCD_Send_Command		(0x14,0x11);

	LCD_Clear();
	
	CHIP_VCC_ON();
	vTaskDelay(pdMS_TO_TICKS(150));

	//Set Display on/off
	LCD_Send_Reg(CMD_DISPON);
	vTaskDelay(pdMS_TO_TICKS(150));
}
//***************************************************************************************
void LCD_Display_Setup(void)
{
	CHIP_VCC_OFF();
	LCD_Init_Spi();

	LCD_Init();
	LCD_Fill_ImageRAM(0);
	LCD_Fill_GRAM(0);
}

//***************************************************************************************
void LCD_Clear(void)
{
	LCD_Fill_GRAM(0);
}

//***************************************************************************************
void LCD_Fill_GRAM(uint8_t data)
{
uint16_t row;

	LCD_Set_Column_Address(FIRST_COLUMN,LAST_COLUMN);//63 colonne composte però da 2 byte ciascuna
	LCD_Set_Row_Address(FIRST_ROW,LAST_ROW);
	LCD_Set_Write_RAM();

	//NB è solo il calcolo del n°di pixel...lui procede dalla 1a riga alla 1a all'ultima colonna,e poi con la 2nda riga
	for (row = 0; row < 64; row++)
		LCD_Send_Data(&data,128,FALSE);
}
//***************************************************************************************
void LCD_Fill_ImageRAM(uint8_t data)
{
	memset(screen_image,data,sizeof(screen_image));
}
//***************************************************************************************
void LCD_Set_Write_RAM()
{
	LCD_Send_Reg(0x5C);			// Internal DDRAM Memory Access
}
//***************************************************************************************
void LCD_Set_Column_Address(uint8_t a, uint8_t b)
{
uint8_t d[2];

	LCD_Send_Reg(0x15);
	d[0] = a;
	d[1] = b;
	LCD_Send_Data(d,2,TRUE);
}
//***************************************************************************************
void LCD_Set_Row_Address(uint8_t a, uint8_t b)
{
uint8_t d[2];

	LCD_Send_Reg(0x75);
	d[0] = a;
	d[1] = b;
	LCD_Send_Data(d,2,TRUE);
}
//***************************************************************************************
void LCD_Set_ReMap(uint8_t a, uint8_t b)
{
uint8_t d[2];

	LCD_Send_Reg(0xa0);
	d[0] = a;
	d[1] = b;
	LCD_Send_Data(d,2,TRUE);
}
//***************************************************************************************
void LCD_Set_VSL(uint8_t a, uint8_t b)
{
uint8_t d[2];

	LCD_Send_Reg(0xb4);
	d[0] = a;
	d[1] = b;
	LCD_Send_Data(d,2,TRUE);
}
//***************************************************************************************
void LCD_Set_DisplayEnh(uint8_t a, uint8_t b)
{
uint8_t d[2];

	LCD_Send_Reg(0xd1);
	d[0] = a;
	d[1] = b;
	LCD_Send_Data(d,2,TRUE);
}

//***************************************************************************************
