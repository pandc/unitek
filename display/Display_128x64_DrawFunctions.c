/*
 * Display_128x64_DrawFunctions.c
 *
 *  Created on: 19/mar/2016
 *      Author: Administrator
 */

/* Including needed modules to compile this module/procedure */
#include "bsp.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"


#include "FontsCalibri.h"

coordinate_struct_type coordinate_struct1;
bitmap_struct_type mybmp_struct1,mybmp_struct2;

extern uint8_t array_line[128];
extern unsigned char screen_image[1024];
extern unsigned char char_size;

unsigned int bmp_offset,bmp_offset_successivo;//offset in flash di ogni bmp
unsigned char height_font;
unsigned char width_font;

FONT_INFO* font_info_pt_table[16];
//***************************************************************************************
void RigaHoriz(unsigned short xstart,unsigned short  xstop,unsigned short y)
{
	    unsigned short x8start;
	    unsigned short screen_mem_index,j;


	    x8start=xstart/8;//vedo in quale grupo di 8 pixel orizzontali parto
	    x8start +=y*16;
	    screen_mem_index=xstart & 7;

		for(j=xstart;j<(xstop+1);j++)
		{
				screen_image[x8start] |=1<<screen_mem_index;
				screen_mem_index++;
				if(screen_mem_index>7)
				{
					screen_mem_index=0;
					x8start++;
				}
		}
}
//***************************************************************************************
void RigaVertic(unsigned short x_start,unsigned short  y_start,unsigned short y_stop)
{
	    unsigned short x8start;
	    unsigned short i,j;


	    //xstop =xstart+1;//coordinate_struct1.thick;
	    x8start=x_start/8;//vedo in quale grupo di 8 pixel orizzontali parto
	    x8start +=y_start*16;
	    i=x_start & 7;

		for(j=y_start;j<(y_stop+1);j++)
		{
				screen_image[x8start] |=1<<i;

				x8start+=16;

		}
}
//***************************************************************************************
void CleanRigaVertic(unsigned short x_start,unsigned short  y_start,unsigned short y_stop)
{
	    unsigned short x8start;
	    unsigned short i,j;


	    //xstop =xstart+1;//coordinate_struct1.thick;
	    x8start=x_start/8;//vedo in quale grupo di 8 pixel orizzontali parto
	    x8start +=y_start*16;
	    i=x_start & 7;

		for(j=y_start;j<(y_stop+1);j++)
		{
				screen_image[x8start] &=~(1<<i);

				x8start+=16;

		}
}

