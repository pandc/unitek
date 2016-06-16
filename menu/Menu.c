/*
 * Menu.c
 *
 *  Created on: 19/mar/2016
 *      Author: Administrator
 */
#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"
#include "ffile.h"
#include "meas.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"

#include "VariabiliGlobali_di_Lavoro.h"
#include "keyboard.h"
#include "menu.h"
#include "my_types.h"

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

unsigned char MenuFunction_Index;

print_conc_var struct_conc_print;



unsigned int menu_triang_x,menu_triang_y;
//unsigned char menu_triang_index=0;
unsigned char menu_triang_limit_up;
unsigned char menu_triang_limit_dn;
unsigned char menu_triang_limit_dx;
unsigned char menu_triang_limit_sx;
unsigned char menu_triang_index;

unsigned short ADC_array[4];


extern unsigned char screen_image[1024];

extern unsigned int blink_timer_on;
extern unsigned int blink_timer_off;
extern unsigned int triangolino_inversion_timer;
extern unsigned char loop_flag;

extern const char StringsMenuProg      [4][6][20];
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;
//dovrebbero essere costanti in flash,ma non riesco a inizializzarle
//***************************************************************************************
void (*MenuFunctionPt[30])(void);
void (*WorkMenu_CalcPrint_UnMisura_Conc[8])(unsigned int);
void (*CalcPrint_UnMisura_Conc[8])(unsigned int,unsigned int,unsigned int);
void (*IncrPrint_UnMisura_Conc[8])(unsigned short*,unsigned int,unsigned int,unsigned int);
void (*Formula_ConcConvers    [8])(unsigned int);
unsigned int(* FormulaInversa_Conc [8])(void);
//***************************************************************************************
static void menu_task(void *par)
{
        LCD_Display_Setup();
	FontPointerInit(); 
        LoadDisplay_Logo();
        ffready(portMAX_DELAY);
        LoadRamSettingsFrom_External_DataFlash();


	MenuFunction_Index=MENU_TEMPHUM;

	for (;;)
	{
		MARK_ARROW_KEYS_MOVE_UPDOWN;
		CLEAR_ARROW_KEYS_MOVE_SXDX;
		menu_triang_x=0;
		MenuFunctionPt[MenuFunction_Index]();
	}
}

