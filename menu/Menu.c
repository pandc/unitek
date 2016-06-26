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
unsigned char menu_triang_limit_up;
unsigned char menu_triang_limit_dn;
unsigned char menu_triang_limit_dx;
unsigned char menu_triang_limit_sx;
unsigned char menu_triang_index;



 // An array to hold handles to the created timers.
 TimerHandle_t xTimers[ NUM_TIMERS ];
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
        LCD_Display_Setup();
	FontPointerInit(); 
        LoadDisplay_Logo();
        ffready(portMAX_DELAY);
        LoadRamSettingsFrom_External_DataFlash();

        MyCreateTimers();

	MenuFunction_Index=MENU_TEMPHUM;

	for (;;)
	{
		MARK_ARROW_KEYS_MOVE_UPDOWN;
		menu_triang_x=0;
		MenuFunctionPt[MenuFunction_Index]();
	}
}

//***************************************************************************************
void MenuInit(void)
{  
        //associo i puntatori a funzione menu a specifiche funzioni
	MenuFunctionPt[MENU_TEMPHUM]		=&SchermataDiLavoro;	    //0
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

        
        //associo i puntatori a funzione calcolo e stampa concentrazione a specifiche funzioni che tengono conto unità di misura
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
        
        MARK_HEATER_STATE_RIPOSO;
        MARK_PUMP_STATE_RIPOSO;
        CLEAR_TEMP_ALARMS_MASK;
        CLEAR_CONC_ALARMS_MASK;  
        MARK_OVER_TEMP_NORMAL;
        
        MARK_ACCENSIONE_CONC;
        MARK_ACCENSIONE_TEMP;
        if( xTimerStart( xTimers[ TIMER1_RIT_ACC_CONC ], 0 ) != pdPASS ){}
        if( xTimerStart( xTimers[ TIMER6_RIT_ACC_TEMP ], 0 ) != pdPASS ){}  
        
        
        
}

