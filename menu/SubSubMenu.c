/*
 * SubSubMenu.c
 *
 *  Created on: 22/apr/2016
 *      Author: Administrator
 */

#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"
#include "keyboard.h"
#include "Menu.h"
#include "VariabiliGlobali_di_Lavoro.h"


/*
struct
{
	unsigned short SetConc;
	unsigned short AllConcMin;
	unsigned short AllConcMax;
	unsigned short IsteresiConc;
	unsigned short SetTemp;
	unsigned short AllTempMin;
	unsigned short AllTempMax;
	unsigned short IsteresiTemp;

}StructSoglie;
*/


extern unsigned char incr_step,incr_counter;

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

extern unsigned char menuchange;
extern unsigned char MenuFunction_Index;
extern unsigned char unita_mis_concentr;
extern unsigned int var_concentr_to_print;

extern unsigned int keyold_flags;

extern unsigned int menu_triang_x,menu_triang_y;
//extern unsigned char menu_triang_index;
extern unsigned char menu_triang_limit_up;
extern unsigned char menu_triang_limit_dn;
extern unsigned char menu_triang_limit_dx;
extern unsigned char menu_triang_limit_sx;
extern unsigned char menu_triang_index;

extern unsigned char array_line[128];//un array abbastanza grande da contenere la + lunga linea possibile
extern unsigned char char_size;

extern unsigned char screen_image[1024];
extern unsigned int global_flags;
extern unsigned int key_flags;

extern unsigned int triangolino_inversion_timer;
extern unsigned char loop_flag;
extern unsigned int blink_timer_on;

extern const char StringsSubmenuProg      		[5][20];
extern const char StringsSubmenuSelezProg 		[5][20];
extern const char StringsSubmenuLingua    		[4][10];
extern const char StringsSubmenuImpostaSimboli	[5][20];
extern const char StringsSubmenuImpostaSoglie 	[8][20];
extern const char StringsSubmenuImpostaTimer  	[9][20];
extern const char StringsSubmenuSimboliConc     [5][4];
extern const char StringsSubmenuCurvaLavoro  	[16][4];
extern const char StringsSubmenuTipoCurvLav     [2][20];

