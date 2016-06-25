/*
 * Conversioni.c
 *
 *  Created on: Jul 17, 2013
 *      Author: User
 */
#include "bsp.h"
#include "my_definitions.h"
unsigned char unit,decine,cent,migl,decmigl,cenmigl,Milioni,decM;
unsigned char number_to_print[8];

extern bitmap_struct_type mybmp_struct1;
extern unsigned char width_font;


#include "Conversioni.h"
#include "Display_128x64.h"
//------------------------------------------------------------------------------------------------------------
//uso shift and add3
void BinToBCDisp(unsigned int mybin,unsigned char decimali,unsigned char start_x,unsigned char start_y)
{
 unsigned char i=32;
 unsigned int j=0x80000000;
 unsigned int myshort=0;
 unsigned char lungh=1;

 unsigned char local_width_font,corrected_dot_local_width_font;

 local_width_font=width_font;
 corrected_dot_local_width_font=width_font/2;

 mybmp_struct1.start_x=start_x;

 if(decimali==0)
 {      //a seconda della lunghezza della cifra da stampare mi sposta l'offset a dx,se cifra++ offset--
	if(mybin<10)mybmp_struct1.start_x=start_x + 3*local_width_font+corrected_dot_local_width_font;
	else
	{
		if(mybin<100)mybmp_struct1.start_x=start_x + 2*local_width_font+corrected_dot_local_width_font;
		else
		{
			if(mybin<1000)mybmp_struct1.start_x=start_x + 1*local_width_font+corrected_dot_local_width_font;
		}

	}
 }

 if(decimali==1)
 {
 	if(mybin<100)mybmp_struct1.start_x=start_x + 2*local_width_font;
 	else
 	{
 		if(mybin<1000)mybmp_struct1.start_x=start_x + 1*local_width_font;
 		else
 		{
 			//if(mybin<1000)mybmp_struct1.start_x=start_x;
 		}

 	}
 }


 if(decimali==2)
 {
  	if(mybin<1000)mybmp_struct1.start_x=start_x + 1*local_width_font;
  	else
  	{
  		/*
  		if(mybin<100)mybmp_struct1.start_x=start_x + 1*local_width_font;
  		else
  		{
  			//if(mybin<1000)mybmp_struct1.start_x=start_x;
  		}*/

  	}
  }


 mybmp_struct1.start_y=start_y;

 while (i)
  { 
    /*
	Shift and Add-3 Algorithm

	1Shift the binary number left one bit.
	If 8 shifts have taken place, the BCD number is in the Hundreds, Tens, and Units column.
	If the binary value in any of the BCD columns is 5 or greater, add 3 to that value in that BCD column.
	Go to 1.
	   */
	 if((myshort & 0x000F)>0x0004)myshort+=0x0003;	
     if((myshort & 0x00F0)>0x0040)myshort+=0x0030;
     if((myshort & 0x0F00)>0x0400)myshort+=0x0300;	
     if((myshort & 0xF000)>0x4000)myshort+=0x3000;
     if((myshort & 0x000F0000)>0x00040000)myshort+=0x00030000;	
     if((myshort & 0x00F00000)>0x00400000)myshort+=0x00300000;
     if((myshort & 0x0F000000)>0x04000000)myshort+=0x03000000;	
     if((myshort & 0xF0000000)>0x40000000)myshort+=0x30000000;
     
     
     myshort<<=1;
     //shifta msb del binario in lsb del BCD
     if(mybin & j)
     {
      myshort |=1;
     }

     i--;
     j>>=1;
   }

 unit   =(myshort & 0x000F);
 decine =( myshort >>4) & 0xF;
 cent   =(myshort >>8)  & 0xF;
 migl   =(myshort >>12) & 0xF;
 decmigl=(myshort >>16) & 0xF;
 cenmigl=(myshort >>20) & 0xF;
 Milioni=(myshort >>24) & 0xF;
 decM   =(myshort >>28) & 0xF;


 if(decimali==0)
 {
	if( decine)lungh=2;
	else 	   lungh=1;
	if(   cent)lungh=3;
	if(   migl)lungh=4;
	if(decmigl)lungh=5;
	if(cenmigl)lungh=6;
	if(Milioni)lungh=7;
	if(   decM)lungh=8;
 }

 if(decimali==1)
  {
 	//if( decine)lungh=2;
 	if(   cent)lungh=3;
 	else	   lungh=2;
 	if(   migl)lungh=4;
 	if(decmigl)lungh=5;
 	if(cenmigl)lungh=6;
 	if(Milioni)lungh=7;
 	if(   decM)lungh=8;
  }

 if(decimali==2)
 {
 	//if( decine)lungh=2;
 	if(   migl)lungh=4;
 	else	   lungh=3;
 	if(decmigl)lungh=5;
 	if(cenmigl)lungh=6;
 	if(Milioni)lungh=7;
 	if(   decM)lungh=8;
  }
 //else lungh=decimali;
 

 if(lungh>7)LCD_DrawChar(0x30 + decM);
 if(lungh>6)LCD_DrawChar(0x30 + Milioni);
 if(lungh>5)LCD_DrawChar(0x30 + cenmigl);
 if(lungh>4)LCD_DrawChar(0x30 + decmigl);
 if(lungh>3)LCD_DrawChar(0x30 + migl);
 if(lungh>2)LCD_DrawChar(0x30 + cent);
 if(decimali==DUE_DECIMALI)LCD_DrawChar('.');
 if(lungh>1)LCD_DrawChar(0x30 + decine);
 if(decimali==UN_DECIMALE)LCD_DrawChar('.');
            LCD_DrawChar(0x30 + unit);
}
//------------------------------------------------------------------------------------------------------------
void BinToBCDTimeDate(unsigned int mybin,unsigned char neg_digit)
{
	unsigned char i=8;
	unsigned char j=0x80;
	unsigned int myshort=0;
	
	if(mybin>0x3FF)mybin-=2000;

	while (i)
	{ 
	    /*
		Shift and Add-3 Algorithm

		1Shift the binary number left one bit.
		If 8 shifts have taken place, the BCD number is in the Hundreds, Tens, and Units column.
		If the binary value in any of the BCD columns is 5 or greater, add 3 to that value in that BCD column.
		Go to 1.
		   */
		 if((myshort & 0x000F)>0x0004)myshort+=0x0003;	
	     if((myshort & 0x00F0)>0x0040)myshort+=0x0030;
	     
	     
	     
	     myshort<<=1;
	     //shifta msb del binario in lsb del BCD
	     if(mybin & j)
	     {
	      myshort |=1;
	     }

	     i--;
	     j>>=1;
	}
	
	 unit   =(myshort & 0x000F);
	 decine =( myshort >>4) & 0xF;
	 
	 if(neg_digit )
	 {
		// LCD_DrawNegChar(0x30 +  decine);
		 //LCD_DrawNegChar(0x30 +  unit);
	 }
	 else		    
	 {
		 LCD_DrawChar(0x30 +  decine);
		 LCD_DrawChar(0x30 +  unit);
	 }
	
	
}
//------------------------------------------------------------------------------------------------------------
//  SERVE per es per stampare da seriale
//------------------------------------------------------------------------------------------------------------
void BinToBCD(unsigned int mybin)
{/*
 unsigned char i=32;
 unsigned int j=0x80000000;
 unsigned int myshort=0;
 //unsigned char lungh;

 while (i)
  {

	Shift and Add-3 Algorithm

	1Shift the binary number left one bit.
	If 8 shifts have taken place, the BCD number is in the Hundreds, Tens, and Units column.
	If the binary value in any of the BCD columns is 5 or greater, add 3 to that value in that BCD column.
	Go to 1.

	 if((myshort & 0x000F)>0x0004)myshort+=0x0003;
     if((myshort & 0x00F0)>0x0040)myshort+=0x0030;
     if((myshort & 0x0F00)>0x0400)myshort+=0x0300;
     if((myshort & 0xF000)>0x4000)myshort+=0x3000;
     if((myshort & 0x000F0000)>0x00040000)myshort+=0x00030000;
     if((myshort & 0x00F00000)>0x00400000)myshort+=0x00300000;
     if((myshort & 0x0F000000)>0x04000000)myshort+=0x03000000;
     if((myshort & 0xF0000000)>0x40000000)myshort+=0x30000000;


     myshort<<=1;
     //shifta msb del binario in lsb del BCD
     if(mybin & j)
     {
      myshort |=1;
     }

     i--;
     j>>=1;
   }

 unit   =(myshort & 0x000F);
 decine =( myshort >>4) & 0xF;
 cent   =(myshort >>8)  & 0xF;
 migl   =(myshort >>12) & 0xF;
 decmigl=(myshort >>16) & 0xF;
 cenmigl=(myshort >>20) & 0xF;
 Milioni=(myshort >>24) & 0xF;
 decM   =(myshort >>28) & 0xF;


 number_to_print[5] =cenmigl;
 number_to_print[4] =decmigl;
 number_to_print[3] =migl;
 number_to_print[2] =cent;
 number_to_print[1] =decine;
 number_to_print[0] =unit;

 if( decine)lungh=2;
 if(   cent)lungh=3;
 if(   migl)lungh=4;
 if(decmigl)lungh=5;
 if(cenmigl)lungh=6;
 if(Milioni)lungh=7;
 if(   decM)lungh=8;



 if(lungh>7)LCD_DrawChar(0x30 + decM);
 if(lungh>6)LCD_DrawChar(0x30 + Milioni);
 if(lungh>5)LCD_DrawChar(0x30 + cenmigl);
 if(lungh>4)LCD_DrawChar(0x30 + decmigl);
 if(lungh>3)LCD_DrawChar(0x30 + migl);
 if(lungh>2)LCD_DrawChar(0x30 + cent);
 if(lungh>1)LCD_DrawChar(0x30 + decine);
            LCD_DrawChar(0x30 + unit);*/
}

