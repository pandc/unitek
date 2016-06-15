/*
 * Submenu.c
 *
 *  Created on: 11/apr/2016
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
#include "Menu.h"
#include "keyboard.h"
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

unsigned char incr_step,incr_counter;

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

extern unsigned char MenuFunction_Index;


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

extern const char StringsSubmenuProg      	[4][5][22];
extern const char StringsSubmenuSelezProg 	[4][5][20];
extern const char StringsSubmenuLingua    	   [4][10];
extern const char StringsSubmenuImpostaSimboli	[4][5][20];
extern const char StringsSubmenuImpostaSoglie 	[4][8][20];
extern const char StringsSubmenuImpostaTimer  	[4][9][20];
extern const char StringsSubmenuSimboliConc   	    [5][4];

extern setp_e_soglie_type setp_e_soglie;
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;


//***************************************************************************************
void SubmenuINOUT(void)
{
	uint8_t key;
	LCD_Fill_ImageRAM(0x00);

	unsigned char loop_flag=1;
	unsigned short submenuINOUT_index,y_old;

	menu_triang_limit_up=26;
	menu_triang_limit_dn=38;
	abilita_disabilita=RamSettings.abilita_disabilita;

	if(abilita_disabilita)	submenuINOUT_index=0;
	else 					submenuINOUT_index=1;

	menu_triang_y=26+(submenuINOUT_index*12);

	y_old=menu_triang_y;



#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);

	LCDPrintString("IN - OUT",	44,2);

	LCDPrintString("ABILITA",	46,26);
	LCDPrintString("DISABILITA",38,38);

	LCD_CopyScreen();

	DisegnaMarker(92,menu_triang_y,menu_triang_y);

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);
		
		if (key == KEY_PROG)
		//if(CHECK_TASTO_PROG_PRESSED)
		{
			CLEAR_TASTO_PROG_PRESSED;
			MenuFunction_Index=MENU_PROGR;
			return;

		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			CLEAR_TASTO_OK_PRESSED;
			if(submenuINOUT_index==0)
			{
				abilita_disabilita=ABILITA;
			}//chiudi i relay ecc
			else
			{
				abilita_disabilita=DISABILITA ;
			}//apri i relay ecc

			DisegnaMarker(92,menu_triang_y,y_old);
			y_old=menu_triang_y;
			//RamSettings.abilita_disabilita=abilita_disabilita;
			//SaveImageInFlash();


		}

		if (key == KEY_DOWNRIGHT)
		//if(CHECK_TASTO_DN_DX_PRESSED)
		{
			MoveTriangolinoDown();
			if(submenuINOUT_index<1)submenuINOUT_index+=1;
			CLEAR_TASTO_DN_DX_PRESSED;

		}

		if (key == KEY_UPLEFT)
		//if(CHECK_TASTO_UP_SX_PRESSED)
		{
			MoveTriangolinoUp();
			if(submenuINOUT_index)submenuINOUT_index-=1;
			CLEAR_TASTO_UP_SX_PRESSED;
		}
	}
}
//***************************************************************************************
void SubmenuSelProgr(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=2,y_old;
	unsigned char loop_flag=1,test;
/*
	unsigned char x_prova=0,half_x=0;
	unsigned char y_prova=0,half_y=0;
*/
	unsigned short submenuSelProg_index;

	
	submenuSelProg_index=RamSettings.selected_program_id;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(submenuSelProg_index*12);

	LCD_Fill_ImageRAM(0x00);