//***************************************************************************************
void MenuInit(void)
{  
        //associo i puntatori a funzione menu a specifiche funzioni
	MenuFunctionPt[MENU_TEMPHUM]		=&MenuTempHum;	    //0
	MenuFunctionPt[MENU_PROGR]		=&MenuProg;			//1
	MenuFunctionPt[SUBMENU_INOUT]		=&SubmenuINOUT;		//2
	MenuFunctionPt[SUBMENU_SELEZIONA_PROG]	=&SubmenuSelProgr;	//3
	MenuFunctionPt[SUBMENU_COMUNIC]		=&SubmenuComunic;	//4
	MenuFunctionPt[SUBMENU_SETCLOCK]	=&SubmenuSetClock;  //5
	MenuFunctionPt[SUBMENU_SEL_LINGUA]	=&SumMenuSelLingua;	//6
	MenuFunctionPt[SUBMENU_SERVIZIO]	=&SubmenuServizio;	//7
	MenuFunctionPt[SUBMENU_SELECTED_PROGR]	=&SubmenuSelectedProgr;	//8


	MenuFunctionPt[SUB2MENU_IMPOSTA_SIMBOLI]	=&Sub2MenuImpostaSimboli;
	MenuFunctionPt[SUB2MENU_TK]			=&Sub2MenuTK;
	MenuFunctionPt[SUB3MENU_CURVA_DI_LAVORO]	=&Sub2MenuCurvadiLavoro;
	MenuFunctionPt[SUB2MENU_IMPOSTA_SOGLIE] 	=&Sub2MenuImpostaSoglie;
	MenuFunctionPt[SUB2MENU_IMPOSTA_TIMER]  	=&Sub2MenuImpostaTimer;
	MenuFunctionPt[SUB2MENU_SEL_TIPO_CURV_LAV]      =&Sub2MenuSelTipoCurvaLavoro;
	MenuFunctionPt[SUB3MENU_CURVA_DI_LAVORO3pt]	=&Sub2MenuCurvadiLavoro3Punti;
        MenuFunctionPt[SUB3MENU_SEL_LCH]		=&Sub2Sel_L_C_H;

        
        //associo i puntatori a funzione calcolo e stampa concentrazione a specifiche funzioni che tengono conto unit� di misura
        //vale solo per schermata iniziale con fonts=20
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PERCENTUALE ]=WorkMenu_CalcPrint_Percent;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PUNT_TITOL  ]=WorkMenu_CalcPrint_PuntTitol;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_GRAMMILITRO ]=WorkMenu_CalcPrint_GrammiLitro;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_uSIEMENS    ]=WorkMenu_CalcPrint_uSiemens;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_mSIEMENS    ]=WorkMenu_CalcPrint_milliSiemens;
       
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PERCENTUALE ]=CalcPrint_Percent_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PUNT_TITOL  ]=CalcPrint_PuntTitol_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_GRAMMILITRO ]=CalcPrint_GrammiLitro_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_uSIEMENS    ]=CalcPrint_uSiemens_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_mSIEMENS    ]=CalcPrint_milliSiemens_xy;
       
       IncrPrint_UnMisura_Conc  [UNIT_MIS_CONCENTR_PERCENTUALE ]=IncrPrintConc_Percent_xy;
       IncrPrint_UnMisura_Conc  [UNIT_MIS_CONCENTR_PUNT_TITOL  ]=IncrPrintConc_PuntTitol_xy;
       IncrPrint_UnMisura_Conc  [UNIT_MIS_CONCENTR_GRAMMILITRO ]=IncrPrintConc_GrammiLitro_xy;
       IncrPrint_UnMisura_Conc  [UNIT_MIS_CONCENTR_uSIEMENS    ]=IncrPrintConc_uSiemens_xy;
       IncrPrint_UnMisura_Conc  [UNIT_MIS_CONCENTR_mSIEMENS    ]=IncrPrintConc_milliSiemens_xy;
     
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_PERCENTUALE ]=Formula_ConcConvers_Percent;
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_PUNT_TITOL  ]=Formula_ConcConvers_PuntTitol;
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_GRAMMILITRO ]=Formula_ConcConvers_grammiLitro; 
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_uSIEMENS    ]=Formula_ConcConvers_uSiemens;
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_mSIEMENS    ]=Formula_ConcConvers_milliSiemens;
       //limiti massimi e minimi di set points e soglie
       conc_soglie_limit_up.ses_struct.SetConc=64000;
       conc_soglie_limit_up.ses_struct.AllConcMin=64000;
       conc_soglie_limit_up.ses_struct.AllConcMax=64000;
       conc_soglie_limit_up.ses_struct.IsteresiConc=10000;
       conc_soglie_limit_up.ses_struct.SetTemp=30000;
       conc_soglie_limit_up.ses_struct.AllTempMin=30000;
       conc_soglie_limit_up.ses_struct.AllTempMax=30000;
       conc_soglie_limit_up.ses_struct.IsteresiTemp=2000;


       conc_soglie_limit_dn.ses_struct.SetConc=0;
       conc_soglie_limit_dn.ses_struct.AllConcMin=0;
       conc_soglie_limit_dn.ses_struct.AllConcMax=0;
       conc_soglie_limit_dn.ses_struct.IsteresiConc=100;
       conc_soglie_limit_dn.ses_struct.SetTemp=200;
       conc_soglie_limit_dn.ses_struct.AllTempMin=200;
       conc_soglie_limit_dn.ses_struct.AllTempMax=200;
       conc_soglie_limit_dn.ses_struct.IsteresiTemp=100;
        
        
        
	xTaskCreate(menu_task, "menu", 256, NULL, tskIDLE_PRIORITY, NULL);
}