unsigned char BinToBCD_to_RTCC(unsigned int mybin)
{
	unsigned char i=8;
	unsigned char j=0x80;
	unsigned int myshort=0;
	
	if(mybin>0x3FF)mybin-=2000;

	while (i)
	{ 
	    /*
		Shift and Add-3 Algorithm

		1Shift the binary number left one bit.
		If 8 shifts have taken place, the BCD number is in the Hundreds, Tens, and Units column.
		If the binary value in any of the BCD columns is 5 or greater, add 3 to that value in that BCD column.
		Go to 1.
		   */
		 if((myshort & 0x000F)>0x0004)myshort+=0x0003;	
	     if((myshort & 0x00F0)>0x0040)myshort+=0x0030;
	     
	     
	     
	     myshort<<=1;
	     //shifta msb del binario in lsb del BCD
	     if(mybin & j)
	     {
	      myshort |=1;
	     }

	     i--;
	     j>>=1;
	}
	
	 return myshort;
}	 

/*------------------------------------
//ricava in qualche modo una stringa di float solo poitivi
void PrintFakeFloat(float* ptfloat)
{
 float myfloat;
 unsigned int myint;
 unsigned char i=32;
 unsigned int j=0x80000000;
 unsigned int myshort=0;
 unsigned char lungh=0;
 
 myfloat=*ptfloat;
 myint=(unsigned long)(myfloat*1000);



  while (i)
   {
     
	 Shift and Add-3 Algorithm

	 1Shift the binary number left one bit.
	 If 8 shifts have taken place, the BCD number is in the Hundreds, Tens, and Units column.
	 If the binary value in any of the BCD columns is 5 or greater, add 3 to that value in that BCD column.
	 Go to 1.
		
 	  if((myshort & 0x000F)>0x0004)myshort+=0x0003;
      if((myshort & 0x00F0)>0x0040)myshort+=0x0030;
      if((myshort & 0x0F00)>0x0400)myshort+=0x0300;
      if((myshort & 0xF000)>0x4000)myshort+=0x3000;
      if((myshort & 0x000F0000)>0x00040000)myshort+=0x00030000;
      if((myshort & 0x00F00000)>0x00400000)myshort+=0x00300000;
      if((myshort & 0x0F000000)>0x04000000)myshort+=0x03000000;
      if((myshort & 0xF0000000)>0x40000000)myshort+=0x30000000;


      myshort<<=1;
      //shifta msb del binario in lsb del BCD
      if(myint & j)
      {
       myshort |=1;
      }

      i--;
      j>>=1;
    }

  unit   =(myshort & 0x000F);
  decine =( myshort >>4) & 0xF;
  cent   =(myshort >>8)  & 0xF;
  migl   =(myshort >>12) & 0xF;
  decmigl=(myshort >>16) & 0xF;
  cenmigl=(myshort >>20) & 0xF;
  Milioni=(myshort >>24) & 0xF;
  decM   =(myshort >>28) & 0xF;

  if( decine)lungh=2;
  if(   cent)lungh=3;
  if(   migl)lungh=4;
  if(decmigl)lungh=5;
  if(cenmigl)lungh=6;
  if(Milioni)lungh=7;
  if(   decM)lungh=8;


 
 //lcd_write_data(0x30 + cenmigl);
 //lcd_write_data(0x30 + decmigl);
 lcd_write_data(0x30 + migl);
 lcd_write_data('.');
 lcd_write_data(0x30 + cent);
 lcd_write_data(0x30 + decine);
 lcd_write_data(0x30 + unit);
	
}

*/

/*
//------------------------------------
void DisplayText(char* stringtoprint)
{
  while(*stringtoprint)
  {
	  lcd_write_data(*stringtoprint);
	  stringtoprint++;
  }
	
}
*/