#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);
	DisegnaMarker(72,menu_triang_y,menu_triang_y);
    y_old=menu_triang_y;
	SelectFont(CALIBRI_10);


	for(string_index=0;string_index<5;string_index++)
	{
		LCDPrintString("Progr",12,strings_y);
		BinToBCDisp(string_index+1  ,INTERO,40,strings_y);
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
			
			//program_arr=saved_prog_arr[selected_progr_index];
			DisegnaMarker(72,menu_triang_y,y_old);
			y_old=menu_triang_y;
			MenuFunction_Index=SUBMENU_SELECTED_PROGR;
			loop_flag=0;
			
                        RamSettings.selected_program_id=submenuSelProg_index;
                        test=SaveRamSettings_in_External_DataFlash();
                        if(!test)
                        {
                              LCD_Fill_ImageRAM(0x00);
                              SelectFont(CALIBRI_10);
                              LCDPrintString("File system error",4,24);
                              LCD_CopyPartialScreen(4,80,24,36);
                        }
			//SaveImageInFlash();
		}


		if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
		{
			if (key == KEY_DOWNRIGHT)
			//if(CHECK_TASTO_DN_DX_PRESSED)
			{
				MoveTriangolinoDown();
				if(submenuSelProg_index<5)submenuSelProg_index+=1;
				CLEAR_TASTO_DN_DX_PRESSED;
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(submenuSelProg_index)submenuSelProg_index-=1;
				CLEAR_TASTO_UP_SX_PRESSED;
			}
		}

		//if(CHECK_ARROW_KEYS_MOVE_SXDX)
		{



		}
	}
}
//***************************************************************************************
void SubmenuSelectedProgr(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=2;
	unsigned char loop_flag=1;
/*
	unsigned char x_prova=0,half_x=0;
	unsigned char y_prova=0,half_y=0;
*/
	static unsigned short submenuSelProg_index=0;


	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(submenuSelProg_index*12);

	LCD_Fill_ImageRAM(0x00);
#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);




	SelectFont(CALIBRI_10);


	for(string_index=0;string_index<5;string_index++)
	{
		LCDPrintString(StringsSubmenuSelezProg[RamSettings.Linguaggio][string_index],12,strings_y);
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
			MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
			loop_flag=0;

		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			CLEAR_TASTO_OK_PRESSED;
			switch(submenuSelProg_index)
			{
				case 0:
					MenuFunction_Index=SUB2MENU_IMPOSTA_SIMBOLI;
					break;
				case 1:
					MenuFunction_Index=SUB2MENU_TK;
					break;

				case 2:
					MenuFunction_Index=SUB2MENU_IMPOSTA_SOGLIE ;
					break;

				case 3:
					MenuFunction_Index=SUB2MENU_SEL_TIPO_CURV_LAV;
					break;

				case 4:
					MenuFunction_Index=SUB2MENU_IMPOSTA_TIMER;
					break;

				default:
					break;
			}


			loop_flag=0;
			CLEAR_TASTO_DN_DX_PRESSED;

		}


		if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
		{
			if (key == KEY_DOWNRIGHT)
			//if(CHECK_TASTO_DN_DX_PRESSED)
			{
				MoveTriangolinoDown();
				if(submenuSelProg_index<5)submenuSelProg_index+=1;
				CLEAR_TASTO_DN_DX_PRESSED;
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(submenuSelProg_index)submenuSelProg_index-=1;
				CLEAR_TASTO_UP_SX_PRESSED;
			}
		}

		//if(CHECK_ARROW_KEYS_MOVE_SXDX)
		{



		}
	}


}
//***************************************************************************************
void SubmenuComunic(void)
{
	uint8_t key;
	unsigned char loop_flag=1,to_print=1;
	static unsigned short submenuComunic_index=0;

	LCD_Fill_ImageRAM(0x00);

	stato_RS485=RamSettings.ComunRS485;
	stato_USB  =RamSettings.ComunUSB;


	menu_triang_y=26+(submenuComunic_index*12);



#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);
	LCDPrintString("Comunicazioni",	20,2);
	LCDPrintString("RS 485 PLC"	,	12,26);
	LCDPrintString("USB",			12,38);
	LCD_CopyScreen();


	menu_triang_limit_up=26;
	menu_triang_limit_dn=38;
	menu_triang_limit_dx=76;
	menu_triang_limit_sx=0;


	#ifdef DISEGNA_CORNICE
	DisegnaCornice();
	#endif

	//DisegnaTriangolinoMenu(0,menu_triang_y);



	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);
		//if(CHECK_KEY_READY)
		{
			CLEAR_KEY_READY;
			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuComunic_index<1)submenuComunic_index+=1;
					else				 submenuComunic_index=1;
					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuComunic_index)submenuComunic_index-=1;
					else 			   submenuComunic_index =0;
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

					//RamSettings.ComunRS485=stato_RS485;
					//RamSettings.ComunUSB=stato_USB;
					//SaveImageInFlash();
				}
			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if (key == KEY_PLUS)
				//if(CHECK_TASTO_PLUS_PRESSED)
					{

						if(menu_triang_x==76)
						{

							if(submenuComunic_index==0)
							{
								if((stato_RS485)<12)
								{
									stato_RS485++;
									to_print=1;
								}
							}

							else
							{
								if(stato_USB==0)stato_USB=1;

								to_print=1;
							}
						}
					}

					if(CHECK_TASTO_PLUS_RELEASED)
					{
						CLEAR_TASTO_PLUS_RELEASED;
					}



					if (key == KEY_MINUS)
					//if(CHECK_TASTO_MENO_PRESSED)
					{

						if(submenuComunic_index==0)
						{
							if((stato_RS485)>0)
							{
								stato_RS485--;
								to_print=1;
							}
						}

						else
						{
							if(stato_USB)stato_USB=0;
							to_print=1;
						}
					}

					if(CHECK_TASTO_MENO_RELEASED)
					{
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
				CleanArea_Ram_and_Screen(90,124,26,50);
				BinToBCDisp(stato_RS485,INTERO,90,26);
				BinToBCDisp(stato_USB  ,INTERO,90,38);
				LCD_CopyPartialScreen(90,124,26,50);

				to_print=0;
			}

		}
	}
}
//***************************************************************************************
void SubmenuSetClock(void)
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
		LCDPrintString("Orologio..da fare..",4,26);



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
		}

}
//***************************************************************************************
void SumMenuSelLingua(void)
{
	uint8_t key;

	unsigned short string_index=0,strings_y=2,y_old;
        unsigned char loop_flag=1,test;
/*
        unsigned char x_prova=0,half_x=0;
        unsigned char y_prova=0,half_y=0;
*/
        unsigned short submenuSelLing_index;



        submenuSelLing_index=RamSettings.Linguaggio;

        menu_triang_limit_up=2;
        menu_triang_limit_dn=38;

        menu_triang_y=2+(submenuSelLing_index*12);

		LCD_Fill_ImageRAM(0x00);
	#ifdef DISEGNA_CORNICE
		DisegnaCornice();
	#endif
		DisegnaTriangolinoMenu(0,menu_triang_y);
		DisegnaMarker(72,menu_triang_y,menu_triang_y);
                y_old=menu_triang_y;
		SelectFont(CALIBRI_10);


		for(string_index=0;string_index<4;string_index++)
		{
                    LCDPrintString(StringsSubmenuLingua[string_index],12,strings_y);
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
				RamSettings.Linguaggio=submenuSelLing_index;
                                test=SaveRamSettings_in_External_DataFlash();
                                if(!test)
                                {
                                      LCD_Fill_ImageRAM(0x00);
                                      SelectFont(CALIBRI_10);
                                      LCDPrintString("File system error",4,24);
                                      LCD_CopyPartialScreen(4,80,24,36);
                                }
                                
				DisegnaMarker(72,menu_triang_y,y_old);
				y_old=menu_triang_y;

				loop_flag=0;
				//RamSettings.Linguaggio=linguaggio;
				//SaveImageInFlash();
			}


			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuSelLing_index<3)submenuSelLing_index+=1;
					CLEAR_TASTO_DN_DX_PRESSED;
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuSelLing_index)submenuSelLing_index-=1;
					CLEAR_TASTO_UP_SX_PRESSED;
				}
			}

			//if(CHECK_ARROW_KEYS_MOVE_SXDX)
			{



			}
		}
}
//***************************************************************************************
void SubmenuServizio(void)
{

#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);

	//MenuFunction_Index=1;
	vTaskSuspend(NULL);
}
//***************************************************************************************
void PrintSoglia(unsigned short index,unsigned short x ,unsigned short y)//viene chiamata riga per riga quindi stampa solo 1 valore
{
	unsigned int   multiplied;
        unsigned int selected_progr=RamSettings.selected_program_id;
	SelectFont(CALIBRI_10);
	if(index<4)//solo concentrazioni
	{
		switch(RamSettings.ptype_arr[RamSettings.selected_program_id].unita_mis_concentr)
		{
		  case UNIT_MIS_CONCENTR_PERCENTUALE:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_Percent(RamSettings.ptype_arr[selected_progr].setp_e_soglie.setp_e_soglie_arr[index]);
			  CleanArea_Ram_and_Screen(x,x+30,y,y+10);
			  BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
			  break;

		  case UNIT_MIS_CONCENTR_PUNT_TITOL:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_PuntTitol(RamSettings.ptype_arr[selected_progr].setp_e_soglie.setp_e_soglie_arr[index]);
			  CleanArea_Ram_and_Screen(x,x+30,y,y+10);
			  BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
			  break;

		  case UNIT_MIS_CONCENTR_GRAMMILITRO:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_grammiLitro(RamSettings.ptype_arr[selected_progr].setp_e_soglie.setp_e_soglie_arr[index]);
			  CleanArea_Ram_and_Screen(x,x+30,y,y+10);
			  BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
			  break;

		  case UNIT_MIS_CONCENTR_uSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_uSiemens(RamSettings.ptype_arr[selected_progr].setp_e_soglie.setp_e_soglie_arr[index]);
			  CleanArea_Ram_and_Screen(x,x+30,y,y+10);
			  BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
			  break;

		  case UNIT_MIS_CONCENTR_mSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_milliSiemens(RamSettings.ptype_arr[selected_progr].setp_e_soglie.setp_e_soglie_arr[index]);
			  CleanArea_Ram_and_Screen(x,x+30,y,y+10);
			  BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
			  break;

		  default:
			  break;
		}
                LCD_CopyPartialScreen(x,x+30,y,y+10);
	}
	else//solo temperature
	{
		multiplied=RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]*3; ;
		multiplied/=128;

		CleanArea_Ram_and_Screen(x,x+30,y,y+10);
		BinToBCDisp(multiplied,UN_DECIMALE,x,y);
	}
}
//***************************************************************************************
void IncrSoglia(unsigned short index,unsigned short incr)//viene chiamata riga per riga quindi stampa solo 1 valore
{
	//unsigned short decimillesimi/*,num_to_print*/;
	unsigned int   multiplied;
	unsigned short resto;
	unsigned int temp_test;

	//controllo che il valore minimo non sia superiore al massimo
	if(index==SOGLIE_ALL_CONC_MIN_INDEX)
	{
		if(!(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MIN_INDEX] <
                      RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MAX_INDEX] ))
                {
			return;
		}
	}

	if(index==SOGLIE_ALL_TEMP_MIN_INDEX)
	{
		if(!(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MIN_INDEX] <
                      RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MAX_INDEX] ))
		{
			return;
		}
	}

	SelectFont(CALIBRI_10);
	if(index<4)//solo per le concentrazioni
	{       //potrei usare puntatore a funzione ,ma per comodità di debug uso lo switch
		switch(RamSettings.ptype_arr[RamSettings.selected_program_id]. unita_mis_concentr)
		{
		  case UNIT_MIS_CONCENTR_PERCENTUALE:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_Percent(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print+=incr;
                          temp_test=FormulaInversa_Conc_Percent();
			 //prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_PUNT_TITOL:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
                          Formula_ConcConvers_PuntTitol(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print+=incr;
                          temp_test=FormulaInversa_Conc_PuntTitol();
 			  break;

		  case UNIT_MIS_CONCENTR_GRAMMILITRO:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_grammiLitro(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_grammiLitro();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_uSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_uSiemens(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_mSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_milliSiemens(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  default:
			  break;
		}
          if(!(temp_test>conc_soglie_limit_up.setp_e_soglie_arr[index]))
          {
                  RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]= temp_test;
          }
          else
          {}      
                
	}
	else//temperature
	{
		multiplied=RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]*3;
		multiplied/=128;
		resto=multiplied % 128;

		if(multiplied<TEMP_MAX_LIMIT)multiplied++;

		multiplied*=128;
		multiplied+=resto;
		RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]=multiplied /3;

	}
}
//***************************************************************************************
void DecrSoglia(unsigned short index,unsigned short incr)//viene chiamata riga per riga quindi stampa solo 1 valore
{

	unsigned int   multiplied;
	unsigned short resto;
        unsigned int temp_test;



	//controllo che il valore minimo non sia superiore al massimo
	if(index==SOGLIE_ALL_CONC_MAX_INDEX)
	{
		if(!(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MAX_INDEX]> RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MIN_INDEX]))
		{
			return;
		}
	}

	if(index==SOGLIE_ALL_TEMP_MIN_INDEX)
	{
		if(!(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MAX_INDEX]> RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MIN_INDEX]))
		{
			return;
		}
	}


	SelectFont(CALIBRI_10);
	if(index<4)//solo per le concentrazioni
	{
         
          switch(RamSettings.ptype_arr[RamSettings.selected_program_id].unita_mis_concentr)
		{
		  case UNIT_MIS_CONCENTR_PERCENTUALE:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_Percent(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print-=incr;
                          temp_test=FormulaInversa_Conc_Percent();

			  break;

		  case UNIT_MIS_CONCENTR_PUNT_TITOL:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_PuntTitol(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print-=incr;
                          temp_test=FormulaInversa_Conc_PuntTitol();

			  break;

		  case UNIT_MIS_CONCENTR_GRAMMILITRO:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_grammiLitro(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_grammiLitro();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  
			  break;

		  case UNIT_MIS_CONCENTR_uSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_uSiemens(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print-=incr;
			  temp_test=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_mSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_milliSiemens(RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print-=incr;
			  temp_test=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  default:
			  break;
		}
                
              if(!(temp_test>conc_soglie_limit_up.setp_e_soglie_arr[index]))
              {
                      RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]= temp_test;
              }
              else
              {

              }

	}
	else
	{
		multiplied=RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]*3;
		multiplied/=128;
		resto=multiplied % 128;

		if(multiplied>TEMP_MIN_LIMIT)multiplied--;

		multiplied*=128;
		multiplied+=resto;
		RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]=multiplied /3;
	}
}
//***************************************************************************************
void PrintUnitMis(unsigned short index,unsigned short x ,unsigned short y)//viene chiamata riga per riga quindi stampa solo 1 valore
{
	SelectFont(CALIBRI_10);
	if(index<4)
	{
		LCDPrintString(StringsSubmenuSimboliConc[RamSettings.ptype_arr[RamSettings.selected_program_id].unita_mis_concentr],x,y);
	}
	else
	{
		LCDPrintString("'C",x,y);
	}
}
//***************************************************************************************************************************************
void DecrParamConc(unsigned short* par_pt,unsigned short incr)//viene chiamata riga per riga quindi stampa solo 1 valore
{


  switch(RamSettings.ptype_arr[RamSettings.selected_program_id].unita_mis_concentr)
  {
    case UNIT_MIS_CONCENTR_PERCENTUALE:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_Percent(*par_pt);
            struct_conc_print.conc_to_print-=incr;
            *par_pt=FormulaInversa_Conc_Percent();

            break;

    case UNIT_MIS_CONCENTR_PUNT_TITOL:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_PuntTitol(*par_pt);
            struct_conc_print.conc_to_print-=incr;
            *par_pt=FormulaInversa_Conc_PuntTitol();

            break;

    case UNIT_MIS_CONCENTR_GRAMMILITRO:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_grammiLitro(*par_pt);
            struct_conc_print.conc_to_print+=incr;
            *par_pt=FormulaInversa_Conc_grammiLitro();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            
            break;

    case UNIT_MIS_CONCENTR_uSIEMENS:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_uSiemens(*par_pt);
            struct_conc_print.conc_to_print-=incr;
            *par_pt=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            break;

    case UNIT_MIS_CONCENTR_mSIEMENS:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_milliSiemens(*par_pt);
            struct_conc_print.conc_to_print-=incr;
            *par_pt=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            break;

    default:
            break;
  }
          
        
	
}