//***************************************************************************************
void LCD_CleanArea(unsigned int x_start,unsigned int x_stop,unsigned int y_start,unsigned int y_stop)
{
	unsigned int diff_x,diff_y,diff_xy;
	uint8_t b = 0;

	diff_x=x_stop-x_start;
	diff_y=y_stop-y_start;
	diff_xy=(diff_x+1)*diff_y/8;//a costo di ripetere la scrittura...per non analizzare bit a bit


	x_start+=FIRST_COLUMN;
	x_stop +=FIRST_COLUMN;
	LCD_Set_Column_Address(x_start, x_stop-1);
	LCD_Set_Row_Address   (y_start, y_stop-1);//+coordinate_struct1.thick);
	LCD_Set_Write_RAM();

	LCD_Send_Data(&b,diff_xy,FALSE);

	//screen_image[]

}
//***************************************************************************************
//***************************************************************************************
void LCD_CopyPartialScreen(unsigned int start_byte_x,unsigned int stop_byte_x ,unsigned int start_y,unsigned int stop_y)
{
  static unsigned int screen_mem_index,bit,row,col_start_byte,bits_restanti_start,col_stop_byte,bits_restanti_stop;
  uint16_t col,col_stop,i;

  start_byte_x /=2;
  stop_byte_x  /=2;

  col=(start_byte_x+FIRST_COLUMN);
  col_stop=(stop_byte_x+FIRST_COLUMN) - 1;
  
  if(stop_byte_x>128)stop_byte_x=128;//per non sforare
  if(stop_y> 64)stop_y= 64;//per non sforare

  LCD_Set_Column_Address(col,col_stop);
  //LCD_Set_Column_Address(col,col_stop);//63 colonne composte però da 2 byte ciascuna
  LCD_Set_Row_Address(start_y,stop_y-1);




/*
   screen_mem_index=1000;
   while(screen_mem_index--)
   {
	   LCD_Send_Data(0x85);
   }
   col=start_byte_x*8;
   col_stop=stop_byte_x*8;*/
 //posso usare 64 valori per 128 righe per adesso provo solo INTERVALLI PARI es 0-14 ecc
  screen_mem_index=start_byte_x+start_y*16;//indirizzo di partenza del byte  memoria video da copiare,attenzione che potrei partire dal 4° bit

  //potendo suddividere in 64 righe da 2 pixel alla volta se dico riga 2 vuol dire dal 4° pixel
  //quindi se  voglio copiare da colonna 2 a colonna 10 devo partire da screen image al byte 0 per il 2(che è il 4° pixel )  e finire col byte numero 3 per 10 che è il 20à pixel


  //per ogni riga che è rappresentata da 16 byte devo aumentare l'indice di 16 rispetto al valore di partenza della riga precedente

  //quindi se parto da colonna 4 riga 0 devo fare screen_mem_index=4/8=0  nell'interno del quale devo testare partendo da bit 4 e saltando i primi 3 bit
  //attenzione che il bit 0 nel byte è quello della mia colonna 1

  //ma purtroppo per questa rottura di coglioni dei 2 pixel alla volta devo avere sempre dei restanti PARI
  start_byte_x*=2;

  bits_restanti_start=start_byte_x %8;

  stop_byte_x*=2;
  col_stop_byte=stop_byte_x/8;//ossia stop_byte_x*2/8
  bits_restanti_stop=stop_byte_x %8;
  //if(bits_restanti_stop)col_stop_byte++;
  LCD_Set_Write_RAM();

  for( row = start_y; row<stop_y;row++)//NB è solo il calcolo del n°di pixel...lui procede dalla 1a riga alla 1a all'ultima colonna,e poi con la 2nda riga
  {
	  col_start_byte=start_byte_x/8;//ossia start_byte_x*2/8
	  screen_mem_index=col_start_byte+row*16;

	i = 0;
    if (bits_restanti_start)
    {
		for (bit = bits_restanti_start; bit < 8; bit++,i++)
			array_line[i] = (screen_image[screen_mem_index] & (1 << bit))? 0xff: 0;
    	col_start_byte++;
    	screen_mem_index++;
    }

    //for( col = 0;col<3;col++)
    for(col = col_start_byte;col<col_stop_byte;col++)//NB ogni colonna delle 63 possibili è composta da 2 pixel cioè 2 bit di ogni elemento di screen image
    {						 //quindi devo scegliere un numero di colonne multiplo di 8 bit es 8 colonne=16 bit =2 bytes di screen_image
		for (bit = 0; bit < 8; bit++,i++)
    		array_line[i] = (screen_image[screen_mem_index] & (1 << bit))? 0xff: 0;
    	screen_mem_index++;
    }

    if(bits_restanti_stop)
	{
		for (bit = 0; bit < bits_restanti_stop; bit++,i++)
    		array_line[i] = (screen_image[screen_mem_index] & (1 << bit))? 0xff: 0;
	}
	LCD_Send_Data(array_line,i,TRUE);
  }
}
/**************************************************************************/
void LCDPrintString(const char* stringtoprint,unsigned char start_x,unsigned char start_y)
{
	mybmp_struct1.start_x=start_x;
	mybmp_struct1.start_y=start_y;

	while(*stringtoprint)
	{
		LCD_DrawChar(*stringtoprint++);
	}
}

