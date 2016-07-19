/*
 * Menu.c
 *
 *  Created on: 19/mar/2016
 *      Author: Administrator
 */
#include "bsp.h"
#include "io.h"
#include "stdio.h"
#include "string.h"

#include "freertos.h"
#include "task.h"
#include "ffile.h"
#include "meas.h"
#include "timers.h"
#include "i2c.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"

#include "VariabiliGlobali_di_Lavoro.h"
#include "keyboard.h"
#include "menu.h"
#include "my_types.h"
#include "ioexp.h"

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

unsigned char MenuFunction_Index;

print_conc_var struct_conc_print;



unsigned int menu_triang_x,menu_triang_y;
unsigned char menu_triang_limit_up;
unsigned char menu_triang_limit_dn;
unsigned char menu_triang_limit_dx;
unsigned char menu_triang_limit_sx;
unsigned char menu_triang_index;




// An array to hold handles to the created timers.
TimerHandle_t xTimers[ NUM_TIMERS ];
TimerHandle_t AuxTimer; 

// An array to hold a count of the number of times each timer expires.
//int32_t lExpireCounters[ NUM_TIMERS ] = { 0 };


extern unsigned char screen_image[1024];

extern unsigned int blink_timer_on;
extern unsigned int blink_timer_off;
extern unsigned char loop_flag;

extern const char StringsMenuProg      [4][6][20];
extern const char StringsSubmenuSimboliConc   	    [5][4];
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;
extern unsigned char width_font;
//dovrebbero essere costanti in flash,ma non riesco a inizializzarle
//***************************************************************************************
void (*MenuFunctionPt[30])(void);
void (*WorkMenu_CalcPrint_UnMisura_Conc [5])(unsigned int);
void (*CalcPrint_UnMisura_Conc          [5])(unsigned int ,unsigned int,unsigned int);
void (*CalcPrint_Conc_Only              [5])(unsigned int ,unsigned int,unsigned int);

void (*Formula_ConcConvers              [5])(unsigned int );
unsigned int(* FormulaInversa_Conc      [5])(void);
//***************************************************************************************
static void menu_task(void *par)
{
    unsigned char input_status;
  
    LCD_Display_Setup();
    FontPointerInit(); 
    LoadDisplay_Logo();
    ffready(portMAX_DELAY);
    LoadRamSettingsFrom_External_DataFlash();
    //se voglio forzare uscite disabilitate indipendentemnte da quanto mem lo faccio qui
    
    MyCreateTimers();
      
    input_status=IOEXP_get();
    //impostazioni al power on
    if(input_status & DISABIL_CONC_EXT)//controllo input,se interrutore è in OFF
    {  //se arrivo da stato OFF dovrei avere già fatto tutto,trane la stampa,se arrivo da fuori
      CLEAR_ATTIVAZIONE_EXT_CH_CONC;
      MARK_PRINT_CONC_CH_OFF;// se arrivo da OFF ho già stampato,ma se arrivo da fuori ho già i flags che mi dicono di stampare
      CLEAR_PRINT_ALARMS_CONC_MASK;
      CLEAR_PRINT_CONC_LIMITS;
    }
    else  //se inizialmente switch CONC è ON
    {
      MARK_ACCENSIONE_CONC;
      //if( xTimerStart( xTimers[ TIMER1_RIT_ACC_CONC ], 0 ) != pdPASS ){}
      MARK_PRINT_CONC_WAIT;
      
      MARK_ATTIVAZIONE_EXT_CH_CONC;
      CLEAR_PUMP_STATES;
      MARK_PUMP_STATE_RIPOSO;

     } 
    
    
    
    if(input_status & DISABIL_TEMP_EXT)//controllo input,se interrutore è in OFF
    {  //se arrivo da stato OFF dovrei avere già fatto tutto,trane la stampa,se arrivo da fuori
      CLEAR_ATTIVAZIONE_EXT_CH_TEMP;
      MARK_PRINT_TEMP_CH_OFF;// se arrivo da OFF ho già stampato,ma se arrivo da fuori ho già i flags che mi dicono di stampare
      CLEAR_PRINT_ALARMS_TEMP_MASK;
      CLEAR_PRINT_TEMP_LIMITS;
    }
    else  //se inizialmente switch TEMP è ON
    {
      MARK_ACCENSIONE_TEMP;
      //if( xTimerStart( xTimers[ TIMER6_RIT_ACC_TEMP ], 0 ) != pdPASS ){}  
      MARK_PRINT_TEMP_WAIT;
      
      MARK_ATTIVAZIONE_EXT_CH_TEMP;
      CLEAR_HEATER_STATES;
      MARK_HEATER_STATE_RIPOSO;
      
    } 
    


 
  
    

 





    
    if(RamSettings.abilita_disabilita==ABILITA) MARK_STATE_ABILITATO;
    else                                        CLEAR_STATE_ABILITATO;

    

    MenuFunction_Index=SCHERM_LAV;

    for (;;)
    {
            MARK_ARROW_KEYS_MOVE_UPDOWN;
            menu_triang_x=0;
            menu_triang_index=0;
            menu_triang_y=2;
            MenuFunctionPt[MenuFunction_Index]();
    }
}