//***************************************************************************************
void MenuTempHum(void)
{
	uint8_t key;
	
	//unsigned char unit_misura_old;
	//unsigned int decimillesimi,conc_to_print;
	unsigned int temperature_to_print;
	unsigned int  /* multiplied,conc_to_print_old,*/temperature_to_print_old;
	unsigned int abilita_disabilita_old;

       // float myfloat_temper,my_float_conc;


	//conc_to_print_old=0xFFFF;
	temperature_to_print_old=0xFFFF;
	abilita_disabilita_old=0x3;


	stato_intervento_conc=STATO_INIZIALE;
	stato_intervento_temper=STATO_INIZIALE;

	LCD_Fill_ImageRAM(0x00);

	//RigaVertic(63,0,63);
	LCD_CopyScreen();


#ifdef SHOW_BEUTA_VUOTA
        mybmp_struct2.bmp_pointer=beutaVuota_bmp;
        mybmp_struct2.righe		 =beutaVuotaHeightPixels;
        mybmp_struct2.colonne	 =beutaVuotaWidthPages;
        mybmp_struct2.start_x=30;
        mybmp_struct2.start_y=42;
        GetBitmap();
#endif

//#define SHOW_TERMOMETRO
#ifdef 	SHOW_TERMOMETRO
	//per disegnare una bitmap :dopo avere assegnato i riferimenti alla bitmap(prime 3 righe qui sotto),
	//scegliere le coordintate(possibilmente pari,per ora) e chiamare GetBitmap();   NB scrive solo in RAM,andr� poi copiata nella RAM del display
	mybmp_struct2.bmp_pointer=termometro_bmp;
	mybmp_struct2.righe		 =termometroHeightPixels;
	mybmp_struct2.colonne	 =termometroWidthPages;
	mybmp_struct2.start_x=66;
	mybmp_struct2.start_y=42;
	GetBitmap();


#endif

	//#define SHOW_RISCALDATORE
	#ifdef SHOW_RISCALDATORE
	mybmp_struct2.bmp_pointer=riscaldatore_bmp;
	mybmp_struct2.righe		 =riscaldatoreHeightPixels;
	mybmp_struct2.colonne	 =riscaldatoreWidthPages;
	mybmp_struct2.start_x=104;
	mybmp_struct2.start_y=42;
	GetBitmap();

	#endif
	mybmp_struct2.bmp_pointer=gradi_bmp;
	mybmp_struct2.righe		 =gradi_HeightPixels;
	mybmp_struct2.colonne	 =gradi_WidthPages;
	mybmp_struct2.start_x=94;
	mybmp_struct2.start_y=2;
	GetBitmap();
        
        

	SelectFont(CALIBRI_10);

	LCDPrintString("C",98,2);

	LCD_CopyScreen();

	//unit_misura_old=10;

	while(1)
	{
		if (key_getstroke(&key,100/*portMAX_DELAY*/) && (key == KEY_PROG))
		{
		  MenuFunction_Index=MENU_PROGR;
		  return;
		}

		if(abilita_disabilita!=abilita_disabilita_old)
		{
			SelectFont(CALIBRI_20);
			//abilita_disabilita=DISABILITA;
			if(abilita_disabilita==DISABILITA)
			{
				 CleanArea_Ram_and_Screen(6,124,34,56);
				 LCDPrintString("USCITE_OFF",6,38);
				 LCD_CopyPartialScreen(6,124,34,58);
			}
			else
			{
				 CleanArea_Ram_and_Screen(6,124,34,56);
				 //LCDPrintString("USCITE_OFF",6,34);
				 LCD_CopyPartialScreen(6,124,34,56);
			}

			abilita_disabilita_old=abilita_disabilita;

		}
#define SHOW_ADC
#if defined(SHOW_ADC)

		 if(1)// meas_temp(&myfloat_temper))
		 {
			 ADC_array[LETTURA_TEMP]=38951;//(unsigned short)myfloat_temper;

			 if(temperature_to_print != temperature_to_print_old)
			 {
				 SelectFont(CALIBRI_20);
				 CleanArea_Ram_and_Screen(68,120,14,36);
				 BinToBCDisp(ADC_array[LETTURA_TEMP]/*temperature_to_print*/,UN_DECIMALE,68,14);
				 LCD_CopyPartialScreen(68,120,14,36);
				 temperature_to_print_old=temperature_to_print;
			 }

		 }

                
                if(1)// meas(&my_float_conc))
                {  
                    
                  ADC_array[LETTURA_CONC]=43528;//(unsigned short)my_float_conc; 
                  
                 WorkMenu_CalcPrint_UnMisura_Conc[RamSettings.ptype_arr[RamSettings.selected_program_id].unita_mis_concentr](ADC_array[LETTURA_CONC]);

                 } 
#endif
/*
		if(stato_intervento_conc==STATO_INIZIALE)
		{
		   //per stampare  "SETP xxx" o bitmpa pompa quando entro,solo la prima volta fingo di avere lettura ADC abbondante
                   ADC_array[LETTURA_CONC] =1+ (RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetConc +
                                                    RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiConc);
		  
		   stato_intervento_conc=  STATO_POMPA_ON_CONC_SCARSA;
		}

		if(stato_intervento_temper==STATO_INIZIALE)
		{
			//se ne manca allora faccio saltare a stato riposo,come se da questo si accorgesse che ne manca
			ADC_array[LETTURA_TEMP]= (RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetTemp +
                                                    RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiTemp);
			stato_intervento_temper=STATO_RISC_ON_TEMP_SCARSA;
		}

*/
		//**************** INTERVENTO!!!********************************************
		//confronto con setpoint,isteresi ecc
		switch(stato_intervento_conc)
		{


			case STATO_POMPA_RIPOSO:
				//if((ADC_array[LETTURA_CONC]) < RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.SetConc - RamSettings.ptype_arr[RamSettings.selected_program_id]ses_struct.setp_e_soglie.ses_struct.IsteresiConc)
				if((ADC_array[LETTURA_CONC]) < (RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetConc - RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiConc))
				{

					stato_intervento_conc=STATO_POMPA_ON_CONC_SCARSA;
					CleanArea_Ram_and_Screen(2,28,42,54);
					mybmp_struct2.bmp_pointer=pompa_OK_bmp;
					mybmp_struct2.righe		 =pompa_OKHeightPixels;
					mybmp_struct2.colonne	 =pompa_OKWidthPages;
					mybmp_struct2.start_x=2;
					mybmp_struct2.start_y=42;
					GetBitmap();
					//LCD_CopyScreen();
					LCD_CopyPartialScreen(2,26,42,64);

				}
				break;


			case STATO_POMPA_ON_CONC_SCARSA:
				//if((ADC_array[LETTURA_CONC]) > (RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.SetConc + RamSettings.ptype_arr[RamSettings.selected_program_id]ses_struct.setp_e_soglie.ses_struct.IsteresiConc))
				if((ADC_array[LETTURA_CONC]) > (RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetConc + RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiConc))
				{
					stato_intervento_conc=STATO_POMPA_RIPOSO;
					//cancello disegno pompa
					CleanArea_Ram_and_Screen(2,26,42,64);
					SelectFont(CALIBRI_10);
					CleanArea_Ram_and_Screen(2,58,42,54);
					LCDPrintString("SET",2,42);

					PrintSoglia(0,30 ,42);
					LCD_CopyPartialScreen(2,58,42,54);

				}
				else
				{
					if((ADC_array[LETTURA_CONC]) < (RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.AllConcMin-+ RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiConc))
					{
						//prova=RamSettings.ptype_arr[RamSettings.selected_program_id]setp_e_soglie.ses_struct.AllConcMin;
						SelectFont(CALIBRI_10);
						LCDPrintString("MIN!",30,42);
						LCD_CopyPartialScreen(30,58,42,54);
					}
					else
					{
						if(abilita_disabilita==ABILITA)CleanArea_Ram_and_Screen(30,58,42,54);
					}

				}
				break;

			default:
				break;
		}


		switch(stato_intervento_temper)
		{

/*
		 		 //se va bene faccio saltare in stato CONC_SCARSA come se si accorgesse da l� che conc � ok
		 		if((ADC_array[LETTURA_TEMP]) < RamSettings.ptype_arr[RamSettings.selected_program_id]setp_e_soglie.ses_struct.SetTemp - RamSettings.ptype_arr[RamSettings.selected_program_id]setp_e_soglie.ses_struct.IsteresiTemp)
		 		{
		 			 stato_intervento_temper=  STATO_RISC_ON_TEMP_SCARSA;
		 			CleanArea_Ram_and_Screen(68,128,42,64);
					mybmp_struct2.bmp_pointer=riscaldatore_bmp;
					mybmp_struct2.righe		 =riscaldatoreHeightPixels;
					mybmp_struct2.colonne	 =riscaldatoreWidthPages;
					mybmp_struct2.start_x=104;
					mybmp_struct2.start_y=42;
					GetBitmap();
					//LCD_CopyScreen();
					LCD_CopyPartialScreen(68,128,42,64);

		 		 }
		 		 break;
*/

			case STATO_RISC_RIPOSO:
				if((ADC_array[LETTURA_TEMP]) < RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetTemp - RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiTemp)
					{
					stato_intervento_temper=STATO_RISC_ON_TEMP_SCARSA;
					CleanArea_Ram_and_Screen(68,128,42,64);
					mybmp_struct2.bmp_pointer=riscaldatore_bmp;
					mybmp_struct2.righe		 =riscaldatoreHeightPixels;
					mybmp_struct2.colonne	 =riscaldatoreWidthPages;
					mybmp_struct2.start_x=104;
					mybmp_struct2.start_y=42;
					GetBitmap();
					//LCD_CopyScreen();
					LCD_CopyPartialScreen(68,128,42,64);
				}


				break;


			case STATO_RISC_ON_TEMP_SCARSA:
                                if((ADC_array[LETTURA_TEMP]) > RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetTemp +
                                                                      RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.IsteresiTemp)
				
				{
					stato_intervento_temper=STATO_RISC_RIPOSO;
					//cancello disegno riscaldatore

					CleanArea_Ram_and_Screen(68,128,42,64);
					//CleanProva();
					SelectFont(CALIBRI_10);
					CleanArea_Ram_and_Screen(68,127,42,54);
					LCDPrintString("SET",68,42);

					temperature_to_print=RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.SetTemp*3;

					temperature_to_print/=128;

					BinToBCDisp(temperature_to_print,UN_DECIMALE,90,42);
					LCD_CopyPartialScreen(68,128,42,54);
				}
				else
				{
					if((ADC_array[LETTURA_TEMP]) < RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.ses_struct.AllTempMin)
					{
						//CleanArea_Ram_and_Screen(104,128,42,64);
/*
						prova=RamSettings.ptype_arr[RamSettings.selected_program_id]ses_struct.setp_e_soglie.ses_struct.AllTempMin;
						mybmp_struct2.bmp_pointer=termometro_bmp;
						mybmp_struct2.righe		 =termometroHeightPixels;
						mybmp_struct2.colonne	 =termometroWidthPages;
						mybmp_struct2.start_x=66;
						mybmp_struct2.start_y=42;
						GetBitmap();
						LCD_CopyPartialScreen(66,80,42,64);*/
					}
					else
					{
						CleanArea_Ram_and_Screen(66,80,42,64);
					}

				}
				break;





			default:
				break;
		}

	}
}
//***************************************************************************************
void MenuProg(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=2;
	unsigned short menu_prog_index=0;
	unsigned short scroll_old=0;	// scroll=0
	unsigned char loop_flag=1;
	unsigned char first_string_to_print,last_string_to_print;
	unsigned char to_print=1;



	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(menu_prog_index*12);


	//WriteMyFlashSector();

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);
		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			//CLEAR_TASTO_OK_PRESSED;
			//MARK_ARROW_KEYS_MOVE_SXDX;
			//CLEAR_ARROW_KEYS_MOVE_UPDOWN;
			switch(menu_prog_index)
			{
				case 0:
					MenuFunction_Index=SUBMENU_INOUT;
					break;
				case 1:
					MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
					break;

				case 2:
					MenuFunction_Index=SUBMENU_COMUNIC;
					break;

				case 3:
					MenuFunction_Index=SUBMENU_SETCLOCK;
					break;

				case 4:
					MenuFunction_Index=SUBMENU_SEL_LINGUA;
					break;

				case 5:
					MenuFunction_Index=SUBMENU_SERVIZIO;
					break;

				default:
					break;
			}


			loop_flag=0;

		}


		if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
		{
			if (key == KEY_DOWNRIGHT)
			//if(CHECK_TASTO_DN_DX_PRESSED)
			{
				MoveTriangolinoDown();
				if(menu_prog_index<5)menu_prog_index+=1;
				if(menu_prog_index==5)
				{
					if(scroll_old==menu_prog_index)to_print=0;
					else                           to_print=1;
				}
				CLEAR_TASTO_DN_DX_PRESSED;
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(menu_prog_index)menu_prog_index-=1;
				if(menu_prog_index==0)
				{
					if(scroll_old==menu_prog_index)to_print=0;
					else                           to_print=1;
				}
				CLEAR_TASTO_UP_SX_PRESSED;
			}
		}

		if (key == KEY_PROG)
		//if(CHECK_TASTO_PROG_PRESSED)
		{
			CLEAR_TASTO_PROG_PRESSED;
			MenuFunction_Index=MENU_TEMPHUM;
			loop_flag=0;

		}

		if(to_print)
		{
			to_print=0;

			if(menu_prog_index==0)
			{
				first_string_to_print=0;

			}
			if(menu_prog_index==5)
			{
				first_string_to_print=1;

			}
			LCD_Fill_ImageRAM(0x00);
#ifdef DISEGNA_CORNICE
			DisegnaCornice()
#endif
			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;
			last_string_to_print=first_string_to_print + 5;
			for(string_index=first_string_to_print;string_index<last_string_to_print;string_index++)
			{
				LCDPrintString(StringsMenuProg[RamSettings.Linguaggio][string_index],12,strings_y);
				strings_y+=12;
			}

			LCD_CopyScreen();
			scroll_old=menu_prog_index;
		}
	}
}