//***************************************************************************************
void CleanArea_Ram_and_Screen(unsigned int start_byte_x,unsigned int stop_byte_x ,unsigned int start_y,unsigned int stop_y)
{
	 static unsigned int screen_mem_index,bit,col,col_stop,row,col_start_byte,bits_restanti_start,col_stop_byte,bits_restanti_stop;
	 uint16_t i;
         start_byte_x /=2;
         stop_byte_x  /=2;

          if(stop_byte_x>128)stop_byte_x=128;//per non sforare
          if(stop_y> 64)stop_y= 64;//per non sforare

	  col		=(start_byte_x+FIRST_COLUMN);
	  col_stop	=(stop_byte_x +FIRST_COLUMN) - 1;


	  LCD_Set_Column_Address(col,col_stop);
	  //LCD_Set_Column_Address(col,col_stop);//63 colonne composte però da 2 byte ciascuna
	  LCD_Set_Row_Address(start_y,stop_y-1);




	/*
	   screen_mem_index=1000;
	   while(screen_mem_index--)
	   {
		   LCD_Send_Data(0x85);
	   }
	   col=start_byte_x*8;
	   col_stop=stop_byte_x*8;*/
	  //posso usare 64 valori per 128 righe per adesso provo solo INTERVALLI PARI es 0-14 ecc
	  screen_mem_index=start_byte_x+start_y*16;//indirizzo di partenza del byte  memoria video da copiare,attenzione che potrei partire dal 4° bit

	  //potendo suddividere in 64 righe da 2 pixel alla volta se dico riga 2 vuol dire dal 4° pixel
	  //quindi se  voglio copiare da colonna 2 a colonna 10 devo partire da screen image al byte 0 per il 2(che è il 4° pixel )  e finire col byte numero 3 per 10 che è il 20à pixel


	  //per ogni riga che è rappresentata da 16 byte devo aumentare l'indice di 16 rispetto al valore di partenza della riga precedente

	  //quindi se parto da colonna 4 riga 0 devo fare screen_mem_index=4/8=0  nell'interno del quale devo testare partendo da bit 4 e saltando i primi 3 bit
	  //attenzione che il bit 0 nel byte è quello della mia colonna 1

	  //ma purtroppo per questa rottura di coglioni dei 2 pixel alla volta devo avere sempre dei restanti PARI
	  start_byte_x*=2;

	  bits_restanti_start=start_byte_x %8;

	  stop_byte_x*=2;
	  col_stop_byte=stop_byte_x/8;//ossia stop_byte_x*2/8
	  bits_restanti_stop=stop_byte_x %8;
	  //if(bits_restanti_stop)col_stop_byte++;
	  LCD_Set_Write_RAM();

	  for( row = start_y; row<stop_y;row++)//NB è solo il calcolo del n°di pixel...lui procede dalla 1a riga alla 1a all'ultima colonna,e poi con la 2nda riga
	  {
		  i = 0;
		  col_start_byte=start_byte_x/8;//ossia start_byte_x*2/8
		  screen_mem_index=col_start_byte+row*16;

	    if(bits_restanti_start)
	    {
			for (bit = bits_restanti_start; bit < 8; bit++,i++)
			{
				screen_image[screen_mem_index] &= ~(1 << bit);
				array_line[i] = 0;
			}
	    	col_start_byte++;
	    	screen_mem_index++;
	    }

	    //for( col = 0;col<3;col++)
	    for( col = col_start_byte;col<col_stop_byte;col++)//NB ogni colonna delle 63 possibili è composta da 2 pixel cioè 2 bit di ogni elemento di screen image
	    {						 //quindi devo scegliere un numero di colonne multiplo di 8 bit es 8 colonne=16 bit =2 bytes di screen_image
	     	bit=0;
			for (bit = 0; bit < 8; bit++,i++)
			{
				screen_image[screen_mem_index] &= ~(1 << bit);
				array_line[i] = 0;
			}
	    	screen_mem_index++;
	    }

	    if(bits_restanti_stop)
		{
			for (bit = 0; bit < bits_restanti_stop; bit++,i++)
			{
				screen_image[screen_mem_index] &= ~(1 << bit);
				array_line[i] = 0;
			}
		}
		LCD_Send_Data(array_line,i,TRUE);
	  }
}
//******************************************************************************************************************************************************************************
void CleanAreaScreenOnly(unsigned int start_byte_x,unsigned int stop_byte_x ,unsigned int start_y,unsigned int stop_y)
{
   static unsigned int bit,col,col_stop,row,col_start_byte,bits_restanti_start,col_stop_byte,bits_restanti_stop;
   uint16_t i;
   start_byte_x /=2;
   stop_byte_x  /=2;

    if(stop_byte_x>128)stop_byte_x=128;//per non sforare
    if(stop_y> 64)stop_y= 64;//per non sforare

    col		=(start_byte_x+FIRST_COLUMN);
    col_stop	=(stop_byte_x +FIRST_COLUMN) - 1;


    LCD_Set_Column_Address(col,col_stop);
    //LCD_Set_Column_Address(col,col_stop);//63 colonne composte però da 2 byte ciascuna
    LCD_Set_Row_Address(start_y,stop_y-1);


    

    start_byte_x*=2;

    bits_restanti_start=start_byte_x %8;

    stop_byte_x*=2;
    col_stop_byte=stop_byte_x/8;//ossia stop_byte_x*2/8
    bits_restanti_stop=stop_byte_x %8;
    //if(bits_restanti_stop)col_stop_byte++;
    LCD_Set_Write_RAM();

    for( row = start_y; row<stop_y;row++)//NB è solo il calcolo del n°di pixel...lui procede dalla 1a riga alla 1a all'ultima colonna,e poi con la 2nda riga
    {
      i = 0;
      col_start_byte=start_byte_x/8;//ossia start_byte_x*2/8


      if(bits_restanti_start)
      {
          for (bit = bits_restanti_start; bit < 8; bit++,i++)
          {
                  
                  array_line[i] = 0;
          }
          col_start_byte++;

      }

      //for( col = 0;col<3;col++)
      for( col = col_start_byte;col<col_stop_byte;col++)//NB ogni colonna delle 63 possibili è composta da 2 pixel cioè 2 bit di ogni elemento di screen image
      {						 //quindi devo scegliere un numero di colonne multiplo di 8 bit es 8 colonne=16 bit =2 bytes di screen_image
          //bit=0;
          for (bit = 0; bit < 8; bit++,i++)
          {
                  
                  array_line[i] = 0;
          }

      }

      if(bits_restanti_stop)
          {
                  for (bit = 0; bit < bits_restanti_stop; bit++,i++)
                  {
                          
                          array_line[i] = 0;
                  }
          }
          LCD_Send_Data(array_line,i,TRUE);
    }
  
  
}
//******************************************************************************************************************************************************************************
void GetBitmap(void)
{
	unsigned short righe,colonne,screen_mem_index,xo1,xo2;
	const unsigned char* bmpt;
	unsigned short tempshift;



	xo1=mybmp_struct2.start_x/8;
	xo2=mybmp_struct2.start_x  %8;

	bmpt=mybmp_struct2.bmp_pointer;
    screen_mem_index=0;

	for(righe=0;righe<mybmp_struct2.righe;righe++)
	{
		screen_mem_index=(righe + mybmp_struct2.start_y)*16;
		for(colonne=0; colonne< mybmp_struct2.colonne;colonne++)
		{
			tempshift=*bmpt++;
			tempshift<<=xo2;//al massimo lo shifta di 7
			screen_image[screen_mem_index+xo1]  |=(tempshift & 0xFF);
			screen_image[screen_mem_index+1+xo1] |=(tempshift>> 8);
			screen_mem_index++;
		}
	}
}
//***************************************************************************************
void FontPointerInit(void)
{
	//cambria_10ptFontInfo.p_character_descriptor=cambria_10ptDescriptors;
	//cambria_10ptFontInfo.p_character_bitmaps   =cambria_10pt_bmp;

	calibri_10ptFontInfo.p_character_descriptor=calibri_10ptDescriptors;
	calibri_10ptFontInfo.p_character_bitmaps   =calibri_10pt_bmp;

	calibri_20ptFontInfo.p_character_descriptor=calibri_20ptDescriptors;
	calibri_20ptFontInfo.p_character_bitmaps   =calibri_20pt_bmp;







	//font_info_pt_table[CAMBRIA_10]	   =&cambria_10ptFontInfo;
	font_info_pt_table[CALIBRI_10]	   =&calibri_10ptFontInfo;
	font_info_pt_table[CALIBRI_20]	   =&calibri_20ptFontInfo;
}
/**************************************************************************/
void SelectFont(unsigned char font_type)
{
   //seleziono un font scegliendo una font_info_pt_table che punta alla [x]
   //FONT INFO che a sua volta contiene una FONT_CHAR_INFO

	//questa font_info_pt_table la passo alla struttura bitmap,che è sempre quella e che
	//passo alle varie funzioni





	
	mybmp_struct1.bmp_pointer	= font_info_pt_table[font_type]->p_character_bitmaps;
	height_font				 	= font_info_pt_table[font_type]->p_character_descriptor->height; //font_info_pt_table[font_type]->p_character_descriptor[0].height;//deve essere un numero tipo 13,20 ecc
	mybmp_struct1.font_info_pt  = font_info_pt_table[font_type];
	//width_font					= font_info_pt_table[font_type]->p_character_descriptor->width;
	//if(font_type==CAMBRIA_10)width_font=6;
	if(font_type==CALIBRI_10)width_font=5;
	if(font_type==CALIBRI_20)width_font=11;




}
//***************************************************************************************
void LCD_DrawChar(char my_char)
{
	unsigned short righe,colonne_bytes,screen_mem_index,xo1,xo2;
	const unsigned char* bmpt;
	unsigned short tempshift;
	unsigned char temp,j,temp_image;//fa corrispondere ogni char con la bitmap
	unsigned char char_offset;
	unsigned char bit_shift;



	char_offset=my_char -32;//per avere offset nella bitmap table generata
	//if(my_char>'`')char_offset--;
	bmp_offset				= mybmp_struct1.font_info_pt->p_character_descriptor[char_offset].offset;					//cambria_10ptDescriptors[char_offset].offset;//per puntare alla descrizione necessaria es {5,340} nell'array di descrizioni
	bmp_offset_successivo	= mybmp_struct1.font_info_pt->p_character_descriptor[char_offset+1].offset;



	xo1=mybmp_struct1.start_x/8;
	xo2=mybmp_struct1.start_x  %8;

	bmpt=mybmp_struct1.bmp_pointer;
	bmpt+=bmp_offset;
    screen_mem_index=0;


    mybmp_struct1.colonne =(bmp_offset_successivo-bmp_offset)/height_font;
    mybmp_struct1.righe=height_font;//



    for(righe=0;righe< mybmp_struct1.righe;righe++)
    	{
    		screen_mem_index=(righe + mybmp_struct1.start_y)*16;



    		for(colonne_bytes=0; colonne_bytes<  mybmp_struct1.colonne;colonne_bytes++)
    		{
    			temp_image=0;
    			temp=*bmpt++;
    			for(j=0;j<8;j++)
    			{
    				bit_shift=1<<j;
    				if(temp & bit_shift)temp_image|=bit_shift;
    			}
    			//screen_image[screen_mem_index]=temp_image;

    			tempshift=temp_image;
    			tempshift<<=xo2;//al massimo lo shifta di 7
    			screen_image[screen_mem_index+xo1]   |=(tempshift & 0xFF);
    			screen_image[screen_mem_index+1+xo1] |=(tempshift>> 8);
    			screen_mem_index++;



    		}
    	}

    mybmp_struct1.start_x +=mybmp_struct1.font_info_pt->p_character_descriptor[char_offset].width;
    mybmp_struct1.start_x+=1;
}
//***************************************************************************************
void DisegnaCornice (void)
{
	RigaHoriz(0,127,0);
	RigaHoriz(0,127,63);
	RigaVertic(0,0,63);
	RigaVertic(127,0,63);
}