//***************************************************************************************
void  Sub2MenuSelTipoCurvaLavoro(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=2,y_old;
		unsigned char loop_flag=1;
	/*
		unsigned char x_prova=0,half_x=0;
		unsigned char y_prova=0,half_y=0;
	*/
		static unsigned short submenuCurLavType_index=0;
		submenuCurLavType_index=program_arr.curva_lav_cal_type;

		menu_triang_limit_up=26;
		menu_triang_limit_dn=38;
		menu_triang_y=2+(submenuCurLavType_index*12);

		LCD_Fill_ImageRAM(0x00);
	#ifdef DISEGNA_CORNICE
		DisegnaCornice();
	#endif
		DisegnaTriangolinoMenu(0,menu_triang_y);
		DisegnaMarker(102,menu_triang_y,menu_triang_y);
	    y_old=menu_triang_y;
		SelectFont(CALIBRI_10);


		for(string_index=0;string_index<2;string_index++)
		{
			LCDPrintString(StringsSubmenuTipoCurvLav[string_index],12,strings_y);
			strings_y+=12;
		}

		LCD_CopyScreen();

		while(loop_flag)
		{
			key_getstroke(&key,portMAX_DELAY);
			if (key == KEY_PROG)
			//if(CHECK_TASTO_PROG_PRESSED)
			{
				CLEAR_TASTO_PROG_PRESSED;
				MenuFunction_Index=MENU_PROGR;
				loop_flag=0;

			}

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				CLEAR_TASTO_OK_PRESSED;
				program_arr.curva_lav_cal_type=submenuCurLavType_index;

				DisegnaMarker(102,menu_triang_y,y_old);
				y_old=menu_triang_y;
				if(program_arr.curva_lav_cal_type==CALIBR_CENTR)
					MenuFunction_Index=SUB3MENU_CURVA_DI_LAVORO;
				else
					MenuFunction_Index=SUB3MENU_CURVA_DI_LAVORO3pt;

				loop_flag=0;
			}


			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuCurLavType_index<5)submenuCurLavType_index+=1;
					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuCurLavType_index)submenuCurLavType_index-=1;
					CLEAR_TASTO_UP_SX_PRESSED;
				}
			}

			//if(CHECK_ARROW_KEYS_MOVE_SXDX)
			{



			}
		}
}
//***************************************************************************************
void Sub2MenuCurvadiLavoro(void)
{
	uint8_t key,last_key;
	unsigned short string_index=0,strings_y=2,temp;
	unsigned short menu_CurvaLavoro_index=0;
	unsigned short page=0,page_old;
	unsigned char loop_flag=1;
	unsigned char first_string_to_print=0,last_string_to_print;
	unsigned char to_print=1,resto;
	unsigned char blink_enable=0,toggler = 0;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_limit_dx=74;
	menu_triang_y=2+(menu_CurvaLavoro_index*H_RIGA_CALIBRI10);
	menu_triang_index=menu_CurvaLavoro_index;
	incr_step=1;

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key, kDec*2))
			last_key = key;
		else
		{
			if (blink_enable)
				DisegnaCarattereBlink('R',40,menu_triang_y,&toggler);
			key = 0;
		}
		//if(CHECK_KEY_READY)
		{
			//CLEAR_KEY_READY;

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				CLEAR_TASTO_OK_PRESSED;


				if(menu_triang_x==0)
				{
					MoveTriangolinoDx();
					//non marcare MOVE_SXDX!!
					CLEAR_ARROW_KEYS_MOVE_UPDOWN;
					MARK_PIU_MENO_ENABLED;
				}
				else  //se quindi confermo la scelta
				{
					MoveTriangolinoSx();
					MARK_ARROW_KEYS_MOVE_UPDOWN;
					CLEAR_ARROW_KEYS_MOVE_SXDX;
					CLEAR_PIU_MENO_ENABLED;
					program_arr.curva_lav1_C_index=page*5+menu_triang_index;


					RicalcolaCurvaLavoro();


					to_print=1;
				}
			}

			if (key == KEY_PROG)
			//if(CHECK_TASTO_PROG_PRESSED)
			{
				CLEAR_TASTO_PROG_PRESSED;
				MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
				loop_flag=0;

			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
				//if(CHECK_TASTO_PLUS_PRESSED)
				{

					if(menu_triang_x==menu_triang_limit_dx)
					{
						incr_counter++;
						if(incr_counter>10) incr_step=10;
						if(incr_counter>20)
						{
							incr_step=100;
							incr_counter=21;
						}

						string_index=menu_triang_index+ page*5;

						if((program_arr.curva_lavoro[string_index]+incr_step)<10000)
							program_arr.curva_lavoro[string_index]+=incr_step;
						CleanArea_Ram_and_Screen(92,118,menu_triang_y,menu_triang_y+10);
						//temp=program_arr.curva_lavoro[string_index];
						BinToBCDisp(temp,DUE_DECIMALI,92,menu_triang_y);
						LCD_CopyPartialScreen(92,118,menu_triang_y,menu_triang_y+10);
					}
				}
				else if (key == (KEY_PLUS | KEY_RELEASED))
				//if(CHECK_TASTO_PLUS_RELEASED)
				{
					incr_step=1;
					incr_counter=0;
					CLEAR_TASTO_PLUS_RELEASED;
				}

				if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
				//if(CHECK_TASTO_MENO_PRESSED)
				{

					if(menu_triang_x==menu_triang_limit_dx)
					{
						incr_counter++;
						if(incr_counter>10)
						{
							incr_step=10;
						}
						if(incr_counter>20)
						{
							incr_step=100;
							incr_counter=21;
						}


						string_index=menu_triang_index+ page*5;

						//sistemare...per step>1 può scendere sotto 0
						if(program_arr.curva_lavoro[string_index]-incr_step)program_arr.curva_lavoro[string_index]-=incr_step;
						CleanArea_Ram_and_Screen(92,120,menu_triang_y,menu_triang_y+10);
						temp=program_arr.curva_lavoro[string_index];
						BinToBCDisp(temp,DUE_DECIMALI,92,menu_triang_y);
						LCD_CopyPartialScreen(92,120,menu_triang_y,menu_triang_y+10);
						//PrintUnitMis(string_index,112,strings_y);
					}
				}
				else if (key == (KEY_MINUS | KEY_RELEASED))
				//if(CHECK_TASTO_MENO_RELEASED)
				{
					incr_step=1;
					incr_counter=0;
					CLEAR_TASTO_MENO_RELEASED;
				}
			}

			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(menu_CurvaLavoro_index<15)menu_CurvaLavoro_index+=1;
					page=(menu_CurvaLavoro_index)/5;

					if(page!=page_old)
					{
						to_print=1;
						if(page==3)menu_triang_limit_dn=2;
						else 	  menu_triang_limit_dn=50;
						menu_triang_index=0;
						menu_triang_y=menu_triang_limit_up;
					}
					else              to_print=0;


					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(menu_CurvaLavoro_index)menu_CurvaLavoro_index-=1;

					page=(menu_CurvaLavoro_index)/5;

					if(page!=page_old)
					{
						to_print=1;
						if(page==3)menu_triang_limit_dn=2;
						else 	  menu_triang_limit_dn=50;
						menu_triang_index=4;
						menu_triang_y=menu_triang_limit_dn;

					}
					else              to_print=0;

					CLEAR_TASTO_UP_SX_PRESSED;
				}
			}
			else//cioè if(CHECK_ARROW_KEYS_MOVE_SXDX)
			{
				//if(CHECK_TASTO_DN_DX_PRESSED)CLEAR_TASTO_DN_DX_PRESSED;
				//if(CHECK_TASTO_UP_SX_PRESSED)CLEAR_TASTO_UP_SX_PRESSED;
			}
		}//fine if(CHECK_KEY_READY)



		if(to_print)
		{
			to_print=0;

			switch(page)
			{
				case 0:
					first_string_to_print=0;
					last_string_to_print =5;//sarebbe 4

					break;

				case 1:
					first_string_to_print=5;
					last_string_to_print =10;//sarebbe 9

					break;

				case 2:
					first_string_to_print=10;
					last_string_to_print =15;//sarebbe 14

					break;

				case 3:
					first_string_to_print=15;
					last_string_to_print= 16;//sarebbe 15

					break;

				default:

					break;
			}

			LCD_Fill_ImageRAM(0x00);

#ifdef DISEGNA_CORNICE
			DisegnaCornice();
#endif
			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;

			for(string_index=first_string_to_print;string_index < last_string_to_print;string_index++)
			{
				LCDPrintString(StringsSubmenuCurvaLavoro[string_index],10,strings_y);
				BinToBCDisp(program_arr.curva_lavoro[string_index],DUE_DECIMALI,92,strings_y);
				//PrintUnitMis(string_index,118,strings_y);
				LCDPrintString("%",118,strings_y);


				strings_y+=H_RIGA_CALIBRI10;
			}
			//uso la variabile page_old perchè tanto verrà aggiornata tra poco
			page_old=	program_arr.curva_lav1_C_index / 5;
			resto=		program_arr.curva_lav1_C_index % 5;//es scelgo 7,è nella pag 1 riga 2(0,1,2)
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("C",80,strings_y);
			}


			page_old=page;

			LCD_CopyScreen();

		}
	}
}//fine Sub2MenuCurvaDiLavoro();