//***************************************************************************************
void Update_KeyOld(void)
{
	/*unsigned int temp;
	temp=global_flags & KEYS_MASK;
	keyold_flags=temp;*/
}
//***************************************************************************************
void MoveTriangolinoDown(void)
{

	unsigned int	old_menu_triang_y;

    if(menu_triang_y>menu_triang_limit_dn || menu_triang_y==menu_triang_limit_dn)
    {

    	//menu_triang_index=4;
    	return;
    }

	old_menu_triang_y=menu_triang_y;

	CleanArea_Ram_and_Screen(0,8,menu_triang_y,menu_triang_y+TRIANG_H);//cancello il triangolo...e un pezzettino di riga verticale

#ifdef DISEGNA_CORNICE
	RigaVertic(0,0,63);
#endif

	menu_triang_y+=12;
	menu_triang_index++;



	DisegnaTriangolinoMenu(0,menu_triang_y);


	LCD_CopyPartialScreen(0,8,old_menu_triang_y,menu_triang_y+20);

}
//***************************************************************************************
void MoveTriangolinoUp(void)
{

	unsigned int	old_menu_triang_y;

    if(menu_triang_y<menu_triang_limit_up  || menu_triang_y==menu_triang_limit_up)
    {
		//menu_triang_index=0;
		return;

    }

	old_menu_triang_y=menu_triang_y;

	CleanArea_Ram_and_Screen(0,8,menu_triang_y,menu_triang_y+TRIANG_H);//cancello il triangolo...e un pezzettino di riga verticale

#ifdef DISEGNA_CORNICE
	RigaVertic(0,0,63);
#endif

	menu_triang_y-=12;
	menu_triang_index--;

	DisegnaTriangolinoMenu(0,menu_triang_y);


	LCD_CopyPartialScreen(0,8,menu_triang_y,old_menu_triang_y+20);

}
//***************************************************************************************
void MoveTriangolinoDx(void)
{

//qui devo riscrivere sia la riga verticale SX che l'area del nuovo triangolino

    if(menu_triang_x>menu_triang_limit_dx  || menu_triang_x==menu_triang_limit_dx)
    {
		//menu_triang_index=0;
		return;
    }



	CleanArea_Ram_and_Screen(0,8,menu_triang_y,menu_triang_y+TRIANG_H);//cancello il triangolo...e un pezzettino di riga verticale

	menu_triang_x=menu_triang_limit_dx;
#ifdef DISEGNA_CORNICE
	RigaVertic(0,0,63);
#endif
	LCD_CopyPartialScreen(0,2,0,63);

	DisegnaTriangolinoMenu(menu_triang_x,menu_triang_y);

	LCD_CopyPartialScreen(menu_triang_x,84,menu_triang_y,menu_triang_y+20);

}
//***************************************************************************************
void MoveTriangolinoSx(void)
{

//qui devo riscrivere sia la riga verticale SX che l'area del nuovo triangolino

    if(menu_triang_x<menu_triang_limit_sx  || menu_triang_x==menu_triang_limit_sx)
    {
		//menu_triang_index=0;
		return;
    }



	CleanArea_Ram_and_Screen(menu_triang_limit_dx,menu_triang_limit_dx+TRIANG_W,
							 menu_triang_y,menu_triang_y+TRIANG_H);//cancello il triangolo...e un pezzettino di riga verticale

#ifdef DISEGNA_CORNICE
	RigaVertic(0,0,63);
#endif
	menu_triang_x=0;
	//menu_triang_index--;

	DisegnaTriangolinoMenu(0,menu_triang_y);


	LCD_CopyPartialScreen(0,8,menu_triang_y,menu_triang_y+20);

}
//***************************************************************************************
void DisegnaTriangolinoMenu(unsigned short triang_x,unsigned short triang_y)
{
	mybmp_struct2.bmp_pointer=triangolino_bmp;
	mybmp_struct2.righe		 =triangolino_HeightPixels;
	mybmp_struct2.colonne	 =triangolino_WidthPages;
	mybmp_struct2.start_x=triang_x;
	mybmp_struct2.start_y=triang_y;
	GetBitmap();
}
//***************************************************************************************
void DisegnaCornice (void)
{
	RigaHoriz(0,127,0);
	RigaHoriz(0,127,63);
	RigaVertic(0,0,63);
	RigaVertic(127,0,63);
}
//***************************************************************************************
void LoadRamSettingsFrom_uC_Flash(void)
{
	const unsigned char* cuipt=(const unsigned char *)&saved_prog_arr[0];//0xFF000;
	unsigned short i,size;
	unsigned char * sec_image_pt;

	sec_image_pt=(unsigned char *)&RamSettings;
	size=sizeof RamSettings;

	for(i=0;i<size;i++)
	{
		*sec_image_pt++=*cuipt++;
	}
}
//***************************************************************************************
void LoadRamSettingsFrom_External_DataFlash(void)
{
  FFILE *FileMyParameters;
  unsigned short size;

  size=sizeof RamSettings;
  
  FileMyParameters =ffopen("MyParameters.bin",'r');
  if (!FileMyParameters)
  {
    //decidere se oltre a copiare da const in uc Flash,o addirittura creare il file  mancante
    //memcpy(&RamSettings,&
    FileMyParameters = ffopen("MyParameters.bin",'w');
    if (FileMyParameters)
    {
      ffwrite(FileMyParameters,&RamSettings,size);
      ffclose(FileMyParameters);
    }
  }
  else
  {  
    ffread(FileMyParameters,&RamSettings,size);
    ffclose(FileMyParameters);
  } 
}
//***************************************************************************************
unsigned char SaveRamSettings_in_External_DataFlash(void)
{
  FFILE *FileMyParameters;
  unsigned short size;
  
  size=sizeof RamSettings;
  
  FileMyParameters=ffopen("MyParameters.bin",'w');
  if(FileMyParameters)
  {
    ffwrite(FileMyParameters,&RamSettings,size);
    ffclose(FileMyParameters);
    return 1;//tutto ok
  }
  else return 0;
}












