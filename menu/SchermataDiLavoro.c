#include "bsp.h"
#include "io.h"
#include "stdio.h"
#include "string.h"

#include "freertos.h"
#include "task.h"
//#include "ffile.h"
#include "meas.h"
#include "timers.h"
#include "i2c.h"

#include "my_definitions.h"
#include "Display_128x64.h"
//#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"

#include "VariabiliGlobali_di_Lavoro.h"
#include "keyboard.h"
#include "menu.h"
#include "my_types.h"
#include "ioexp.h"

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;
extern unsigned char screen_image[1024];
extern const char StringsSubmenuSimboliConc   	    [5][4];
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;


extern  TimerHandle_t xTimers[ NUM_TIMERS ];






//***************************************************************************************
void SchermataDiLavoro(void)
{
	uint8_t key;	//,test;
        float c_float,t_float;
       // float generic_float;
        
        unsigned int generic_ui;
        unsigned int un_misura;
        
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
        
        CLEAR_PUMP_STATES;
        MARK_PUMP_STATE_RIPOSO;
        
        MARK_PRINT_CONC_LIMITS;
        CLEAR_PRINT_PUMP;
        
        MARK_HEATER_STATE_RIPOSO;
        CLEAR_HEATER_STATE_ATTIVO;
        MARK_PRINT_TEMP_LIMITS;
        CLEAR_PRINT_HEATER;
        

        CLEAR_TEMP_ALARMS_MASK;
        CLEAR_CONC_ALARMS_MASK;  
        MARK_OVER_TEMP_NORMAL;
        MARK_OVER_CONC_NORMAL;
        
        if(CHECK_ACCENSIONE_CONC)MARK_PRINT_CONC_WAIT;
        if(CHECK_ACCENSIONE_TEMP)MARK_PRINT_TEMP_WAIT;
        
        un_misura=PROGR_IN_USO.unita_mis_concentr;

#if 0        
        immagine_stato_uscite=0x00;
        test=0;
        while(!test)
        {  
          test=I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
        }
#endif
		IOEXP_clr(0xff);	// clear all ioexp outputs
        
        if(CHECK_ACCENSIONE_CONC)
        {
          if( xTimerStart( xTimers[ TIMER1_RIT_ACC_CONC ], 0 ) != pdPASS ){}
        }
        
        if(CHECK_ACCENSIONE_TEMP)
        {  
          if( xTimerStart( xTimers[ TIMER6_RIT_ACC_TEMP ], 0 ) != pdPASS ){}  
        }
        //E' solo qui che marco lo stato della schermata di lavoro che rimane come stato_old e verrà confrontato
        //con le nuove condizioni della variabile RamSettings.abilita_disabilita,ce viene impostata solo nell'apposito menu
        if(RamSettings.abilita_disabilita==ABILITA)
        {
             if(CHECK_STATE_ABILITATO) 
             { 
                //non fare niente
             } 
             else//se ero disabilitato cancello 
             {
               //CleanArea_Ram_and_Screen(2,128,34,60); //cancello scritte Outs off
               MARK_STATE_ABILITATO;
              }
         }
        else //cioè if(RamSettings.abilita_disabilita==DISABILITA) //Se trovo disabilitato ed ero abilitato finora
        {
             SelectFont(CALIBRI_20);  
             CleanArea_Ram_and_Screen(2,28,42,54);//cancella area pompa
             CleanArea_Ram_and_Screen(6,124,34,56);
             LCDPrintString("OUT OFF",14,38);
             LCD_CopyPartialScreen(6,124,34,58);
           if(CHECK_STATE_ABILITATO) //allora scrivo
           { 
             CLEAR_STATE_ABILITATO;
           } 
           else//se ero giàdisabilitato 
           {
             //già a posto,non fare niente
           }
        }
        
        
        
         //valuto ancora la stessa variabile...lo faccio solo per chiarezza di lettura 
         if(RamSettings.abilita_disabilita==ABILITA)
         { 
          
           
           if(CHECK_TIMEOUT_CONC)
          {
            SelectFont(CALIBRI_10);
            CleanArea_Ram_and_Screen(2,64,42,64);//cancella pompa e limiti
            LCDPrintString("TimeOut",4,42);
            LCD_CopyPartialScreen(2,64,42,58); 
              
          }
          
          if(CHECK_TIMEOUT_TEMP)
          {
            SelectFont(CALIBRI_10);
            CleanArea_Ram_and_Screen(66,128,42,64);//cancella pompa e limiti
            LCDPrintString("TimeOut",68,42);
            LCD_CopyPartialScreen(66,128,42,58);
              
          }
        }
        else  //se ==DISABILITA
        {
            CLEAR_PRINT_CONC_LIMITS;
            CLEAR_PRINT_PUMP;
            CLEAR_PRINT_TEMP_LIMITS;
            CLEAR_PRINT_HEATER;
        }
        //
         
        
        
        
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
		  
                  StopAllTimers();
#if 0				  
                  immagine_stato_uscite=0x00;
                  test=0;
                  while(!test)
                  {  
                    test=I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
                  }
#endif
				  IOEXP_clr(0xff);		// clear all ioexp outputs

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
                       SelectFont(CALIBRI_20);
                      /*  +-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+
                         |C|O|M|P|E|N|S|A|Z|I|O|N|E| |T|K|
                         +-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+  */    
                   c_float=CompensConduc_TK(&measures.conduc);
                   /* +-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                      |C|A|L|C|O|L|O| |E| |S|T|A|M|P|A| |C|O|N|C|E|N|T|R|A|Z|I|O|N|E|
                      +-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
                   measures.meas_ok=0;
                   c_float=CalcoloConcent_Now(c_float);
                   PrintConc_WorkMenu(); 
                   MARK_CONTROL_CONC_ENA;
                  // c_float=global_float;//da qui in avanti i confronti sono fatti col valore non convertito in unità di misura

                }
                        /* +-+-+-+-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+
                           |F|I|N|E| |S|T|A|M|P|A| |V|A|L|O|R|I|
                           +-+-+-+-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+   */
                
                
                if(CHECK_ACCENSIONE_CONC |  CHECK_TIMEOUT_CONC)
                {
                   CLEAR_CONTROL_CONC_ENA;
                   
                }
                 
                
                if(CHECK_ACCENSIONE_TEMP | CHECK_TIMEOUT_TEMP)
                {
                  CLEAR_CONTROL_TEMP_ENA;
                } 
                

                
                if(RamSettings.abilita_disabilita==DISABILITA)
                {
                  CLEAR_CONTROL_CONC_ENA;
                  CLEAR_CONTROL_TEMP_ENA;
              
                }
                
                
                
                
                
                //ritardo all'accensione elettrica reale
                if(CHECK_PRINT_CONC_WAIT)
                {
                   SelectFont(CALIBRI_20);  
                   CleanArea_Ram_and_Screen(2,62,38,58);
                   LCDPrintString("  WAIT",2,38);
                   LCD_CopyPartialScreen   (2,62,38,58); 
                   CLEAR_PRINT_CONC_WAIT;

                }
                
                //ritardo all'accensione elettrica reale
                if(CHECK_PRINT_TEMP_WAIT)
                {
                   SelectFont(CALIBRI_20);  
                   CleanArea_Ram_and_Screen(66,126,38,58);
                   LCDPrintString("  WAIT",66,38);
                   LCD_CopyPartialScreen   (66,126,38,58); 
                   CLEAR_PRINT_TEMP_WAIT; 
               } 
                       
               
                if( CHECK_CONTROL_CONC_ENA) ControlloSoglieAllarmi_Conc(&c_float);
                if( CHECK_CONTROL_TEMP_ENA) ControlloSoglieAllarmi_Temp(&t_float); 
               
                ControlloRitardi();
                 

                if(CHECK_CONTROL_CONC_ENA)
                {  
                  if(CHECK_PRINT_PUMP)
                  {
                    /*CLEAR_PRINT_PUMP;
                    CleanArea_Ram_and_Screen(2,62,42,64);
                    mybmp_struct2.bmp_pointer=pompa_OK_bmp;
                    mybmp_struct2.righe	 =pompa_OKHeightPixels;
                    mybmp_struct2.colonne	 =pompa_OKWidthPages;
                    mybmp_struct2.start_x=2;
                    mybmp_struct2.start_y=42;
                    GetBitmap();
                    
                    PrintSoglia(PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc,32,54);
                    
                    LCD_CopyPartialScreen(2,26,42,64);*/
                  }
                  
                  
                  if(CHECK_PRINT_CONC_LIMITS)  //print limiti concentrazione
                  {
                    CLEAR_PRINT_CONC_LIMITS;
                    //pulisco disegno pompa
                    
                    
                    SelectFont(CALIBRI_10);
                    CleanArea_Ram_and_Screen(2,28,42,64);
                    


                    LCDPrintString("Max",2,42);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
                    CalcPrint_Conc_Only[un_misura](generic_ui,28,42);
                    LCDPrintString("Min",2,54);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
                    CalcPrint_Conc_Only[un_misura](generic_ui,28,54);
                    

                    LCD_CopyPartialScreen(2,50,42,64);
                  }
                } 
                
                
                 if(CHECK_CONTROL_TEMP_ENA)
                 { 
                  //mettere anche qui un bit
                  if(CHECK_PRINT_HEATER)
                  {
                   /* CLEAR_PRINT_HEATER;
                    CleanArea_Ram_and_Screen(70,124,42,64);
                    mybmp_struct2.bmp_pointer=riscaldatore_bmp;
                    mybmp_struct2.righe	 =riscaldatoreHeightPixels;
                    mybmp_struct2.colonne =riscaldatoreWidthPages;
                    mybmp_struct2.start_x=104;
                    mybmp_struct2.start_y=42;
                    GetBitmap();
                    
                    PrintSoglia(PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp,62,54);
                    
                    LCD_CopyPartialScreen(90,128,44,64);*/
                  }
                  
                  if(CHECK_PRINT_TEMP_LIMITS ) //print limiti temperatura
                  {
                    CLEAR_PRINT_TEMP_LIMITS;
                    SelectFont(CALIBRI_10);
                    CleanArea_Ram_and_Screen(70,128,42,64);//cancello disegno heater
                    

                    LCDPrintString("Max",70,42);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
                    generic_ui/=10;
                    BinToBCDisp(generic_ui,UN_DECIMALE,96,42);
                    LCDPrintString("Min",70,54);
                    generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp; ;
                    generic_ui/=10;
                    BinToBCDisp(generic_ui,UN_DECIMALE,96,54);
                    
                    LCD_CopyPartialScreen(70,128,42,64);
                    
                    
                  }
                  
                 }  
                   
                if(RamSettings.abilita_disabilita==ABILITA)
                {  
                      //**************** INTERVENTO!!!********************************************
                      //confronto con setpoint,isteresi ecc
                    ConcPump_AtWork   (&c_float);
                    TempHeater_AtWork (&t_float);
                 }
 
	}//fine while(1) loop di lavoro
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
            case PUMP_STATE_RIPOSO:
                      
              //verifico se vado sotto setp-isteresi 
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc);
              //generic_float/=100;
              if(*c_float < generic_float)
              {
                 CLEAR_PUMP_STATES;
                 MARK_PUMP_STATE_WAIT;
                
                
                 if( xTimerStart( xTimers[ TIMER3_RIT_DOSAGGIO ], 0 ) != pdPASS ){}
                 //il timer parte,se alla prox misura dovessi avere allarme devo impedire che accenda pompa

              }
             break;


            case PUMP_STATE_ATTIVO:
             
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc);
              //generic_float/=100;
              if(*c_float >generic_float) //se è abbastanza caldo
              {
                  CLEAR_PUMP_STATES;
                  MARK_PUMP_STATE_RIPOSO; 
                                    
                  MARK_PRINT_CONC_LIMITS;
                  CLEAR_PRINT_PUMP;
                  CleanArea_Ram_and_Screen(2,64,42,64);//cancello subito disegno pompa
				  IOEXP_clr(IOEXP0_PUMP_ENABLE);                  
                  //>>>>>>>>>>>>>Disable_Pump();    
              }
              else
              {
                     // if(RamSettings.abilita_disabilita==ABILITA)CleanArea_Ram_and_Screen(30,58,42,54);
              }

              break;
            
          case PUMP_STATE_WAIT :
              
            
              
              
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc );
              //generic_float/=100;
              
              if(*c_float >generic_float) //se è anche solo superiore a setpoint era falso intervento
              {   
                xTimerStop( xTimers[TIMER3_RIT_DOSAGGIO], 0 );
              
                CLEAR_PUMP_STATES;
                MARK_PUMP_STATE_RIPOSO; 
                
                
                MARK_PRINT_CONC_LIMITS;
                CLEAR_PRINT_PUMP;
               // CleanArea_Ram_and_Screen(2,28,42,64);//cancello subito disegno pompa
				IOEXP_clr(IOEXP0_PUMP_ENABLE);
              }
              //>>>>>>>>>>>>>Disable_Pump();    
           
          
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
                 
                  CleanArea_Ram_and_Screen(70,124,42,64);
                  mybmp_struct2.bmp_pointer=riscaldatore_bmp;
                  mybmp_struct2.righe	 =riscaldatoreHeightPixels;
                  mybmp_struct2.colonne =riscaldatoreWidthPages;
                  mybmp_struct2.start_x=104;
                  mybmp_struct2.start_y=42;
                  GetBitmap();
                  PrintSoglia(SOGLIE_SET_TEMP_INDEX,72,42);//come parametro gli basta l'id della soglia da mostrare
                  LCD_CopyPartialScreen(72,128,42,64);
                  

				  IOEXP_set(IOEXP0_HEATER_ENABLE);
                 CLEAR_PRINT_TEMP_LIMITS;
                 if( xTimerStart( xTimers[ TIMER7_TOUT_TEMP ], 0 ) != pdPASS ){} //Timeout parte qui              
              }
               }
              break;


            case HEATER_STATE_ATTIVO:
              {
              generic_float=(float)(PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp);
              generic_float/=100;
              if(*t_float > generic_float)
              {
                  
                  CLEAR_HEATER_STATE_ATTIVO;
                  MARK_HEATER_STATE_RIPOSO; 
                  
                  MARK_PRINT_TEMP_LIMITS;
                  CLEAR_PRINT_HEATER;
                                    
                  CleanArea_Ram_and_Screen(70,128,42,64);//cancello subito disegno heater
				  IOEXP_clr(IOEXP0_HEATER_ENABLE);
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


void CheckRiaccensione(void)
{
}
//***************************************************************************************