//***************************************************************************************
void SchermataDiLavoro(void)
{
	uint8_t key;
        float c_float,t_float;
       // float generic_float;
        
        unsigned int prova_x,prova_y,prova_x2,generic_ui;
        unsigned int un_misura;
        static unsigned char abilita_disabilita_old=0xFF;
	stato_intervento_conc=  STATO_POMPA_RIPOSO;//STATO_INIZIALE;
	stato_intervento_temper=STATO_RISC_RIPOSO;//STATO_INIZIALE;

	LCD_Fill_ImageRAM(0x00);

	//LCD_CopyScreen();  //perchè copiare uno schermo vuoto?Lo riempio man mano che utilizzo
          /* +-+-+-+-+-+-+ +-+-+-+-+-+-+-+
             |S|t|a|m|p|a| |s|i|m|b|o|l|i|
             +-+-+-+-+-+-+ +-+-+-+-+-+-+-+*/

	mybmp_struct2.bmp_pointer=gradi_bmp;
	mybmp_struct2.righe	 =gradi_HeightPixels;
	mybmp_struct2.colonne	 =gradi_WidthPages;
	mybmp_struct2.start_x=94;
	mybmp_struct2.start_y=2;
	GetBitmap();
        
   	SelectFont(CALIBRI_10);
	LCDPrintString("C",98,2);
	LCD_CopyScreen();
    
        CleanArea_Ram_and_Screen(24,48,2,14);
        LCDPrintString(StringsSubmenuSimboliConc[PROGR_IN_USO.unita_mis_concentr],24,2);
        LCD_CopyPartialScreen(24,48,2,14);
        
        MARK_PUMP_STATE_RIPOSO;
        MARK_PRINT_CONC_LIMITS;
        MARK_PRINT_PUMP;
        MARK_HEATER_STATE_RIPOSO;
        MARK_PRINT_TEMP_LIMITS;
        MARK_PRINT_HEATER;
        
        un_misura=PROGR_IN_USO.unita_mis_concentr;
        
        
        
        
        
        
        /*    _    ____ ____ ___     ___  _    _    ____ _  _ ____ ____ ____ 
              |    |  | |  | |__]    |  \ |    |    |__| |  | |  | |__/ |  | 
              |___ |__| |__| |       |__/ |    |___ |  |  \/  |__| |  \ |__| 
        */
	while(1)
	{       /* +-+-+-+-+-+
                   |T|A|S|T|I|
                   +-+-+-+-+-+*/
                
		if (key_getstroke(&key,200/*portMAX_DELAY*/) && (key == KEY_PROG))//pdMS_TO_TICKS
		{
		  MenuFunction_Index=MENU_PROGR;
                  //Funzione che disattiva il tutto,per esempio azzerare i timer
		  return;
		}
               /*+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                 |L|E|T|T|U|R|A| |T|E|M|P|E|R|A|T|U|R|A| |E| |C|O|N|C|E|N|T|R|A|Z|I|O|N|E|
                 +-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+    */
                if(measures.temp_ok)
                {
                 
                    SelectFont(CALIBRI_20);
                    CalcPrintTemperatura(&t_float);
                    measures.temp_ok=0;
                    
                    MARK_CONTROL_TEMP_ENA;
                    
                  
                }
                
                 if(measures.meas_ok)
                 {  
                     /*  +-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+
                         |C|O|M|P|E|N|S|A|Z|I|O|N|E| |T|K|
                         +-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+  */    
                   c_float=CompensConduc_TK(&measures.conduc);
                   /* +-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                      |C|A|L|C|O|L|O| |E| |S|T|A|M|P|A| |C|O|N|C|E|N|T|R|A|Z|I|O|N|E|
                      +-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
                   measures.meas_ok=0;
                   c_float=CalcoloConcent_Now(c_float);
                  
                   PrintConc_WorkMenu(&c_float); 

                   
                   
                   MARK_CONTROL_CONC_ENA;

                }
                        /* +-+-+-+-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+
                           |F|I|N|E| |S|T|A|M|P|A| |V|A|L|O|R|I|
                           +-+-+-+-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+   */
                
                
                if(CHECK_ACCENSIONE_CONC)
                {
                   MARK_PRINT_CONC_WAIT;
                   CLEAR_CONTROL_CONC_ENA;
                }
                
                
                if(CHECK_ACCENSIONE_TEMP)
                {
                   MARK_PRINT_TEMP_WAIT;
                   CLEAR_CONTROL_CONC_ENA;
                } 
               
                if(CHECK_PRINT_CONC_WAIT)
                {
                   SelectFont(CALIBRI_10);  
                   CleanArea_Ram_and_Screen(2,62,42,54);
                   LCDPrintString("  WAIT",10,38);
                   LCD_CopyPartialScreen   (2,62,42,54); 
                   CLEAR_PRINT_CONC_WAIT;

                }
                
                
                if(CHECK_PRINT_TEMP_WAIT)
                {
                   SelectFont(CALIBRI_10);  
                   CleanArea_Ram_and_Screen(66,126,42,54);
                   LCDPrintString("  WAIT",74,38);
                   LCD_CopyPartialScreen   (74,126,42,54); 
                   CLEAR_PRINT_TEMP_WAIT; 
               } 
               
               
                
                
          
                  SelectFont(CALIBRI_20);
                  //abilita_disabilita=DISABILITA;
                  if(RamSettings.abilita_disabilita==DISABILITA)
                  {
                       if(abilita_disabilita_old!=RamSettings.abilita_disabilita) 
                       { 
                           CleanArea_Ram_and_Screen(2,28,42,54);//cancella area pompa
                           CleanArea_Ram_and_Screen(6,124,34,56);
                           LCDPrintString("USCITE_OFF",6,38);
                           LCD_CopyPartialScreen(6,124,34,58);
                       }   
                  }
                  else
                  {
                       if(abilita_disabilita_old!=RamSettings.abilita_disabilita) 
                       {      
                            CleanArea_Ram_and_Screen(6,124,34,56);
                           
                           LCD_CopyPartialScreen(6,124,34,56);
                       }
                  }

                  abilita_disabilita_old=RamSettings.abilita_disabilita;
		
                
                
                if( CHECK_CONTROL_CONC_ENA) ControlloSoglieAllarmi_Conc(&c_float);
                if( CHECK_CONTROL_TEMP_ENA) ControlloSoglieAllarmi_Temp(&t_float); 
               
                ControlloRitardi();
 
                
              
                
                if(RamSettings.abilita_disabilita==ABILITA)
                {  
                      //**************** INTERVENTO!!!********************************************
                      //confronto con setpoint,isteresi ecc
                     if( CHECK_CONTROL_CONC_ENA)ConcPump_AtWork   (&c_float);
                     if( CHECK_CONTROL_TEMP_ENA) TempHeater_AtWork (&t_float);
               }

                if(CHECK_CONTROL_CONC_ENA)
                {  
                  if(CHECK_PRINT_PUMP)
                  {
                    CLEAR_PRINT_PUMP;
                    CleanArea_Ram_and_Screen(2,62,42,64);
                    mybmp_struct2.bmp_pointer=pompa_OK_bmp;
                    mybmp_struct2.righe	 =pompa_OKHeightPixels;
                    mybmp_struct2.colonne	 =pompa_OKWidthPages;
                    mybmp_struct2.start_x=2;
                    mybmp_struct2.start_y=42;
                    GetBitmap();
                    LCD_CopyPartialScreen(2,26,42,64);
                  }
                  
                  
                  if(CHECK_PRINT_CONC_LIMITS)  //print limiti concentrazione
                  {
                    CLEAR_PRINT_CONC_LIMITS;
                    //pulisco disegno pompa
                    
                    
                    SelectFont(CALIBRI_10);
                    CleanArea_Ram_and_Screen(2,28,42,64);
                    
                    prova_x =2;
                    prova_x2=28;
                    prova_y =42;

                    LCDPrintString("Max",prova_x,prova_y);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax;
                    CalcPrint_Conc_Only[un_misura](generic_ui,prova_x2,prova_y);
                    LCDPrintString("Min",prova_x,prova_y+12);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin;
                    CalcPrint_Conc_Only[un_misura](generic_ui,prova_x2,prova_y+12);
                    
                    
                    prova_x2=50;
                    LCD_CopyPartialScreen(2,prova_x2,42,64);
                  }
                } 
                
                
                 if(CHECK_CONTROL_CONC_ENA)
                 { 
                  //mettere anche qui un bit
                  if(CHECK_PRINT_HEATER)
                  {
                    CLEAR_PRINT_HEATER;
                    CleanArea_Ram_and_Screen(70,124,42,64);
                    mybmp_struct2.bmp_pointer=riscaldatore_bmp;
                    mybmp_struct2.righe	 =riscaldatoreHeightPixels;
                    mybmp_struct2.colonne	 =riscaldatoreWidthPages;
                    mybmp_struct2.start_x=104;
                    mybmp_struct2.start_y=42;
                    GetBitmap();
                    LCD_CopyPartialScreen(90,128,44,64);
                  }
                  
                  if(CHECK_PRINT_TEMP_LIMITS ) //print limiti temperatura
                  {
                    CLEAR_PRINT_TEMP_LIMITS;
                    SelectFont(CALIBRI_10);
                    CleanArea_Ram_and_Screen(70,128,42,64);
                    

                    LCDPrintString("Max",70,42);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMax;
                    generic_ui/=10;
                    BinToBCDisp(generic_ui,UN_DECIMALE,96,42);
                    LCDPrintString("Min",70,54);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMin ;
                    generic_ui/=10;
                    BinToBCDisp(generic_ui,UN_DECIMALE,96,54);
                    
                    LCD_CopyPartialScreen(70,128,42,64);
                    
                    
                  }
                  
                 }  
                   
                if(CHECK_PRINT_DISABILITA)
                {
                  
                  
                  
                }
                
                
                
                
                
                
                

	}//fine while(1) loop di lavoro
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
void ControlloSoglieAllarmi_Conc(float*c_float)
{
   float generic_float;
  //controllo concentrazioni,immagino che sia + grave se troppo alta,per cui la controllo per prima
   
  
   switch(CHECK_OVER_CONC_STATE)
   {
      case(OVER_CONC_NORMAL)://allora guardo se sfora soglie
        generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax +
                      (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
        generic_float/=100;
        if(*c_float>generic_float)
        {
          MARK_OVER_CONC_MAX;
          CLEAR_OVER_CONC_MIN;
          // non fermo altri timer,tanto marcano solo un flag xTimerStop
          if( xTimerStart( xTimers[ TIMER5_RIT_ALL_MAX_CONC ], 0 ) != pdPASS )
          {}// The timer could not be set into the Active state.  
        }
        else//controllo allarme basso solo se non ho quello alto
        {
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin -
                        (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
          generic_float/=100;
          if(*c_float < generic_float)
          {
            MARK_OVER_CONC_MIN;
            CLEAR_OVER_CONC_MAX;
          }
          if( xTimerStart( xTimers[ TIMER4_RIT_ALL_MIN_CONC ], 0 ) != pdPASS )
          {}// The timer could not be set into the Active state. 
        }
      break;


      case(OVER_CONC_MAX)://è già over,guardo se sfora soglie altro lato,o rientra normale
         generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin -
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
         generic_float/=100; 
         if(*c_float < generic_float)
          {
            MARK_OVER_CONC_MIN;
            CLEAR_OVER_CONC_MAX;
            if( xTimerStart( xTimers[ TIMER4_RIT_ALL_MIN_CONC ], 0 ) != pdPASS )
            {}// The timer could not be set into the Active state. 
          }
          else
          {
            generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax-
                            (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
            generic_float/=100;
            if(*c_float<generic_float)
            {
              MARK_OVER_CONC_NORMAL;
            }
         }
       break;


      case(OVER_CONC_MIN)://è già over,guardo se sfora soglie altro lato,o rientra normale
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax +
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
          generic_float/=100;
          if(*c_float > generic_float)
          {
            MARK_OVER_CONC_MAX;
            CLEAR_OVER_CONC_MIN;
            if( xTimerStart( xTimers[ TIMER4_RIT_ALL_MIN_CONC ], 0 ) != pdPASS )
            {}// The timer could not be set into the Active state. 
          }
          else
          {
            generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin+
                            (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
            generic_float/=100;
            if(*c_float>generic_float)
            {
              MARK_OVER_CONC_NORMAL;
              
              if( xTimerStart( xTimers[ TIMER4_RIT_ALL_MIN_CONC ], 0 ) != pdPASS )
              {}// The timer could not be set into the Active state. 
            }
       }
      break;

     
   }//fine switch
   
   
  //if(sensore_fusto==0)MARK_ALARM_FUSTO_VUOTO;
}
//***************************************************************************************
void ControlloSoglieAllarmi_Temp(float*t_float)
{
   //per prima cosa guardo che la temperatura non sia troppo alta
  float generic_float;
  
 //NB è solo in questa funzione che marco PRE_ALARM o ALARM
  
  
  //o rifaccio ogni volta i controlli,oppure li faccio solo se 
  //serve,cioè,se sono già in pralarm per quell'allarme inutile che lo controllo ancora
  //controllerò solo la condizione che lo fa rientrare
  //CLEAR_PREALARMS_MASK;//errore, me lo marca ancora anche s eè già partito il timer
  
  //ossia se sono già in un allarme intutile che controllo se andarci
  
  //se temperatura troppo alta marca allarme
   switch(CHECK_OVER_TEMP_STATE)
   {
      case(OVER_TEMP_NORMAL)://allora guardo se sfora soglie
        generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMax +
                      (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
        generic_float/=100;
        if(*t_float>generic_float)
        {
          MARK_OVER_TEMP_MAX;
          CLEAR_OVER_TEMP_MIN;
          CLEAR_ALARM_TEMP_MIN;
          CLEAR_TIMER9_EXPIRED;

          if( xTimerStart( xTimers[ TIMER9_RIT_ALL_MAX_TEMP ], 0 ) != pdPASS ){}
        }
        else//controllo allarme basso solo se non ho quello alto
        {
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMin -
                        (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
          generic_float/=100;
          if(*t_float < generic_float)
          {
            MARK_OVER_TEMP_MIN;
            CLEAR_OVER_TEMP_MAX;
            CLEAR_ALARM_TEMP_MAX;
            if( xTimerStart( xTimers[ TIMER8_RIT_ALL_MIN_TEMP ], 0 ) != pdPASS ){}
          }
        }
      break;


      case(OVER_TEMP_MAX)://è già over,guardo se sfora soglie altro lato,o rientra normale
         generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMin -
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
          generic_float/=100;
          if(*t_float < generic_float)
          {
            MARK_OVER_TEMP_MIN;
            CLEAR_OVER_TEMP_MAX;
            CLEAR_ALARM_TEMP_MAX;
            if( xTimerStart( xTimers[ TIMER8_RIT_ALL_MIN_TEMP ], 0 ) != pdPASS ){}
          }
          else
          {
            generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMax-
                            (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
            generic_float/=100;
            if(*t_float<generic_float)
            {
              MARK_OVER_TEMP_NORMAL;
              CLEAR_ALARM_TEMP_MAX;
            }
         }
       break;


      case(OVER_TEMP_MIN)://è già over,guardo se sfora soglie altro lato,o rientra normale
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMax +
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
          generic_float/=100;
          if(*t_float > generic_float)
          {
            MARK_OVER_TEMP_MAX;
            CLEAR_OVER_TEMP_MIN;
            CLEAR_ALARM_TEMP_MIN;
            CLEAR_TIMER9_EXPIRED;
            if( xTimerStart( xTimers[ TIMER9_RIT_ALL_MAX_TEMP ], 0 ) != pdPASS ){}
          }
          else
          {
            generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMin+
                            (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
            generic_float/=100;
            if(*t_float>generic_float)
            {
              MARK_OVER_TEMP_NORMAL;
              CLEAR_ALARM_TEMP_MIN;
            }
       }
      break;

     
   }//fine switch
  
}
//***************************************************************************************
void ConcPump_AtWork(float * c_float)
{
  float generic_float;
  
  if(CHECK_CONTROL_CONC_ENA)
   {  
      CLEAR_CONTROL_CONC_ENA;
      //lo stato in cui vado dipende dalla concentrazione e dallo stato precedente
     //mettere if allarme non fare niente
      if(!CHECK_CONC_ALARMS_MASK)//non la pulisco qua,la controllo solamente
      {  
          switch(CHECK_PUMP_STATE)
          {
             case PUMP_STATE_RIPOSO://verifico se vado sotto setp-isteresi
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc);
              generic_float/=100;
              if(*c_float < generic_float)
              {
                 CLEAR_PUMP_STATE_RIPOSO;
                 MARK_PUMP_STATE_WAIT;
                
                
                 if( xTimerStart( xTimers[ TIMER3_RIT_DOSAGGIO ], 0 ) != pdPASS ){}
                
              }
              break;


            case PUMP_STATE_ATTIVO:
             
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc);
              generic_float/=100;
              if(*c_float >generic_float) //se è abbastanza caldo
              {
                  MARK_PUMP_STATE_RIPOSO; 
                  CLEAR_PUMP_STATE_ATTIVO;
                  
                  MARK_PRINT_CONC_LIMITS;
                  CLEAR_PRINT_PUMP;
                  //>>>>>>>>>>>>>Disable_Pump();    
              }
              else
              {
                     // if(RamSettings.abilita_disabilita==ABILITA)CleanArea_Ram_and_Screen(30,58,42,54);
              }

              break;
            
          case PUMP_STATE_WAIT :
              
              
                    break;
             
          
          
            default:
                    break;
          
          
             }  
          
          }
      }
    
}
//***************************************************************************************
void TempHeater_AtWork(float * t_float)
{
  float generic_float;
  if(CHECK_CONTROL_TEMP_ENA)
   {  
      CLEAR_CONTROL_TEMP_ENA;

      //lo stato in cui vado dipende dalla concentrazione e dallo stato precedente
     //mettere if allarme non fare niente
      if(!(CHECK_TEMP_ALARMS_MASK))//non la pulisco qua,la controllo solamente
      {  
          switch(CHECK_HEATER_STATE)
          {
             case HEATER_STATE_RIPOSO://verifico se vado sotto setp-isteresi
               {
               generic_float= (float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp);
               generic_float/=100;
               if(*t_float < generic_float)
              {
                 CLEAR_HEATER_STATE_RIPOSO;
                 MARK_HEATER_STATE_ATTIVO;
                 MARK_PRINT_HEATER;
                 CLEAR_PRINT_TEMP_LIMITS;
                 if( xTimerStart( xTimers[ TIMER7_TOUT_TEMP ], 0 ) != pdPASS ){}               
              }
               }
              break;


            case HEATER_STATE_ATTIVO:
              {
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp);
              generic_float/=100;
              if(*t_float > generic_float)
              {
                  MARK_HEATER_STATE_RIPOSO; 
                  CLEAR_HEATER_STATE_ATTIVO;
                  CLEAR_PRINT_HEATER;
                  MARK_PRINT_TEMP_LIMITS;
                  //>>>>>>>>>>>>>Disable_Pump();    
              }
              else
              {
                      //if(RamSettings.abilita_disabilita==ABILITA)CleanArea_Ram_and_Screen(30,58,42,54);
              }
              }
              break;
            
          
          
            default:
              MARK_HEATER_STATE_RIPOSO;       
              break;
          
          
             }  
          
          }
      }
}
//***************************************************************************************
void CalcPrintTemperatura(float * t_float)
{
  char string_to_print[16];
  unsigned int len;
  //float generic_float;
  
  
  *t_float=measures.temp_resist;
  Convers_Res_to_Temp(t_float);
  
 /*+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+ +-+-+
  |S|T|A|M|P|A| |T|E|M|P|E|R|A|T|U|R|A| |F|O|N|T| |2|0|
  +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+ +-+-+  */             
 
 CleanArea_Ram_and_Screen(60,120,14,36);
 //BinToBCDisp(ADC_array[LETTURA_TEMP]/*temperature_to_print*/,UN_DECIMALE,68,14);
 sprintf(string_to_print,"%.1f",*t_float);//pot=206,5 ohm
 len=strlen(string_to_print);
 LCDPrintString(string_to_print,125-(width_font*len)-5,14);
 LCD_CopyPartialScreen(60,120,14,36);
 
  
}
//***************************************************************************************
void PrintConc_WorkMenu(float* c_float)
{
  char string_to_print[16];
  unsigned int len;
   
   switch (struct_conc_print.decimali_to_print)
   {
     case INTERO:
        sprintf(string_to_print,"%.0f",*c_float);//pot=206,5 ohm
        break;
                          
      case UN_DECIMALE:
        sprintf(string_to_print,"%.1f",*c_float);//pot=206,5 ohm
      break;
      
      case DUE_DECIMALI:
        sprintf(string_to_print,"%.2f",*c_float);//pot=206,5 ohm
      break;
      
      default:
      break;
    }
   CleanArea_Ram_and_Screen(00,62,14,36);
   len=strlen(string_to_print);
   LCDPrintString(string_to_print,62-(width_font*len)-5,14);
   LCD_CopyPartialScreen(00,62,14,36);

  
}
//***************************************************************************************
void ControlloRitardi(void)
{
  //se ci sono preallarmi
  //controllo ritardo allarme scaduto
  
  
  //controllo allarmi min e max temperatura
  
    if(CHECK_TIMER9_EXPIRED)
    {
     if(CHECK_OVER_TEMP_MAX)
      {
        MARK_ALARM_TEMP_MAX;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        //SPEGNI HEATER
        //DISABILITA FUNZ LAV TEMPERATURA
      }
      CLEAR_TIMER9_EXPIRED;
      
    }
    
    if(CHECK_TIMER8_EXPIRED)
    {
      if(CHECK_OVER_TEMP_MIN)
      {
        MARK_ALARM_TEMP_MIN;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        //SPEGNI HEATER
        //DISABILITA FUNZ LAV TEMPERATURA
      }
      CLEAR_TIMER8_EXPIRED;
    }
  
  
  
    //controllo allarmi min e max concentrazione

    if(CHECK_TIMER5_EXPIRED)
    {
      CLEAR_TIMER5_EXPIRED;
      if(CHECK_OVER_CONC_MAX)
      {
        MARK_ALARM_CONC_MAX;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        //SPEGNI POMPA
        //DISABILITA FUNZ LAV CONCENTRAZIONE
      }
    }
    
    if(CHECK_TIMER4_EXPIRED)
    {
      CLEAR_TIMER4_EXPIRED;
      if(CHECK_OVER_CONC_MIN)
      {
        MARK_ALARM_CONC_MIN;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        //SPEGNI POMPA
        //DISABILITA FUNZ LAV CONCENTRAZIONE
      }
    }
    
    
    
    if(CHECK_TIMER3_EXPIRED)
    {
      CLEAR_TIMER3_EXPIRED;
      if(CHECK_PUMP_STATE_WAIT)
      {
        MARK_PUMP_STATE_ATTIVO;
        CLEAR_PUMP_STATE_WAIT;
                //>>>>>>>>>>>>>Enable_Pump();
        if( xTimerStart( xTimers[ TIMER2_TOUT_DOSAGGIO ], 0 ) != pdPASS ){}
        MARK_PRINT_PUMP;
        CLEAR_PRINT_CONC_LIMITS;
      }
    }
 
  
    
    if(CHECK_TIMER2_EXPIRED)//se non ho raggiunto conc in tempo limite blocco ,mettere bit
    {
      CLEAR_TIMER2_EXPIRED;
      if(CHECK_PUMP_STATE_ATTIVO)
      {
        MARK_PUMP_STATE_WAIT;//tasnto non entro nemmeno nella funzione di lavoro pompa
        //dovrei restare in wait perchè nessun timer mi fa uscire
        //>>>>>>>>>>>>>Disable_Pump();
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(2,64,42,64);//cancella pompa e limiti
        LCDPrintString("T OUT",18,42);
        LCD_CopyPartialScreen(2,64,42,58);
            
        //MARK_PRINT_TIMOUT;
        CLEAR_PRINT_CONC_LIMITS;
        CLEAR_PRINT_PUMP;
      }
    }
    
    
    
    
    
    if(CHECK_TIMER7_EXPIRED)
    {
      CLEAR_TIMER7_EXPIRED;
      if(CHECK_HEATER_STATE_ATTIVO)
      {
        MARK_HEATER_STATE_RIPOSO;//tanto non entro nemmeno nella funzione di lavoro pompa
        
        //>>>>>>>>>>>>>Disable_Heather();
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(66,128,42,64);//cancella pompa e limiti
        LCDPrintString("T OUT",82,42);
        LCD_CopyPartialScreen(66,128,42,58);
        //MARK_PRINT_TIMOUT;
        CLEAR_PRINT_TEMP_LIMITS;
        CLEAR_PRINT_HEATER;
      }
    }
}
//***************************************************************************************
void CheckRiaccensione(void)
{
  //CLEAR_PREALARMS_MASK;
  
  
}  