/* _____      _           _                      _                                                          __            _          ___   ___  
  / ____|    | |         | |                    | |                                                        / _|          | |        |__ \ / _ \ 
 | |     __ _| | ___ ___ | | ___      ___    ___| |_ __ _ _ __ ___  _ __   __ _     ___ ___  _ __   ___   | |_ ___  _ __ | |_ ___      ) | | | |
 | |    / _` | |/ __/ _ \| |/ _ \    / _ \  / __| __/ _` | '_ ` _ \| '_ \ / _` |   / __/ _ \| '_ \ / __|  |  _/ _ \| '_ \| __/ __|    / /| | | |
 | |___| (_| | | (_| (_) | | (_) |  |  __/  \__ \ || (_| | | | | | | |_) | (_| |  | (_| (_) | | | | (__   | || (_) | | | | |_\__ \   / /_| |_| |
  \_____\__,_|_|\___\___/|_|\___/    \___|  |___/\__\__,_|_| |_| |_| .__/ \__,_|   \___\___/|_| |_|\___|  |_| \___/|_| |_|\__|___/  |____|\___/ 
                                                                   | |                                                                          
                                                                   |_|      */

//***************************************************************************************
void WorkMenu_CalcPrint_Percent(unsigned int bin)
{
     
      static unsigned int conc_to_print_old=0xFFFFFFF;
      
      Formula_ConcConvers_Percent(bin);
      
 
      if(struct_conc_print.conc_to_print!=conc_to_print_old) 
      {
              SelectFont(CALIBRI_10);
              CleanArea_Ram_and_Screen(20,40,UN_MIS_Y_START,14);
              LCDPrintString("%",27,UN_MIS_Y_START);
              LCD_CopyPartialScreen(20,40,UN_MIS_Y_START,14);


              SelectFont(CALIBRI_20);
              CleanArea_Ram_and_Screen(00,62,14,36);
             
              BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,4,14);
             
              LCD_CopyPartialScreen(00,62,14,36);

              conc_to_print_old=struct_conc_print.conc_to_print;
      }
}
//***************************************************************************************
void WorkMenu_CalcPrint_PuntTitol(unsigned int bin)
{
      static unsigned int conc_to_print_old=0xFFFFFFF;
      //AD1_GetValue16(ADC_array);
      //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
      Formula_ConcConvers_PuntTitol(bin);
       
        if(struct_conc_print.conc_to_print!=conc_to_print_old)
        {
                SelectFont(CALIBRI_10);
                CleanArea_Ram_and_Screen(20,40,UN_MIS_Y_START,14);
                LCDPrintString("P",27,UN_MIS_Y_START);
                LCD_CopyPartialScreen(20,40,UN_MIS_Y_START,14);

                
                SelectFont(CALIBRI_20);
                CleanArea_Ram_and_Screen(00,62,14,36);
                
                BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,4,14);

                LCD_CopyPartialScreen(00,62,14,36);

                conc_to_print_old=struct_conc_print.conc_to_print;

        }
}
//***************************************************************************************
void WorkMenu_CalcPrint_GrammiLitro(unsigned int bin)
{
      static unsigned int conc_to_print_old=0xFFFFFFF;
      
      //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
      Formula_ConcConvers_grammiLitro(bin);
     

      if(struct_conc_print.conc_to_print!=conc_to_print_old)
      {
              SelectFont(CALIBRI_10);
              CleanArea_Ram_and_Screen(20,40,UN_MIS_Y_START,14);
              LCDPrintString("g/l",24,UN_MIS_Y_START);
              LCD_CopyPartialScreen(20,40,UN_MIS_Y_START,14);


              SelectFont(CALIBRI_20);
              CleanArea_Ram_and_Screen(00,62,14,36);
              
              BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,4,14);
              

              LCD_CopyPartialScreen(00,62,14,36);

              conc_to_print_old=struct_conc_print.conc_to_print;

      }
}