//***************************************************************************************
void Sub2MenuCurvadiLavoro3Punti(void)
{
	uint8_t key,last_key;
	unsigned short string_index=0,strings_y=2,temp;
	unsigned short menu_CurvaLavoro_index=0;
	unsigned short page=0,page_old;
	unsigned char loop_flag=1;
	unsigned char first_string_to_print=0,last_string_to_print;
	unsigned char to_print=1,resto;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_limit_dx=74;
	menu_triang_y=2+(menu_CurvaLavoro_index*H_RIGA_CALIBRI10);
	menu_triang_index=menu_CurvaLavoro_index;
	incr_step=1;

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,kDec * 2))
			last_key = key;
		else
			key = 0;
		
		//if(CHECK_KEY_READY)
		{
			CLEAR_KEY_READY;

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				CLEAR_TASTO_OK_PRESSED;

				switch(menu_triang_y)
				{


				}







				if(menu_triang_x==0)
				{
					MoveTriangolinoDx();
					//non marcare MOVE_SXDX!!
					CLEAR_ARROW_KEYS_MOVE_UPDOWN;
					MARK_PIU_MENO_ENABLED;
				}
				else  //se quindi confermo la scelta
				{
					MoveTriangolinoSx();
					MARK_ARROW_KEYS_MOVE_UPDOWN;
					CLEAR_ARROW_KEYS_MOVE_SXDX;
					CLEAR_PIU_MENO_ENABLED;
					program_arr.curva_lav1_C_index=page*5+menu_triang_index;

					RicalcolaCurvaLavoro();

					to_print=1;
				}
			}

			if (key == KEY_PROG)
			//if(CHECK_TASTO_PROG_PRESSED)
			{
				CLEAR_TASTO_PROG_PRESSED;
				MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
				loop_flag=0;

			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
				//if(CHECK_TASTO_PLUS_PRESSED)
				{

					if(menu_triang_x==menu_triang_limit_dx)
					{
						incr_counter++;
						if(incr_counter>10) incr_step=10;
						if(incr_counter>20)
						{
							incr_step=100;
							incr_counter=21;
						}


						string_index=menu_triang_index+ page*5;

						if((program_arr.curva_lavoro[string_index]+incr_step)<10000)program_arr.curva_lavoro[string_index]+=incr_step;
						CleanArea_Ram_and_Screen(92,118,menu_triang_y,menu_triang_y+10);
						temp=program_arr.curva_lavoro[string_index];
						BinToBCDisp(temp,DUE_DECIMALI,92,menu_triang_y);
						LCD_CopyPartialScreen(92,118,menu_triang_y,menu_triang_y+10);
					}
				}
				else if (key == (KEY_PLUS | KEY_RELEASED))
				//if(CHECK_TASTO_PLUS_RELEASED)
				{
					incr_step=1;
					incr_counter=0;
					CLEAR_TASTO_PLUS_RELEASED;
				}

				if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
				//if(CHECK_TASTO_MENO_PRESSED)
				{

					if(menu_triang_x==menu_triang_limit_dx)
					{
						incr_counter++;
						if(incr_counter>10)
						{
							incr_step=10;
						}
						if(incr_counter>20)
						{
							incr_step=100;
							incr_counter=21;
						}


						string_index=menu_triang_index+ page*5;

						//sistemare...per step>1 può scendere sotto 0
						if(program_arr.curva_lavoro[string_index]-incr_step)program_arr.curva_lavoro[string_index]-=incr_step;
						CleanArea_Ram_and_Screen(92,120,menu_triang_y,menu_triang_y+10);
						temp=program_arr.curva_lavoro[string_index];
						BinToBCDisp(temp,DUE_DECIMALI,92,menu_triang_y);
						LCD_CopyPartialScreen(92,120,menu_triang_y,menu_triang_y+10);
						//PrintUnitMis(string_index,112,strings_y);
					}
				}
				else if (key == (KEY_MINUS | KEY_RELEASED))
				//if(CHECK_TASTO_MENO_RELEASED)
				{
					incr_step=1;
					incr_counter=0;
					CLEAR_TASTO_MENO_RELEASED;
				}

			}

			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(menu_CurvaLavoro_index<15)menu_CurvaLavoro_index+=1;
					page=(menu_CurvaLavoro_index)/5;

					if(page!=page_old)
					{
						to_print=1;
						if(page==3)menu_triang_limit_dn=2;
						else 	  menu_triang_limit_dn=50;
						menu_triang_index=0;
						menu_triang_y=menu_triang_limit_up;
					}
					else              to_print=0;


					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(menu_CurvaLavoro_index)menu_CurvaLavoro_index-=1;

					page=(menu_CurvaLavoro_index)/5;

					if(page!=page_old)
					{
						to_print=1;
						if(page==3)menu_triang_limit_dn=2;
						else 	  menu_triang_limit_dn=50;
						menu_triang_index=4;
						menu_triang_y=menu_triang_limit_dn;

					}
					else              to_print=0;

					CLEAR_TASTO_UP_SX_PRESSED;
				}
			}
			else//cioè if(CHECK_ARROW_KEYS_MOVE_SXDX)
			{
				//if(CHECK_TASTO_DN_DX_PRESSED)CLEAR_TASTO_DN_DX_PRESSED;
				//if(CHECK_TASTO_UP_SX_PRESSED)CLEAR_TASTO_UP_SX_PRESSED;
			}
		}//fine if(CHECK_KEY_READY)



		if(to_print)
		{
			to_print=0;

			switch(page)
			{
				case 0:
					first_string_to_print=0;
					last_string_to_print =5;//sarebbe 4

					break;

				case 1:
					first_string_to_print=5;
					last_string_to_print =10;//sarebbe 9

					break;

				case 2:
					first_string_to_print=10;
					last_string_to_print =15;//sarebbe 14

					break;

				case 3:
					first_string_to_print=15;
					last_string_to_print= 16;//sarebbe 15

					break;

				default:

					break;
			}

			LCD_Fill_ImageRAM(0x00);

#ifdef DISEGNA_CORNICE
			DisegnaCornice();
#endif
			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;

			for(string_index=first_string_to_print;string_index < last_string_to_print;string_index++)
			{
				LCDPrintString(StringsSubmenuCurvaLavoro[string_index],10,strings_y);
				BinToBCDisp(program_arr.curva_lavoro[string_index],DUE_DECIMALI,92,strings_y);
				//PrintUnitMis(string_index,118,strings_y);
				LCDPrintString("%",118,strings_y);


				strings_y+=H_RIGA_CALIBRI10;
			}
			//uso la variabile page_old perchè tanto verrà aggiornata tra poco
			page_old=	program_arr.curva_lav3_L_index / 5;
			resto=		program_arr.curva_lav3_L_index % 5;
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("L",80,strings_y);
			}

			page_old=	program_arr.curva_lav3_C_index / 5;
			resto=		program_arr.curva_lav3_C_index % 5;
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("C",80,strings_y);
			}

			page_old=	program_arr.curva_lav3_H_index / 5;
			resto=		program_arr.curva_lav3_H_index % 5;
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("H",80,strings_y);
			}


			page_old=page;

			LCD_CopyScreen();

		}
	}
}//fine Sub2MenuCurvaDiLavoro3Punti();
//***************************************************************************************
void Sub2MenuImpostaSoglie(void)
{
	uint8_t key,last_key;
	unsigned short string_index=0,strings_y=2;
	static unsigned short menu_ImpostaSoglie_index=0;
	static unsigned short scroll_old=0;
	unsigned char loop_flag=1;
	unsigned char first_string_to_print,last_string_to_print;
	unsigned char to_print=1;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_limit_dx=76;
	menu_triang_y=2+(menu_ImpostaSoglie_index*H_RIGA_CALIBRI10);
	menu_triang_index=menu_ImpostaSoglie_index;
	incr_step=1;

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,kDec*2))
			last_key = key;
		else
			key = 0;
		//if(CHECK_KEY_READY)
		{
			CLEAR_KEY_READY;

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				CLEAR_TASTO_OK_PRESSED;


				if(menu_triang_x==0)
				{
					MoveTriangolinoDx();
					//non marcare MOVE_SXDX!!
					CLEAR_ARROW_KEYS_MOVE_UPDOWN;
					MARK_PIU_MENO_ENABLED;
				}
				else
				{
					MoveTriangolinoSx();
					MARK_ARROW_KEYS_MOVE_UPDOWN;
					CLEAR_ARROW_KEYS_MOVE_SXDX;
					CLEAR_PIU_MENO_ENABLED;
				}
			}

			if (key == KEY_PROG)
			//if(CHECK_TASTO_PROG_PRESSED)
			{
				CLEAR_TASTO_PROG_PRESSED;
				MenuFunction_Index=SUBMENU_SELECTED_PROGR;
				loop_flag=0;

			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
				//if(CHECK_TASTO_PLUS_PRESSED)
				{

					if(menu_triang_x==menu_triang_limit_dx)
					{
						incr_counter++;
						if(incr_counter>10) incr_step=10;
						if(incr_counter>20)
						{
							incr_step=100;
							incr_counter=21;
						}


						string_index=menu_triang_index;
						IncrSoglia(string_index,incr_step);
						//LCDPrintString(StringsSubmenuImpostaSoglie[string_index],10,strings_y);
						CleanArea_Ram_and_Screen(82,110,menu_triang_y,menu_triang_y+10);
						PrintSoglia(string_index,82,menu_triang_y);
						LCD_CopyPartialScreen(82,110,menu_triang_y,menu_triang_y+10);
						//PrintUnitMis(string_index,112,strings_y);
					}
				}
				else if (key == (KEY_PLUS | KEY_RELEASED))
				//if(CHECK_TASTO_PLUS_RELEASED)
				{
					incr_step=1;
					incr_counter=0;
					CLEAR_TASTO_PLUS_RELEASED;
				}

				if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
				//if(CHECK_TASTO_MENO_PRESSED)
				{

					if(menu_triang_x==menu_triang_limit_dx)
					{
						incr_counter++;
						if(incr_counter>10)
						{
							incr_step=10;
						}
						if(incr_counter>20)
						{
							incr_step=100;
							incr_counter=21;
						}


						string_index=menu_triang_index;
						DecrSoglia(string_index,incr_step);
						//LCDPrintString(StringsSubmenuImpostaSoglie[string_index],10,strings_y);
						CleanArea_Ram_and_Screen(82,110,menu_triang_y,menu_triang_y+10);
						PrintSoglia(string_index,82,menu_triang_y);
						LCD_CopyPartialScreen(82,110,menu_triang_y,menu_triang_y+10);
						//PrintUnitMis(string_index,112,strings_y);
					}
				}
				else if (key == (KEY_MINUS | KEY_RELEASED))
				//if(CHECK_TASTO_MENO_RELEASED)
				{
					incr_step=1;
					incr_counter=0;
					CLEAR_TASTO_MENO_RELEASED;
				}

			}

			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(menu_ImpostaSoglie_index<7)menu_ImpostaSoglie_index+=1;
					if(menu_ImpostaSoglie_index>4)//lo scroll in basso serve solo nei casi che debba mostrare oltre la 5a stringa
					{
						if(scroll_old==menu_ImpostaSoglie_index)to_print=0;
						else                           to_print=1;

					}
					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(menu_ImpostaSoglie_index)menu_ImpostaSoglie_index-=1;
					if(menu_ImpostaSoglie_index<3)//lo scroll in alto può servire se sono prima della 4 stringa
					{
						if(scroll_old==menu_ImpostaSoglie_index)to_print=0;
						else                           to_print=1;
					}
					CLEAR_TASTO_UP_SX_PRESSED;
				}
			}
			else//cioè if(CHECK_ARROW_KEYS_MOVE_SXDX)
			{



			}
		}//fine if(CHECK_KEY_READY)



		if(to_print)
		{
			to_print=0;

			if(menu_ImpostaSoglie_index==0)
			{
				first_string_to_print=0;
			}

			if(menu_ImpostaSoglie_index==1)
			{
				first_string_to_print=1;

			}

			if(menu_ImpostaSoglie_index==2)
			{
				first_string_to_print=2;
			}

			if(menu_ImpostaSoglie_index==5)
			{
				first_string_to_print=1;
			}

			if(menu_ImpostaSoglie_index==6)
			{
				first_string_to_print=2;
			}

			if(menu_ImpostaSoglie_index==7)
			{
				first_string_to_print=3;
			}

			LCD_Fill_ImageRAM(0x00);

#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;
			last_string_to_print=first_string_to_print + 5;
			for(string_index=first_string_to_print;string_index<last_string_to_print;string_index++)
			{
				LCDPrintString(StringsSubmenuImpostaSoglie[string_index],10,strings_y);
				PrintSoglia(string_index,82,strings_y);
				PrintUnitMis(string_index,112,strings_y);


				strings_y+=H_RIGA_CALIBRI10;
			}


			LCD_CopyScreen();
			scroll_old=menu_ImpostaSoglie_index;
		}



	}

}
//***************************************************************************************
void Sub2MenuImpostaTimer(void)
{
	uint8_t key;
	unsigned char loop_flag=1;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=38;
	menu_triang_y=2;

	LCD_Fill_ImageRAM(0x00);

#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif


	//DisegnaTriangolinoMenu(0,menu_triang_y);




	SelectFont(CALIBRI_10);
	LCDPrintString("TIMER..da fare..",8,26);



	LCD_CopyScreen();

	key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,portMAX_DELAY) && (key == KEY_PROG))
		//if(CHECK_TASTO_PROG_PRESSED)
		{
			CLEAR_TASTO_PROG_PRESSED;
			MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
			loop_flag=0;

		}
	}




}
//***************************************************************************************
void Sub2MenuImpostaSimboli(void)
{
	uint8_t key;
	menuchange=MENU_STAY;
	unsigned short string_index=0,strings_y=2,y_old;


	unsigned short submenuImpostaSimboli_index;
	unsigned char loop_flag=1;

	submenuImpostaSimboli_index=program_arr.unita_mis_concentr;

	loop_flag=1;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(H_RIGA_CALIBRI10*submenuImpostaSimboli_index);
	y_old=menu_triang_y;


	LCD_Fill_ImageRAM(0x00);

	DisegnaMarker(80,menu_triang_y,y_old);
#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);


	SelectFont(CALIBRI_10);
	for(string_index=0;string_index<5;string_index++)
	{
		LCDPrintString(StringsSubmenuImpostaSimboli[string_index],H_RIGA_CALIBRI10,strings_y);
		strings_y+=H_RIGA_CALIBRI10;
	}

	LCD_CopyScreen();

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);

		/*if(CHECK_TASTO_OK_PRESSED)
		{
			CLEAR_TASTO_OK_PRESSED;
			MARK_ARROW_KEYS_MOVE_SXDX;

		}*/


		if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
		{
			if (key == KEY_DOWNRIGHT)
			//if(CHECK_TASTO_DN_DX_PRESSED)
			{
				MoveTriangolinoDown();
				if(submenuImpostaSimboli_index<4)submenuImpostaSimboli_index+=1;
				else							 submenuImpostaSimboli_index=5;
				CLEAR_TASTO_DN_DX_PRESSED;
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(submenuImpostaSimboli_index)submenuImpostaSimboli_index-=1;
				else 						   submenuImpostaSimboli_index =0;
				CLEAR_TASTO_UP_SX_PRESSED;
			}
		}
		else//cioè if(CHECK_ARROW_KEYS_MOVE_SXDX)
		{



		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			program_arr.unita_mis_concentr=submenuImpostaSimboli_index;
			program_arr.unita_mis_concentr=submenuImpostaSimboli_index;
			DisegnaMarker(80,menu_triang_y,y_old);//il 3° parametro è la y del triangolino da cancellare

			y_old=menu_triang_y;
			CLEAR_TASTO_OK_PRESSED;
		}

		if (key == KEY_PROG)
		//if(CHECK_TASTO_PROG_PRESSED)
		{
			CLEAR_TASTO_PROG_PRESSED;
			MenuFunction_Index=SUBMENU_SELECTED_PROGR;
			loop_flag=0;

		}
	}
}
//***************************************************************************************
void Sub2MenuTK(void)
{
	uint8_t key,last_key;
	unsigned char loop_flag=1,to_print=1;
	static unsigned short submenuTK_index=0;
	//unsigned int nuovo_TK=2345;

	LCD_Fill_ImageRAM(0x00);

	menu_triang_limit_up=14;
	menu_triang_limit_dn=26;
	menu_triang_limit_dx=76;
	menu_triang_limit_sx=0;

	menu_triang_y=menu_triang_limit_up+(H_RIGA_CALIBRI10*submenuTK_index);

	TK.tk2.nuovo_TK=TK.tk2.old_TK;

#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif

	DisegnaTriangolinoMenu(0,menu_triang_y);

	LCDPrintString("Modifica Valore TK",	20,2);

	LCDPrintString("Attuale",20,14);
	LCDPrintString("Nuovo",	 20,26);

	LCD_CopyScreen();

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,kDec*2))
			last_key = key;
		else
			key = 0;
		//if(CHECK_KEY_READY)
		{
			CLEAR_KEY_READY;
			//if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuTK_index<1)submenuTK_index+=1;
					else				 submenuTK_index=1;
					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuTK_index)submenuTK_index-=1;
					else 			   submenuTK_index =0;
					CLEAR_TASTO_UP_SX_PRESSED;
				}
			}
			//else//cioè if(CHECK_ARROW_KEYS_MOVE_SXDX)
			{



			}

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				CLEAR_TASTO_OK_PRESSED;
                if(menu_triang_y==menu_triang_limit_dn)
                {

					if(menu_triang_x==0)
					{
						MoveTriangolinoDx();
						//non marcare MOVE_SXDX!!
						CLEAR_ARROW_KEYS_MOVE_UPDOWN;
						MARK_PIU_MENO_ENABLED;
					}
					else
					{
						MoveTriangolinoSx();
						MARK_ARROW_KEYS_MOVE_UPDOWN;
						CLEAR_ARROW_KEYS_MOVE_SXDX;
						CLEAR_PIU_MENO_ENABLED;

						TK.tk2.old_TK=TK.tk2.nuovo_TK;


					}
                }
			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
				//if(CHECK_TASTO_PLUS_PRESSED)
					{

						if(menu_triang_x==76)
						{
							incr_counter++;
							if(incr_counter>10) incr_step=10;
							if(incr_counter>20)
							{
								incr_step=100;
								incr_counter=21;
							}
							if((TK.TK_array[submenuTK_index]+incr_step)<9999)
							{
								TK.TK_array[submenuTK_index]+=incr_step;
								to_print=1;
							}


						}
					}
					else if (key == (KEY_PLUS | KEY_RELEASED))
					//if(CHECK_TASTO_PLUS_RELEASED)
					{
						incr_step=1;
						incr_counter=0;
						CLEAR_TASTO_PLUS_RELEASED;
					}



					if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
					//if(CHECK_TASTO_MENO_PRESSED)
					{

						if(menu_triang_x==76)
						{
							incr_counter++;
							if(incr_counter>10)
							{
								incr_step=10;
							}
							if(incr_counter>20)
							{
								incr_step=100;
								incr_counter=21;
							}


							if(!((TK.TK_array[submenuTK_index]-incr_step)<0))
							{
								TK.TK_array[submenuTK_index]-=incr_step;
								to_print=1;
							}
						}
					}
					else if (key == (KEY_MINUS | KEY_RELEASED))
					//if(CHECK_TASTO_MENO_RELEASED)
					{
						incr_step=1;
						incr_counter=0;
						CLEAR_TASTO_MENO_RELEASED;
					}

				}

			if (key == KEY_PROG)
			//if(CHECK_TASTO_PROG_PRESSED)
			{
				CLEAR_TASTO_PROG_PRESSED;
				MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
				loop_flag=0;
			}


			if(to_print)
			{
				CleanArea_Ram_and_Screen(90,124,14,40);
				BinToBCDisp(TK.tk2.old_TK,DUE_DECIMALI,90,14);
				BinToBCDisp(TK.tk2.nuovo_TK  ,DUE_DECIMALI,90,26);
				LCD_CopyPartialScreen(90,124,14,40);

				to_print=0;
			}

		}
	}
}
//***************************************************************************************
void DisegnaMarker(unsigned short x,unsigned short y,unsigned short y_old)
{
	mybmp_struct1.start_x=x;
	mybmp_struct1.start_y=y;
	CleanArea_Ram_and_Screen(x,x+10,y_old,y_old+10);
	CleanArea_Ram_and_Screen(x,x+10,y,y+10);
	LCD_DrawChar('*');
	LCD_CopyPartialScreen(x,x+10,y,y+10);
}
//***************************************************************************************
void DisegnaCarattereBlink(char char_to_blink,unsigned short x,unsigned short y,unsigned char *toggler)
{
	//if(!blink_timer_on)
	{
		//blink_timer_on=50;
		if(*toggler)
		{
			*toggler=0;
			CleanArea_Ram_and_Screen(x,x+10,y,y+10);
		}
		else
		{
			*toggler=1;
			mybmp_struct1.start_x=x;
			mybmp_struct1.start_y=y;
			LCD_DrawChar(char_to_blink);
			LCD_CopyPartialScreen(x,x+10,y,y+10);
		}
	}

}
//***************************************************************************************
//poi sarà fatta per ognuna delle n curve selezionabili
void RicalcolaCurvaLavoro(void)
{
	unsigned char i;
	unsigned short step;

	step=program_arr.curva_lavoro[program_arr.curva_lav1_C_index]/program_arr.curva_lav1_C_index;//curva_lavoro[0][selected_curva_lavoro_index]/selected_curva_lavoro_index;
	//per i valori inferiori al punto centrale
	for(i=0;i<program_arr.curva_lav1_C_index;i++)
	{
		program_arr.curva_lavoro[i]=i*step;
	}

	//per i valori superiori al punto centrale
	for(i=program_arr.curva_lav1_C_index+1;i<16;i++)
	{
		program_arr.curva_lavoro[i]=i*step;
	}


}
//***************************************************************************************
