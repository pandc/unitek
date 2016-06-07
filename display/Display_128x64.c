/*
 * Display_128x64.c
 *
 *  Created on: 02/mar/2016
 *      Author: Die go
 */

#include <string.h>

#include "bsp.h"
#include "io.h"

#include "display_interface.h"
#include "ssd1322.h"
#include "display_128x64.h"

//coordinate_struct_type coordinate_struct1;
//bitmap_struct_type mybmp_struct1;

uint8_t array_line[128];//un array abbastanza grande da contenere la + lunga linea possibile

unsigned char char_size=FONT12PT;

//***************************************************************************************
void LCD_CopyScreen(void)
{
uint16_t screen_mem_index,bit,row,col;

	LCD_Set_Column_Address(FIRST_COLUMN,LAST_COLUMN);//63 colonne composte però da 2 byte ciascuna
	LCD_Set_Row_Address(FIRST_ROW,LAST_ROW);
	LCD_Set_Write_RAM();

	for(screen_mem_index = row = 0; row<64;row++)//NB è solo il calcolo del n°di pixel...lui procede dalla 1a riga alla 1a all'ultima colonna,e poi con la 2nda riga
	{
		for(col = 0;col<16;col++,screen_mem_index++)
		{
			for (bit = 0; bit < 8; bit++)
				array_line[col*8+bit] =  (screen_image[screen_mem_index] & (1 << bit))? 0xff: 0;
		}
		LCD_Send_Data(array_line,128,TRUE);
	}
}

//***************************************************************************************
void LCD_Fill_GRAM_Alternate_Pixel(void)
{
//uint8_t data_1=0xFF,data_2=0x08;
//uint8_t data_3=0x4F,data_4=0x08;
uint16_t row,col;

	LCD_Set_Column_Address(FIRST_COLUMN,LAST_COLUMN);//63 colonne composte però da 2 byte ciascuna
	LCD_Set_Row_Address(FIRST_ROW,LAST_ROW);
	LCD_Set_Write_RAM();

	for(row = 0; row<32;row++)//NB è solo il calcolo del n°di pixel...lui procede dalla 1a riga alla 1a all'ultima colonna,e poi con la 2nda riga
	{
		for(col = 0;col<64;col++)
		{
			array_line[col*2] = 0xff;		// data_1
			array_line[col*2+1] = 0x08;		// data_2
		}
		LCD_Send_Data(array_line,128,TRUE);

		for(col = 0;col<64;col++)
		{
			array_line[col*2] = 0x4f;		// data_3
			array_line[col*2+1] = 0x08;		// data_4
		}
		LCD_Send_Data(array_line,128,TRUE);
	}
}

//***************************************************************************************
void LCDprova(void)
{

}

//***************************************************************************************

//***************************************************************************************