//***************************************************************************************
void WorkMenu_CalcPrint_uSiemens(unsigned int bin)
{
   static unsigned int conc_to_print_old=0xFFFFFFF;
    //AD1_GetValue16(ADC_array);
    //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000

    Formula_ConcConvers_uSiemens(bin);

    if(struct_conc_print.conc_to_print!=conc_to_print_old)
    {
            SelectFont(CALIBRI_10);
            CleanArea_Ram_and_Screen(20,40,UN_MIS_Y_START,14);
            LCDPrintString("uS",26,UN_MIS_Y_START);
            LCD_CopyPartialScreen(20,40,UN_MIS_Y_START,14);


            SelectFont(CALIBRI_20);
            CleanArea_Ram_and_Screen(00,62,14,36);
            BinToBCDisp(struct_conc_print.conc_to_print,INTERO,4,14);
            LCD_CopyPartialScreen(00,62,14,36);

            conc_to_print_old=struct_conc_print.conc_to_print;
    }
}

//***************************************************************************************
void WorkMenu_CalcPrint_milliSiemens(unsigned int bin)
{
    static unsigned int conc_to_print_old=0xFFFFFFF;
      //AD1_GetValue16(ADC_array);
      //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
    Formula_ConcConvers_milliSiemens(bin);

     if(struct_conc_print.conc_to_print!=conc_to_print_old)
     {
              SelectFont(CALIBRI_10);
              CleanArea_Ram_and_Screen(20,40,UN_MIS_Y_START,14);
              LCDPrintString("mS",26,UN_MIS_Y_START);
              LCD_CopyPartialScreen(20,40,UN_MIS_Y_START,14);


              SelectFont(CALIBRI_20);
              CleanArea_Ram_and_Screen(00,62,14,36);
              BinToBCDisp(struct_conc_print.conc_to_print,INTERO,4,14);
              LCD_CopyPartialScreen(00,62,14,36);

              conc_to_print_old=struct_conc_print.conc_to_print;
      }
}
//****************************************************************************************************************************************************


