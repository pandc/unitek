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
extern const char StringsSubmenuSimboliConc   [5][4];
extern const char StringsFustoVuotoUp  [4][10];
extern const char StringsFustoVuotoDn  [4][10];
extern const char StringsCableOpen     [4][10];
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;


extern  TimerHandle_t xTimers[ NUM_TIMERS ];
extern  TimerHandle_t AuxTimer; 

//***************************************************************************************
//questa serve a inizializzare in uno stato ben definito quando entro da power on o da Menu nella schermata di lavoro
//e non quando rientro da ON OFF
void SchermLavoroInitCondition(void)
{
  
  if(CHECK_ACCENSIONE_CONC)if( xTimerStart( xTimers[ TIMER1_RIT_ACC_CONC ], 0 ) != pdPASS ){}
  if(CHECK_ACCENSIONE_TEMP)if( xTimerStart( xTimers[ TIMER6_RIT_ACC_TEMP ], 0 ) != pdPASS ){}  
  
  
  CLEAR_CABLE_OPEN;
  //forzo stato iniziale attuatori e dico di non stamparli
  CLEAR_PUMP_STATES;
  MARK_PUMP_STATE_RIPOSO;
  CLEAR_PRINT_PUMP;
  CLEAR_CONC_ALARMS_MASK; //tutti gli allarmi disattivati 
  MARK_OVER_CONC_NORMAL;
  CLEAR_PRINT_ALARMS_CONC_MASK;//parte senza disposizione di stampare nessun allarme
  CLEAR_PRINT_CONC_CH_OFF;
  CLEAR_CONTROL_CONC_ENA;
             
  
  CLEAR_HEATER_STATES;
  MARK_HEATER_STATE_RIPOSO;
  CLEAR_PRINT_HEATER;
  CLEAR_TEMP_ALARMS_MASK;//tutti gli allarmi disattivati
  MARK_OVER_TEMP_NORMAL;
  CLEAR_PRINT_ALARMS_TEMP_MASK;//parte senza disposizione di stampare nessun allarme
  CLEAR_PRINT_TEMP_CH_OFF;
  CLEAR_CONTROL_TEMP_ENA;
  
  //qui forzo una stampata iniziale per quando rientro da menu,
  //per non lasciare vuoto
  //anche se avendo messo il CheckPrint alla fine sarà 
  //la condizione rilevata a dirmi cosa stampare
  if(CHECK_ACCENSIONE_CONC)MARK_PRINT_CONC_WAIT;
  else
  {
    if(!CHECK_ATTIVAZIONE_EXT_CH_CONC)MARK_PRINT_CONC_CH_OFF;
    else  
    {
      MARK_PRINT_CONC_LIMITS;
    }
  }
  
  if(CHECK_ACCENSIONE_TEMP)MARK_PRINT_TEMP_WAIT;
  else 
  {
    if(!CHECK_ATTIVAZIONE_EXT_CH_TEMP)MARK_PRINT_TEMP_CH_OFF;
    else
    {
      MARK_PRINT_TEMP_LIMITS;   
    }
  }
  IOEXP_clr(0xff);	// clear all ioexp outputs
  //serve solo in caso PowerOn

  
   //E' solo qui che marco lo stato della schermata di lavoro che rimane come stato_old e verrà confrontato
  //con le nuove condizioni della variabile RamSettings.abilita_disabilita,che viene impostata solo nell'apposito menu
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
       RigaVertic(64,0,64);
       LCD_CopyPartialScreen(64,66,0,64);
  }
  else //cioè if(RamSettings.abilita_disabilita==DISABILITA) //Se trovo disabilitato ed ero abilitato finora
  {
       SelectFont(CALIBRI_20);  
       //CleanArea_Ram_and_Screen(2,28,42,54);//cancella area pompa
       CleanArea_Ram_and_Screen(2,124,34,56);
       LCDPrintString("OUT  OFF",20,38);
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
      CleanArea_Ram_and_Screen(2,64,40,64);//cancella pompa e limiti
      LCDPrintString("TO",TO_CONC_X_START,TO_CONC_Y_START);
      LCD_CopyPartialScreen(TO_CONC_X_START,TO_CONC_X_END,TO_CONC_Y_START,TO_CONC_Y_END);
        
    }
    
    if(CHECK_TIMEOUT_TEMP)
    {
      SelectFont(CALIBRI_10);
      CleanArea_Ram_and_Screen(66,128,40,64);//cancella heater e limiti
      LCDPrintString("TO",TO_TEMP_X_START,TO_TEMP_Y_START);
      LCD_CopyPartialScreen(TO_TEMP_X_START,TO_TEMP_X_END,TO_TEMP_Y_START,TO_TEMP_Y_END);
        
    }
  }
  else  //se ==DISABILITA
  {
      CLEAR_PRINT_CONC_LIMITS;
      CLEAR_PRINT_PUMP;
      CLEAR_PRINT_TEMP_LIMITS;
      CLEAR_PRINT_HEATER;
  }
  
} 

