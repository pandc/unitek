/*
 * Display_128x64.h
 *
 *  Created on: 06/mar/2016
 *      Author: Die go
 */

#ifndef SOURCES_DISPLAY_128X64_H_
#define SOURCES_DISPLAY_128X64_H_

#include "display_interface.h"

void LCD_Set_Column_Address(unsigned char a, unsigned char b);
void LCD_Set_ReMap(unsigned char a, unsigned char b);
void LCD_Set_Row_Address(unsigned char a, unsigned char b);
void LCD_Set_Gray_Scale_Table();
void LCD_Set_VSL(unsigned char a, unsigned char b);
void LCD_Set_DisplayEnh(unsigned char a, unsigned char b);
void LCD_Set_Write_RAM();
void LCD_Display_Setup(void);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_Fill_GRAM(unsigned char data);
void LCD_Fill_GRAM_Alternate_Pixel(void);
void LCD_Fill_ImageRAM(unsigned char data);
void LCDprova(void);
void GetBitmap(void);
void LCD_CopyScreen(void);
void SelectFont(unsigned char font_type);
void LCD_DrawChar(char my_char);
void CleanArea_Ram_and_Screen(unsigned int start_byte_x,unsigned int stop_byte_x ,unsigned int start_y,unsigned int stop_y);
void RigaHoriz(unsigned short xstart,unsigned short  xstop,unsigned short y);
void RigaVertic(unsigned short x_start,unsigned short  y_start,unsigned short y_stop);
void LCD_CopyPartialScreen(unsigned int start_byte_x,unsigned int stop_byte_x ,unsigned int start_y,unsigned int stop_y);
void LCDPrintString(const char* stringtoprint,unsigned char start_x,unsigned char start_y);
void FontPointerInit(void);

#define FIRST_COLUMN 0x1C
#define LAST_COLUMN  0x5B

#define FIRST_ROW 0x00
#define LAST_ROW  0x3f

#define FONT12PT 0
#define FONT10PT 1

extern uint8_t screen_image[1024];

#endif /* SOURCES_DISPLAY_128X64_H_ */