/* _____      _      _____      _       _                                                __            _          __  ___    
  / ____|    | |    |  __ \    (_)     | |                                              / _|          | |        /_ |/ _ \   
 | |     __ _| | ___| |__) | __ _ _ __ | |_     ___ ___  _ __   ___    __  __  _   _   | |_ ___  _ __ | |_ ___    | | | | |  
 | |    / _` | |/ __|  ___/ '__| | '_ \| __|   / __/ _ \| '_ \ / __|   \ \/ / | | | |  |  _/ _ \| '_ \| __/ __|   | | | | |  
 | |___| (_| | | (__| |   | |  | | | | | |_   | (_| (_) | | | | (__     >  <  | |_| |  | || (_) | | | | |_\__ \   | | |_| |  
  \_____\__,_|_|\___|_|   |_|  |_|_| |_|\__|   \___\___/|_| |_|\___|   /_/\_\  \__, |  |_| \___/|_| |_|\__|___/   |_|\___/   
                                                                                __/ |                                        
                                                                               |___/   */                                      
//****************************************************************************************************************************************************
void CalcPrint_Percent_xy(unsigned int bin,unsigned int x,unsigned int y)
{
     Formula_ConcConvers_Percent(bin);
  
     SelectFont(CALIBRI_10);
     
     CleanArea_Ram_and_Screen(x,x+40,y,y+10);
     BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
     
     LCD_CopyPartialScreen(x,x+40,y,y+10);
}
//****************************************************************************************************************************************************
void CalcPrint_PuntTitol_xy(unsigned int bin,unsigned int x,unsigned int y)
{
      Formula_ConcConvers_PuntTitol(bin);
   
      SelectFont(CALIBRI_10);
     
      CleanArea_Ram_and_Screen(x,x+40,y,y+10);
      BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
      LCD_CopyPartialScreen(x,x+40,y,y+10);
}
//****************************************************************************************************************************************************
void CalcPrint_GrammiLitro_xy(unsigned int bin,unsigned int x,unsigned int y)
{
      Formula_ConcConvers_grammiLitro(bin);
  
      SelectFont(CALIBRI_10);
       
      CleanArea_Ram_and_Screen(x,x+40,y,y+10);
      BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
      LCD_CopyPartialScreen(x,x+40,y,y+10);

}
//****************************************************************************************************************************************************
void CalcPrint_uSiemens_xy(unsigned int bin,unsigned int x,unsigned int y)
{
    Formula_ConcConvers_uSiemens(bin);  
    SelectFont(CALIBRI_10);
   
    CleanArea_Ram_and_Screen(x,x+40,y,y+10);
    BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
    LCD_CopyPartialScreen(x,x+40,y,y+10);

}
//****************************************************************************************************************************************************
void CalcPrint_milliSiemens_xy(unsigned int bin,unsigned int x,unsigned int y)
{
    Formula_ConcConvers_milliSiemens(bin);
  
    SelectFont(CALIBRI_10);
   
    CleanArea_Ram_and_Screen(x,x+40,y,y+10);
    BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
    LCD_CopyPartialScreen(x,x+40,y,y+10);
}