//***************************************************************************************
void ResetChConc_OFF(void)
{
  //quando vado in off devo:
  //disattivare subito le uscite
  IOEXP_clr(IOEXP0_CONC_MASK);	// solo le uscite conc
  //azzerare gli allarmi
  CLEAR_CONC_ALARMS_MASK; 
  CLEAR_TIMEOUT_CONC;
  
  MARK_OVER_CONC_NORMAL;
  //fermare eventuali timers(anche blink se non si decide di usarlo)
  Stop_ChConc_Timers();
  //pulire i bit richiesta stampa
  CLEAR_PUMP_STATE_ATTIVO;
  CLEAR_PRINT_CONC_LIMITS;
  CLEAR_PRINT_PUMP;
  CLEAR_PRINT_ALARMS_CONC_MASK;
  CLEAR_PRINT_CONC_TO;
  
  CLEAR_ACCENSIONE_CONC;
  
  //marcare riaccensione per quel canale
 
  
}
//***************************************************************************************
void ResetChTemp_OFF(void)
{
  //quando vado in off devo:
  //disattivare subito le uscite
  IOEXP_clr(IOEXP0_TEMP_MASK);	// solo le uscite temp
  //azzerare gli allarmi
  CLEAR_TEMP_ALARMS_MASK;  
  CLEAR_TIMEOUT_TEMP;
  
  MARK_OVER_TEMP_NORMAL;
  //fermare eventuali timers(anche blink se non si decide di usarlo)
  Stop_ChConc_Timers();
  //pulire i bit richiesta stampa
  CLEAR_HEATER_STATE_ATTIVO;
  CLEAR_PRINT_TEMP_LIMITS;
  CLEAR_PRINT_PUMP;
  CLEAR_PRINT_ALARMS_TEMP_MASK;
  CLEAR_PRINT_TEMP_TO;
  
  CLEAR_ACCENSIONE_TEMP;
  //marcare riaccensione per quel canale
  
  
}

//***************************************************************************************
void ResetChConc_ON(void)
{
  //MARK_ATTIVAZIONE_EXT_CH_CONC;
  MARK_ACCENSIONE_CONC;
  if( xTimerStart( xTimers[ TIMER1_RIT_ACC_CONC ], 0 ) != pdPASS ){}
  MARK_PRINT_CONC_WAIT;

  if(!CHECK_ACCENSIONE_CONC)//dovrebbe essere inutile perchè ho forzato il MARK_ACCENSIONE_CONC
  {
    CLEAR_PUMP_STATES;
    MARK_PUMP_STATE_RIPOSO;
    MARK_PRINT_CONC_LIMITS;
  }
}
//***************************************************************************************
void ResetChTemp_ON(void)
{
  //MARK_ATTIVAZIONE_EXT_CH_TEMP;
  MARK_ACCENSIONE_TEMP;
  if( xTimerStart( xTimers[ TIMER6_RIT_ACC_TEMP ], 0 ) != pdPASS ){}
  MARK_PRINT_TEMP_WAIT;

  if(!CHECK_ACCENSIONE_TEMP)//dovrebbe essere inutile perchè ho forzato il MARK_ACCENSIONE_TEMP
  {
    CLEAR_PUMP_STATES;
    MARK_HEATER_STATE_RIPOSO;
    MARK_PRINT_TEMP_LIMITS;
  } 
}
//***************************************************************************************
void SchermLav_ScritteFisse(void)
{
   LCD_Fill_ImageRAM(0x00);

    
      /* +-+-+-+-+-+-+ +-+-+-+-+-+-+-+
         |S|t|a|m|p|a| |s|i|m|b|o|l|i|
         +-+-+-+-+-+-+ +-+-+-+-+-+-+-+*/

    mybmp_struct2.bmp_pointer=gradi_bmp;
    mybmp_struct2.righe	     =gradi_HeightPixels;
    mybmp_struct2.colonne    =gradi_WidthPages;
    mybmp_struct2.start_x=94;
    mybmp_struct2.start_y=2;
    GetBitmap();
    
    SelectFont(CALIBRI_10);
    LCDPrintString("C",98,2);
    LCD_CopyScreen();

    CleanArea_Ram_and_Screen(24,48,2,14);
    LCDPrintString(StringsSubmenuSimboliConc[PROGR_IN_USO.unita_mis_concentr],24,2);
    LCD_CopyPartialScreen(24,48,2,14);
}