//***************************************************************************************
void MenuInit(void)
{  
        //associo i puntatori a funzione menu a specifiche funzioni
	MenuFunctionPt[SCHERM_LAV]		=&SchermataDiLavoro;	    //0
	MenuFunctionPt[MENU_PROGR]		=&MenuProg;			//1
	MenuFunctionPt[SUBMENU_INOUT]		=&SubmenuINOUT;		//2
	MenuFunctionPt[SUBMENU_SELEZIONA_PROG]	=&SubmenuSelProgr;	//3
	MenuFunctionPt[SUBMENU_COMUNIC]		=&SubmenuComunic;	//4
	MenuFunctionPt[SUBMENU_SETCLOCK]	=&SubmenuSetClock;  //5
	MenuFunctionPt[SUBMENU_SEL_LINGUA]	=&SumMenuSelLingua;	//6
	MenuFunctionPt[SUBMENU_SERVIZIO]	=&SubmenuServizio;	//7
	MenuFunctionPt[SUBMENU_SELECTED_PROGR]	=&SubmenuSelectedProgr;	//8
        MenuFunctionPt[SUB2MENU_CAL_PT100]	=&Sub2Menu_Cal_PT100;
        MenuFunctionPt[SUB2MENU_CABLE_COMPENS]	=&Sub2Menu_Cal_Cable;
        MenuFunctionPt[SUB2MENU_MISURA_DIRETTA]	=&Sub2Menu_MisuraDiretta;
        MenuFunctionPt[SUB2MENU_LICENZA]	=&Sub2Menu_Licenza;


	MenuFunctionPt[SUB2MENU_IMPOSTA_SIMBOLI]	=&Sub2MenuImpostaSimboli;
	MenuFunctionPt[SUB2MENU_TK]			=&Sub2MenuTK;
	//MenuFunctionPt[SUB3MENU_CURVA_DI_LAVORO]	=&Sub2MenuCurvadiLavoro;
	MenuFunctionPt[SUB2MENU_IMPOSTA_SOGLIE] 	=&Sub2MenuImpostaSoglie;
	MenuFunctionPt[SUB2MENU_IMPOSTA_TIMER]  	=&Sub2MenuImpostaTimer;
	MenuFunctionPt[SUB2MENU_SEL_TIPO_CURV_LAV]      =&Sub2MenuSelTipoCurvaLavoro;
	MenuFunctionPt[SUB3MENU_CURVA_DI_LAVORO3pt]	=&Sub2MenuCurvadiLavoro3Punti;
        MenuFunctionPt[SUB3MENU_SEL_LCH]		=&Sub2Sel_L_C_H;
        

        
        //associo i puntatori a funzione calcolo e stampa concentrazione a specifiche funzioni che tengono conto unità di misura
        //vale solo per schermata iniziale con fonts=20
     /*  WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PERCENTUALE ]=WorkMenu_CalcPrint_Percent;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PUNT_TITOL  ]=WorkMenu_CalcPrint_PuntTitol;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_GRAMMILITRO ]=WorkMenu_CalcPrint_GrammiLitro;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_uSIEMENS    ]=WorkMenu_CalcPrint_uSiemens;
       WorkMenu_CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_mSIEMENS    ]=WorkMenu_CalcPrint_milliSiemens;*/
       
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PERCENTUALE ]=CalcPrint_Percent_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_PUNT_TITOL  ]=CalcPrint_PuntTitol_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_GRAMMILITRO ]=CalcPrint_GrammiLitro_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_uSIEMENS    ]=CalcPrint_uSiemens_xy;
       CalcPrint_UnMisura_Conc[UNIT_MIS_CONCENTR_mSIEMENS    ]=CalcPrint_milliSiemens_xy;
       
       CalcPrint_Conc_Only[UNIT_MIS_CONCENTR_PERCENTUALE ]=CalcPrintOnly_Percent_xy;
       CalcPrint_Conc_Only[UNIT_MIS_CONCENTR_PUNT_TITOL  ]=CalcPrintOnly_PuntTitol_xy;
       CalcPrint_Conc_Only[UNIT_MIS_CONCENTR_GRAMMILITRO ]=CalcPrintOnly_GrammiLitro_xy;
       CalcPrint_Conc_Only[UNIT_MIS_CONCENTR_uSIEMENS    ]=CalcPrintOnly_uSiemens_xy;
       CalcPrint_Conc_Only[UNIT_MIS_CONCENTR_mSIEMENS    ]=CalcPrintOnly_milliSiemens_xy;
       
    
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_PERCENTUALE ]=Formula_ConcConvers_Percent;
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_PUNT_TITOL  ]=Formula_ConcConvers_PuntTitol;
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_GRAMMILITRO ]=Formula_ConcConvers_grammiLitro; 
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_uSIEMENS    ]=Formula_ConcConvers_uSiemens;
       Formula_ConcConvers      [UNIT_MIS_CONCENTR_mSIEMENS    ]=Formula_ConcConvers_milliSiemens;
       //limiti massimi e minimi di set points e soglie,per impedire che vengano impostati valori troppo alti o tropo bassi
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
		{

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
				
			}
		}

		if (key == KEY_PROG)
		
		{
			
			MenuFunction_Index=SCHERM_LAV;
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