//****************************************************************************************************************************************************
void IncrPrintConc_Percent_xy(unsigned short* val_to_incr,unsigned int x,unsigned int y,unsigned int incr)
{
  unsigned short decimillesimi,num_to_print;
  unsigned int   multiplied;
  unsigned short resto;
  unsigned int temp_test; 
  
  multiplied=*val_to_incr*10;
  decimillesimi=multiplied/64;
  resto=multiplied%64;
  if(decimillesimi>999)	{	 num_to_print=decimillesimi/10;  }
  else		  	{	 num_to_print=decimillesimi;	 }

  num_to_print-=incr;

  if(decimillesimi>999)	{	 decimillesimi=num_to_print*10;  }
  else		  	{	 decimillesimi=num_to_print;	 }
  multiplied=decimillesimi*64;
  multiplied+=resto;
  temp_test=multiplied/10;//prima di assegnare il nuovo valore alla varibile alla quale il programma far� riferimento lo testo
  if(!(temp_test>conc_soglie_limit_up.setp_e_soglie_arr[SET_CONC_INDEX]))
  {
         /* RamSettings.ptype_arr[RamSettings.selected_program_id].setp_e_soglie.setp_e_soglie_arr[index]*/*val_to_incr= temp_test;
  }
  else
  {

  }
  
  
}
//****************************************************************************************************************************************************
void IncrPrintConc_PuntTitol_xy(unsigned short* val_to_incr,unsigned int x,unsigned int y,unsigned int incr)
{
  
}
//****************************************************************************************************************************************************
void IncrPrintConc_GrammiLitro_xy(unsigned short* val_to_incr,unsigned int x,unsigned int y,unsigned int incr)
{
}
//****************************************************************************************************************************************************
void IncrPrintConc_uSiemens_xy(unsigned short* val_to_incr,unsigned int x,unsigned int y,unsigned int incr)
{
}
//****************************************************************************************************************************************************
void IncrPrintConc_milliSiemens_xy(unsigned short* val_to_incr,unsigned int x,unsigned int y,unsigned int incr)
{
  
}

//****************************************************************************************************************************************************
void LoadDisplay_Logo(void)
{
  
}
//****************************************************************************************************************************************************