//***************************************************************************************

void CheckPrint_SchermataDiLavoro(void)
{
    //ritardo all'accensione elettrica reale
    unsigned int generic_ui;
    unsigned int un_misura;
    
    un_misura=PROGR_IN_USO.unita_mis_concentr;
    
    
    if(CHECK_CONC_ALARMS_MASK || CHECK_TIMEOUT_CONC)//un controllo in + per evitare di stampare 
    {
      CLEAR_PRINT_PUMP;
      CLEAR_PRINT_CONC_LIMITS;
    }
    
    if(CHECK_TEMP_ALARMS_MASK || CHECK_TIMEOUT_TEMP)//un controllo in + per evitare di stampare 
    {
      CLEAR_PRINT_HEATER;
      CLEAR_PRINT_TEMP_LIMITS;
    }
    
    
    
  
    if(CHECK_PRINT_CONC_WAIT)
    {
       CLEAR_PRINT_CONC_WAIT;
       SelectFont(CALIBRI_20);
       CleanArea_Ram_and_Screen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END,
                                WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);
       LCDPrintString(" WAIT",2,38);
       LCD_CopyPartialScreen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END,
                             WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);
       
       MARK_FULL_SCREEN_CONC;
    }

    //ritardo all'accensione elettrica reale
    if(CHECK_PRINT_TEMP_WAIT)
    {
       CLEAR_PRINT_TEMP_WAIT; 
       SelectFont(CALIBRI_20);  
       //DP INSERITI DEFINE
       CleanArea_Ram_and_Screen(WAIT_TEMP_AREA_X_START,WAIT_TEMP_AREA_X_END,
                                WAIT_TEMP_AREA_Y_START,WAIT_TEMP_AREA_Y_END);
       LCDPrintString("  WAIT",66,38);
       LCD_CopyPartialScreen   (WAIT_TEMP_AREA_X_START,WAIT_TEMP_AREA_X_END,
                                WAIT_TEMP_AREA_Y_START,WAIT_TEMP_AREA_Y_END); 
       
       MARK_FULL_SCREEN_TEMP;
    } 
    
    
     if(CHECK_PRINT_CONC_CH_OFF)
    {
       CLEAR_PRINT_CONC_CH_OFF;
       SelectFont(CALIBRI_20);
       CleanArea_Ram_and_Screen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END,
                                WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);
       LCDPrintString(" OFF",12,38);
       LCD_CopyPartialScreen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END,
                             WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);
       
       MARK_FULL_SCREEN_CONC;
    }

    //ritardo all'accensione elettrica reale
    if(CHECK_PRINT_TEMP_CH_OFF)
    {
       CLEAR_PRINT_TEMP_CH_OFF;
       SelectFont(CALIBRI_20);  
       //DP INSERITI DEFINE
       CleanArea_Ram_and_Screen(WAIT_TEMP_AREA_X_START,WAIT_TEMP_AREA_X_END,
                                WAIT_TEMP_AREA_Y_START,WAIT_TEMP_AREA_Y_END);
       LCDPrintString("  OFF",76,38);
       LCD_CopyPartialScreen   (WAIT_TEMP_AREA_X_START,WAIT_TEMP_AREA_X_END,
                                WAIT_TEMP_AREA_Y_START,WAIT_TEMP_AREA_Y_END); 
       
       MARK_FULL_SCREEN_TEMP;
    } 
    
    
    

      
      
      
      if(CHECK_PRINT_PUMP)
      {
        CLEAR_PRINT_PUMP;
        CleanArea_Ram_and_Screen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END,
                                 WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);
        mybmp_struct2.bmp_pointer=pompa_OK_bmp;
        mybmp_struct2.righe	 =pompa_OKHeightPixels;
        mybmp_struct2.colonne	 =pompa_OKWidthPages;
        mybmp_struct2.start_x=2;
        mybmp_struct2.start_y=42;
        GetBitmap();
        LCD_CopyPartialScreen(2,26,42,64);
        PrintSoglia(SOGLIE_SET_CONC_INDEX,28,40);//come parametro gli basta l'id della soglia da mostrare
        MARK_FULL_SCREEN_CONC;
      }
      
      
      if(CHECK_PRINT_CONC_LIMITS )   //print limiti concentrazione
      {
        CLEAR_PRINT_CONC_LIMITS;
        //pulisco disegno pompa
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END,
                                 WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);
        mybmp_struct2.bmp_pointer=convergenza_bmp;
        mybmp_struct2.righe	  =convergenzaHeightPixels;
        mybmp_struct2.colonne =convergenzaWidthPages;
        mybmp_struct2.start_x=2;
        mybmp_struct2.start_y=42;
        GetBitmap();
        generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
        CalcPrint_Conc_Only[un_misura](generic_ui,28,40);
        //LCDPrintString("Min",2,54);
        generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetConc - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
        CalcPrint_Conc_Only[un_misura](generic_ui,28,54);
        

        LCD_CopyPartialScreen(2,50,40,64);
        MARK_FULL_SCREEN_CONC;
      }
     
    
    
      
      //mettere anche qui un bit
      if(CHECK_PRINT_HEATER)
      {
        CLEAR_PRINT_HEATER;
        CleanArea_Ram_and_Screen(HEATER_E_SETPOINT_AREA_X_START,HEATER_E_SETPOINT_AREA_X_END
                                  ,HEATER_E_SETPOINT_AREA_Y_START,HEATER_E_SETPOINT_AREA_Y_END);//cancello subito disegno HEATER
        mybmp_struct2.bmp_pointer=riscaldatore_bmp;
        mybmp_struct2.righe	 =riscaldatoreHeightPixels;
        mybmp_struct2.colonne =riscaldatoreWidthPages;
        mybmp_struct2.start_x=70;
        mybmp_struct2.start_y=42;
        GetBitmap();
        PrintSoglia(SOGLIE_SET_TEMP_INDEX,96,40);//come parametro gli basta l'id della soglia da mostrare
        //DP LCD_CopyPartialScreen(70,128,40,64);
        LCD_CopyPartialScreen(HEATER_E_SETPOINT_AREA_X_START,HEATER_E_SETPOINT_AREA_X_END
                        ,HEATER_E_SETPOINT_AREA_Y_START,HEATER_E_SETPOINT_AREA_Y_END);
        MARK_FULL_SCREEN_TEMP;
      }
      
      if(CHECK_PRINT_TEMP_LIMITS ) //print limiti temperatura
      {
        CLEAR_PRINT_TEMP_LIMITS;
        CleanArea_Ram_and_Screen(HEATER_E_SETPOINT_AREA_X_START,HEATER_E_SETPOINT_AREA_X_END
                                  ,HEATER_E_SETPOINT_AREA_Y_START,HEATER_E_SETPOINT_AREA_Y_END);//cancello subito disegno HEATER
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(70,128,40,64);//cancello disegno heater
        mybmp_struct2.bmp_pointer=convergenza_bmp;
        mybmp_struct2.righe	  =convergenzaHeightPixels;
        mybmp_struct2.colonne =convergenzaWidthPages;
        mybmp_struct2.start_x=70;
        mybmp_struct2.start_y=42;
        GetBitmap();
        generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
        generic_ui/=10;
        BinToBCDisp(generic_ui,UN_DECIMALE,96,40);
        //LCDPrintString("Min",70,54);
        generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.SetTemp - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp; ;
        generic_ui/=10;
        BinToBCDisp(generic_ui,UN_DECIMALE,96,54);
        
        LCD_CopyPartialScreen(70,128,40,64);
        MARK_FULL_SCREEN_TEMP;
      }
      
     
    
   /* ____ ____ _  _ ____ ____ _  _ ___ ____ ____ ___  _ ____ _  _ ____ 
      |    |  | |\ | |    |___ |\ |  |  |__/ |__|   /  | |  | |\ | |___ 
      |___ |__| | \| |___ |___ | \|  |  |  \ |  |  /__ | |__| | \| |___ */
                                                                  
    
    
    
    
   /*  +-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+ +-+-+-+ +-+-+-+
       |C|o|n|c|e|n|t|r|a|z|i|o|n|e|    |A|l|l|a|r|m|i| |M|i|n| |M|a|x|
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+ +-+-+-+ +-+-+-+*/
    //------------------------------------------------------------------------------------
    if(CHECK_PRINT_CONC_AL_MIN)
    {
      CLEAR_PRINT_CONC_AL_MIN;
      
      if(CHECK_FULL_SCREEN_CONC)CleanArea_Ram_and_Screen(PUMP_E_SETPOINT_AREA_X_START,PUMP_E_SETPOINT_AREA_X_END
                                  ,PUMP_E_SETPOINT_AREA_Y_START,PUMP_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
      else
      {
        CleanArea_Ram_and_Screen(AL_CONC_X_START,AL_CONC_X_END,AL_CONC_Y_START,AL_CONC_Y_END);
        CleanArea_Ram_and_Screen(AL2r_CONC_X_START,AL2r_CONC_X_END,AL2r_CONC_Y_START,AL2r_CONC_Y_END); 
      }
      
      SelectFont(CALIBRI_10);
      LCDPrintString("AL",AL_CONC_X_START,AL_CONC_Y_START);
      LCD_CopyPartialScreen(AL_CONC_X_START,AL_CONC_X_END,AL_CONC_Y_START,AL_CONC_Y_END);

      LCDPrintString("Min",AL2r_CONC_X_START,AL2r_CONC_Y_START);
      CalcPrint_Conc_Only[un_misura](PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc,28,54);
      LCD_CopyPartialScreen(AL2r_CONC_X_START,AL2r_CONC_X_END,AL2r_CONC_Y_START,AL2r_CONC_Y_END);
     
      CLEAR_FULL_SCREEN_CONC;
    }
    //------------------------------------------------------------------------------------
    if(CHECK_PRINT_CONC_AL_MAX)
    {
        CLEAR_PRINT_CONC_AL_MAX; 
        
        if(CHECK_FULL_SCREEN_CONC)CleanArea_Ram_and_Screen(PUMP_E_SETPOINT_AREA_X_START,PUMP_E_SETPOINT_AREA_X_END
                                  ,PUMP_E_SETPOINT_AREA_Y_START,PUMP_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
        else
       {
         CleanArea_Ram_and_Screen(AL_CONC_X_START,AL_CONC_X_END,AL_CONC_Y_START,AL_CONC_Y_END);
         CleanArea_Ram_and_Screen(AL2r_CONC_X_START,AL2r_CONC_X_END,AL2r_CONC_Y_START,AL2r_CONC_Y_END); 
       }
        SelectFont(CALIBRI_10);
        LCDPrintString("AL",AL_CONC_X_START,AL_CONC_Y_START);
        LCD_CopyPartialScreen(AL_CONC_X_START,AL_CONC_X_END,AL_CONC_Y_START,AL_CONC_Y_END);
       
        LCDPrintString("Max",6,54);
        CalcPrint_Conc_Only[un_misura](PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax + PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc,28,54);
        LCD_CopyPartialScreen(2,62,54,64);
        
        CLEAR_FULL_SCREEN_CONC;
    }
    /*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+ +-+-+-+-+
      |C|o|n|c|e|n|t|r|a|z|i|o|n|e| |E|m|p|t|y| |T|a|n|k|
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+ +-+-+-+-+*/
    if(CHECK_PRINT_CONC_ET)
    {
      CLEAR_PRINT_CONC_ET; 
      
      if(CHECK_FULL_SCREEN_CONC)CleanArea_Ram_and_Screen(PUMP_E_SETPOINT_AREA_X_START,PUMP_E_SETPOINT_AREA_X_END
                                  ,PUMP_E_SETPOINT_AREA_Y_START,PUMP_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
      else CleanArea_Ram_and_Screen(ET_X_START,ET_X_END,ET_Y_START,ET_Y_END);
      
      SelectFont(CALIBRI_10);
      LCDPrintString("ET",ET_X_START,ET_Y_START);
      LCD_CopyPartialScreen(ET_X_START,ET_X_END,ET_Y_START,ET_Y_END);
      
      CLEAR_FULL_SCREEN_CONC;
    } 
   /* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+
      |C|o|n|c|e|n|t|r|a|z|i|o|n|e| |T|i|m|e|o|u|t|
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+*/
    if(CHECK_PRINT_CONC_TO)
    {
      CLEAR_PRINT_CONC_TO;
      if(CHECK_FULL_SCREEN_CONC)CleanArea_Ram_and_Screen(PUMP_E_SETPOINT_AREA_X_START,PUMP_E_SETPOINT_AREA_X_END
                                  ,PUMP_E_SETPOINT_AREA_Y_START,PUMP_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
      else CleanArea_Ram_and_Screen(TO_CONC_X_START,TO_CONC_X_END,TO_CONC_Y_START,TO_CONC_Y_END);
      SelectFont(CALIBRI_10);
      LCDPrintString("TO",TO_CONC_X_START,TO_CONC_Y_START);
      LCD_CopyPartialScreen(TO_CONC_X_START,TO_CONC_X_END,TO_CONC_Y_START,TO_CONC_Y_END);
      CLEAR_FULL_SCREEN_CONC;
    }
    
    
    
    /*___ ____ _  _ ___  ____ ____ ____ ___ _  _ ____ ____ 
       |  |___ |\/| |__] |___ |__/ |__|  |  |  | |__/ |__| 
       |  |___ |  | |    |___ |  \ |  |  |  |__| |  \ |  | */
    
    /* +-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+ +-+-+-+ 
       T|e|m|p|e|r|a|t|u|r|a   |A|l|l|a|r|m|i| |M|i|n| |M|a|x|
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+ +-+-+-+    */
    if(CHECK_PRINT_TEMP_AL_MIN)
    {
      CLEAR_PRINT_TEMP_AL_MIN;
      
      if(CHECK_FULL_SCREEN_TEMP)CleanArea_Ram_and_Screen(HEATER_E_SETPOINT_AREA_X_START,HEATER_E_SETPOINT_AREA_X_END
                                  ,HEATER_E_SETPOINT_AREA_Y_START,HEATER_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
      else
      {
        CleanArea_Ram_and_Screen(AL_TEMP_X_START,AL_TEMP_X_END,AL_TEMP_Y_START,AL_TEMP_Y_END);
        CleanArea_Ram_and_Screen(AL2r_TEMP_X_START,AL2r_TEMP_X_END,AL2r_TEMP_Y_START,AL2r_TEMP_Y_END); 
      }
       SelectFont(CALIBRI_10);
       LCDPrintString("AL",AL_TEMP_X_START,AL_TEMP_Y_START);
       LCD_CopyPartialScreen(AL_TEMP_X_START,AL_TEMP_X_END,AL_TEMP_Y_START,AL_TEMP_Y_END);
        //DP END
        
      LCDPrintString("Min",AL2r_TEMP_X_START,AL2r_TEMP_Y_START);
      generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMin - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
      generic_ui/=10;
      BinToBCDisp(generic_ui,UN_DECIMALE,96,54);
      LCD_CopyPartialScreen(AL2r_TEMP_X_START,AL2r_TEMP_X_END,AL2r_TEMP_Y_START,AL2r_TEMP_Y_END);
      
     
     
      CLEAR_FULL_SCREEN_TEMP;
    }
    //------------------------------------------------------------------------------------
    if(CHECK_PRINT_TEMP_AL_MAX)
    {
      CLEAR_PRINT_TEMP_AL_MAX;
      
      if(CHECK_FULL_SCREEN_TEMP)CleanArea_Ram_and_Screen(HEATER_E_SETPOINT_AREA_X_START,HEATER_E_SETPOINT_AREA_X_END
                                  ,HEATER_E_SETPOINT_AREA_Y_START,HEATER_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
      else
      {
        CleanArea_Ram_and_Screen(AL_TEMP_X_START,AL_TEMP_X_END,AL_TEMP_Y_START,AL_TEMP_Y_END);
        CleanArea_Ram_and_Screen(AL2r_TEMP_X_START,AL2r_TEMP_X_END,AL2r_TEMP_Y_START,AL2r_TEMP_Y_END); 
      }
       SelectFont(CALIBRI_10);
       LCDPrintString("AL",AL_TEMP_X_START,AL_TEMP_Y_START);
       LCD_CopyPartialScreen(AL_TEMP_X_START,AL_TEMP_X_END,AL_TEMP_Y_START,AL_TEMP_Y_END);
        //DP END
        
      LCDPrintString("Max",AL2r_TEMP_X_START,AL2r_TEMP_Y_START);
      generic_ui=PROGR_IN_USO.setp_e_soglie.ses_struct.AllTempMax - PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiTemp;
      generic_ui/=10;
      BinToBCDisp(generic_ui,UN_DECIMALE,96,54);
      LCD_CopyPartialScreen(AL2r_TEMP_X_START,AL2r_TEMP_X_END,AL2r_TEMP_Y_START,AL2r_TEMP_Y_END);
      
      CLEAR_FULL_SCREEN_TEMP;
    }
    
    
    /* +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-
      |T|e|m|p|e|r|a|t|u|r|a| |T|i|m|e|o|u|t|
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-*/
    if(CHECK_PRINT_TEMP_TO)
    {
      CLEAR_PRINT_TEMP_TO;
      if(CHECK_FULL_SCREEN_TEMP)CleanArea_Ram_and_Screen(HEATER_E_SETPOINT_AREA_X_START,HEATER_E_SETPOINT_AREA_X_END
                                  ,HEATER_E_SETPOINT_AREA_Y_START,HEATER_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
      else CleanArea_Ram_and_Screen(TO_TEMP_X_START,TO_TEMP_X_END,TO_TEMP_Y_START,TO_TEMP_Y_END);
      SelectFont(CALIBRI_10);
      LCDPrintString("TO",TO_TEMP_X_START,TO_TEMP_Y_START);
      LCD_CopyPartialScreen(TO_TEMP_X_START,TO_TEMP_X_END,TO_TEMP_Y_START,TO_TEMP_Y_END);
      CLEAR_FULL_SCREEN_TEMP;
    }
    
    if(CHECK_CABLE_OPEN_PRINT)
    {
      CLEAR_CABLE_OPEN_PRINT;
      CleanArea_Ram_and_Screen(2,124,34,64);
      CleanRigaVertic(64,0,64);
      LCD_CopyPartialScreen(64,66,0,64);
      SelectFont(CALIBRI_20);
      LCDPrintString(StringsCableOpen [RamSettings.Linguaggio],10,38);
      LCD_CopyPartialScreen(6,124,38,58);
      
    }
    
    
    // LAMPEGGIO SCRITTE E DISEGNI
    if(CHECK_BLINK_TIMER1_EXPIRED)
    {
      CLEAR_BLINK_TIMER1_EXPIRED;
      if(CHECK_BLINK_SHOW)//disegno
      {
        if(CHECK_PUMP_STATE_ATTIVO)     LCD_CopyPartialScreen(PUMP_AREA_X_START,PUMP_AREA_X_END,PUMP_AREA_Y_START,PUMP_AREA_Y_END);
        if(CHECK_TIMEOUT_CONC)          LCD_CopyPartialScreen(TO_CONC_X_START,TO_CONC_X_END,TO_CONC_Y_START,TO_CONC_Y_END);
        if(CHECK_ALARM_TANK)            LCD_CopyPartialScreen(ET_X_START,ET_X_END,ET_Y_START,ET_Y_END);
        if(CHECK_ALARM_CONC_MAX || CHECK_ALARM_CONC_MIN)
        {
          LCD_CopyPartialScreen(AL_CONC_X_START,AL_CONC_X_END,AL_CONC_Y_START,AL_CONC_Y_END);
          LCD_CopyPartialScreen(AL2r_CONC_X_START,AL2r_CONC_X_END,AL2r_CONC_Y_START,AL2r_CONC_Y_END);
        }
        
        
        if(CHECK_HEATER_STATE_ATTIVO)   LCD_CopyPartialScreen(HEATER_AREA_X_START,HEATER_AREA_X_END,HEATER_AREA_Y_START,HEATER_AREA_Y_END);
        if(CHECK_TIMEOUT_TEMP)          LCD_CopyPartialScreen(TO_TEMP_X_START,TO_TEMP_X_END,TO_TEMP_Y_START,TO_TEMP_Y_END);
        if(CHECK_ALARM_TEMP_MAX || CHECK_ALARM_TEMP_MIN)
        {
          LCD_CopyPartialScreen(AL_TEMP_X_START,AL_TEMP_X_END,AL_TEMP_Y_START,AL_TEMP_Y_END);
          LCD_CopyPartialScreen(AL2r_TEMP_X_START,AL2r_TEMP_X_END,AL2r_TEMP_Y_START,AL2r_TEMP_Y_END);
        }
        
         
        CLEAR_BLINK_SHOW;
      }
      else                //cancello
      {
        if(CHECK_PUMP_STATE_ATTIVO)     CleanAreaScreenOnly(PUMP_AREA_X_START,PUMP_AREA_X_END,PUMP_AREA_Y_START,PUMP_AREA_Y_END);//DP
        if(CHECK_TIMEOUT_CONC)          CleanAreaScreenOnly(TO_CONC_X_START,TO_CONC_X_END,TO_CONC_Y_START,TO_CONC_Y_END);
        if(CHECK_ALARM_TANK)            CleanAreaScreenOnly(ET_X_START,ET_X_END,ET_Y_START,ET_Y_END);
        if(CHECK_ALARM_CONC_MAX || CHECK_ALARM_CONC_MIN)
        {
          CleanAreaScreenOnly(AL_CONC_X_START,AL_CONC_X_END,AL_CONC_Y_START,AL_CONC_Y_END);
          CleanAreaScreenOnly(AL2r_CONC_X_START,AL2r_CONC_X_END,AL2r_CONC_Y_START,AL2r_CONC_Y_END);
        }

        
        if(CHECK_HEATER_STATE_ATTIVO)   CleanAreaScreenOnly(HEATER_AREA_X_START,HEATER_AREA_X_END,HEATER_AREA_Y_START,HEATER_AREA_Y_END);
        if(CHECK_TIMEOUT_TEMP)          CleanAreaScreenOnly(TO_TEMP_X_START,TO_TEMP_X_END,TO_TEMP_Y_START,TO_TEMP_Y_END);
        if(CHECK_ALARM_TEMP_MAX || CHECK_ALARM_TEMP_MIN)
        {
          CleanAreaScreenOnly(AL_TEMP_X_START,AL_TEMP_X_END,AL_TEMP_Y_START,AL_TEMP_Y_END);
          CleanAreaScreenOnly(AL2r_TEMP_X_START,AL2r_TEMP_X_END,AL2r_TEMP_Y_START,AL2r_TEMP_Y_END);
        }
        
        
        
        MARK_BLINK_SHOW;
      }
      
      
    }
}



