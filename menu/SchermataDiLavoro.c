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
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;


extern  TimerHandle_t xTimers[ NUM_TIMERS ];
extern  TimerHandle_t AuxTimer; 
//***************************************************************************************
void SchermataDiLavoro(void)
{
    uint8_t key,input_status;	//,test;
    float c_float,t_float;
   // float generic_float;
    
   SchermLav_ScritteFisse();
   if( xTimerStart( AuxTimer, 0 ) != pdPASS ){}//timer blink
    
    
   SchermLavoroInitCondition();
    
    
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
              IOEXP_clr(0xff);		// clear all ioexp outputs
              MenuFunction_Index=MENU_PROGR;
              xTimerStop( AuxTimer, 0 );
              return;
            }
            
             
            /*+-+-+-+-+ +-+-+-+-+-+-+-+
              |C|a|v|o| |a|p|e|r|t|o|?|
              +-+-+-+-+ +-+-+-+-+-+-+-+  */
            if(!CHECK_CABLE_OPEN)//solo se il cavo è ok controllo gli interruttori e il sensore tank
            {  
              input_status=IOEXP_get();
              
                if(RamSettings.abilita_disabilita==ABILITA)
                {
                  
                 /*  +-+-+-+-+-+-+ +-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                    |S|w|i|t|c|h| |e|s|t|e|r|n|o| |c|o|n|c|e|n|t|r|a|z|i|o|n|e|
                    +-+-+-+-+-+-+ +-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
                  if(input_status  & DISABIL_CONC_EXT)//controllo input,se interrutore è in OFF
                  {  //se arrivo da stato OFF dovrei avere già fatto tutto,trane la stampa,se arrivo da fuori
                     if(CHECK_ATTIVAZIONE_EXT_CH_CONC)//se ero in stato ON
                     {
                       MARK_PRINT_CONC_CH_OFF;// se arrivo da OFF ho già stampato,ma se arrivo da fuori ho già i flags che mi dicono di stampare
                       CLEAR_ATTIVAZIONE_EXT_CH_CONC;
                       ResetChConc_OFF();
                     }
                  }
                  else  //se interruttore mi dà OK
                  {
              
                      if(!CHECK_ATTIVAZIONE_EXT_CH_CONC)//se ero disattivato
                      {
                        MARK_ATTIVAZIONE_EXT_CH_CONC;
                        ResetChConc_ON();
                      }
                       
                      
                      if(!CHECK_ACCENSIONE_CONC)
                      {  
                         /*+-+-+-+-+ +-+-+-+-+-+
                          |T|a|n|k| |v|u|o|t|o|
                          +-+-+-+-+ +-+-+-+-+-+*/
                           if(input_status  & TANK_ALARM_INPUT)
                           {
                              if(!(CHECK_ALARM_TANK))//controllo solo se non sono già in allarme tank
                              {
                              
                                MARK_ALARM_TANK;
                                IOEXP_set(IOEXP0_TANK_ALARM);
                                
                                if(CHECK_PUMP_STATE)
                                {  
                                  
                                  IOEXP_clr(IOEXP0_PUMP_ENABLE);
                                  CLEAR_PUMP_STATES;//indica che la pompa non deve essere presa in considerazione
                                 } 
                                MARK_PRINT_CONC_ET;
                              }
                              
                            }
                            else
                            {
                              if(CHECK_ALARM_TANK)//controllo solo se sono già in allarme tank
                              {
                                MARK_CONTROL_CONC_ENA;
                                IOEXP_clr(IOEXP0_TANK_ALARM);
                                CleanArea_Ram_and_Screen(ET_X_START,ET_X_END,ET_Y_START,ET_Y_END);
                                CLEAR_ALARM_TANK;
                                MARK_PUMP_STATE_RIPOSO;//devo dargli uno dei 3 stati possibili e poi da questo,alla prima lettura ricontrollerà cosa fare
                                MARK_PRINT_CONC_LIMITS;
                              }
                            }  //fine controllo TANK
                          
                      } //fine if(!CHECK_ACCENSIONE_CONC)  
                          
                   }
                   
              
              
                  /* +-+-+-+-+-+-+ +-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+
                     |S|w|i|t|c|h| |e|s|t|e|r|n|o| |t|e|m|p|e|r|a|t|u|r|a|
                     +-+-+-+-+-+-+ +-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+  */
                                
                  if(input_status  & DISABIL_TEMP_EXT)
                  {
                     if(CHECK_ATTIVAZIONE_EXT_CH_TEMP)//se ero in stato ON
                     {
                       MARK_PRINT_TEMP_CH_OFF;// se arrivo da OFF ho già stampato,ma se arrivo da fuori ho già i flags che mi dicono di stampare
                       CLEAR_ATTIVAZIONE_EXT_CH_TEMP;
                       ResetChTemp_OFF();
                     }
                  }
                  else
                  {
                    
                    if(!CHECK_ATTIVAZIONE_EXT_CH_TEMP)
                      {
                        
                        MARK_ATTIVAZIONE_EXT_CH_TEMP;
                        ResetChTemp_ON();
                      }
                   }
                  
                }//fine if(RamSettings.abilita_disabilita==DISABILITA)   
            }//fine   if(!CHECK_CABLE_OPEN)   
            
            
            
           /*+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             |L|E|T|T|U|R|A| |T|E|M|P|E|R|A|T|U|R|A| |E| |C|O|N|C|E|N|T|R|A|Z|I|O|N|E|
             +-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+    */
            if(measures.temp_ok)
            {
                if(measures.temp_resist>LIMITE_CAVO_APERTO_TEMP  || measures.temp_resist < 0 )
                {
                  if(!CHECK_CABLE_OPEN)
                  {  
                    MARK_CABLE_OPEN;
                    MARK_CABLE_OPEN_PRINT;
                    ResetChConc_OFF();
                    ResetChTemp_OFF();
                  }  
                }
                else 
                {
                 if(CHECK_CABLE_OPEN)
                  { 
                    CLEAR_CABLE_OPEN;
                    CLEAR_CABLE_OPEN_PRINT;
                    ResetChConc_ON();
                    ResetChTemp_ON();
                    CleanArea_Ram_and_Screen(62,66,10,64);
                    RigaVertic(64,0,64);
                    LCD_CopyPartialScreen(64,66,0,64);
                  }
                }  
                CalcPrintTemperatura(&t_float);
                measures.temp_ok=0;
                MARK_CONTROL_TEMP_ENA;
                
            }
            
             if(measures.meas_ok)
             {  
           /*+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+
             |C|O|M|P|E|N|S|A|Z|I|O|N|E| |T|K|
             +-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+  */    
               c_float=CompensConduc_TK(&measures.conduc);
           /*+-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             |C|A|L|C|O|L|O| |E| |S|T|A|M|P|A| |C|O|N|C|E|N|T|R|A|Z|I|O|N|E|
             +-+-+-+-+-+-+-+ +-+ +-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+ */
               measures.meas_ok=0;
               c_float=CalcoloConcent_Now(c_float);
               PrintConc_WorkMenu(); 
               MARK_CONTROL_CONC_ENA;
            }
           
            
            
            
            //anche se ho le misure completate,devo evitare di usarne i valori per fare azioni se sono in Wait,in OFF,in CABLE_FAULT
            if(CHECK_ACCENSIONE_CONC ||(!CHECK_ATTIVAZIONE_EXT_CH_CONC) || CHECK_CABLE_OPEN) CLEAR_CONTROL_CONC_ENA;
            if(CHECK_ACCENSIONE_TEMP ||(!CHECK_ATTIVAZIONE_EXT_CH_TEMP) || CHECK_CABLE_OPEN) CLEAR_CONTROL_TEMP_ENA;
            //anche se ho le misure completate,devo evitare di usarne i valori per fare azioni se sono in  DISABILITA          
            if(RamSettings.abilita_disabilita==DISABILITA)
            {
              CLEAR_CONTROL_CONC_ENA;
              CLEAR_CONTROL_TEMP_ENA;
            }
            
            
            
            if( CHECK_CONTROL_CONC_ENA) ControlloSoglieAllarmi_Conc(&c_float);
            if( CHECK_CONTROL_TEMP_ENA) ControlloSoglieAllarmi_Temp(&t_float); 
                     
               
            if(RamSettings.abilita_disabilita==ABILITA)
            {  
                  //**************** INTERVENTO!!!********************************************
                  //confronto con setpoint,isteresi ecc
                if(CHECK_CONTROL_CONC_ENA)ConcPump_AtWork   (&c_float);
                if(CHECK_CONTROL_TEMP_ENA)TempHeater_AtWork (&t_float);
            }
            
            ControlloTimers();
            
            CheckPrint_SchermataDiLavoro();

    }//fine while(1) loop di lavoro
}

//***************************************************************************************
void ConcPump_AtWork(float * c_float)
{
  float generic_float;
  if(CHECK_TIMEOUT_CONC)return;
  
 
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
                  CleanArea_Ram_and_Screen(PUMP_E_SETPOINT_AREA_X_START,PUMP_E_SETPOINT_AREA_X_END
                                          ,PUMP_E_SETPOINT_AREA_Y_START,PUMP_E_SETPOINT_AREA_Y_END);//cancello subito disegno pompa
		  IOEXP_clr(IOEXP0_PUMP_ENABLE);   
                  
                  
                  //>>>>>>>>>>>>>Disable_Pump();    
              }
              else
              {
                     // if(RamSettings.abilita_disabilita==ABILITA)CleanArea_Ram_and_Screen(30,58,42,54);
              }

              break;
            
          case PUMP_STATE_WAIT ://NB in pump state WAIT ci vado solo alla prox misura ,anche se lo stato l'ho segnato prima
              
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
		//IOEXP_clr(IOEXP0_PUMP_ENABLE);
              }
              //>>>>>>>>>>>>>Disable_Pump();    
           
          
                break;
             default:
                    break;
             }  
          
          }

    
}
//***************************************************************************************
void TempHeater_AtWork(float * t_float)
{
  float generic_float;
  
  if(CHECK_TIMEOUT_TEMP)return;
  
     
      CLEAR_CONTROL_TEMP_ENA;
      
      //lo stato in cui vado dipende dalla temperatura e dallo stato precedente
     //mettere if allarme non fare niente
      if(!(CHECK_TEMP_ALARMS_MASK))//non la pulisco qua,la controllo solamente
      {  
          switch(CHECK_HEATER_STATES)
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
                   
                   //DP CleanArea_Ram_and_Screen(66,128,40,64);
                   //prima disegnava heater direttamente qui
                   //DP END
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
                 //DP
                  
                  IOEXP_clr(IOEXP0_HEATER_ENABLE);
                 //DP END           
                //DP CleanArea_Ram_and_Screen(70,128,40,64);//cancello subito disegno heater
                //DP IOEXP_clr(IOEXP0_HEATER_ENABLE);
                //>>>>>>>>>>>>>Disable_Pump();    
              }
              else
              {
                      //if(RamSettings.abilita_disabilita==ABILITA)CleanArea_Ram_and_Screen(30,58,42,54);
              }
            }
            break;
            
          
          
            default:
                
              break;
          
          
             }  
          
          }

}



