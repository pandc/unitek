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
extern  TimerHandle_t AuxTimer; 

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
        //generic_float/=100;
        if(*c_float>generic_float)
        {
          MARK_OVER_CONC_MAX;
          CLEAR_OVER_CONC_MIN;
          CLEAR_ALARM_CONC_MIN;//s eè max non può e non deve essere min
          //CLEAR_TIMER5_EXPIRED;//Timer Max temp..se era già partito sarebbe da fermare
          // non fermo altri timer,tanto marcano solo un flag xTimerStop
          if( xTimerStart( xTimers[ TIMER5_RIT_ALL_MAX_CONC ], 0 ) != pdPASS )
          {}// The timer could not be set into the Active state.  
        }
        else//controllo allarme basso solo se non ho quello alto
        {
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin -
                        (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
          //eneric_float/=100;
          if(*c_float < generic_float)
          {
            MARK_OVER_CONC_MIN;
            CLEAR_OVER_CONC_MAX;//se è min non può e non deve essere max
            CLEAR_ALARM_CONC_MAX;
            if( xTimerStart( xTimers[ TIMER4_RIT_ALL_MIN_CONC ], 0 ) != pdPASS )
            {}// The timer could not be set into the Active state. 
          }
          
        }
      break;


      case(OVER_CONC_MAX)://è già over,guardo se sfora soglie altro lato,o rientra normale
         generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin -
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
         //generic_float/=100; 
         if(*c_float < generic_float)
          {
            MARK_OVER_CONC_MIN;
            CLEAR_OVER_CONC_MAX;
            CLEAR_ALARM_CONC_MAX;
            IOEXP_clr(IOEXP0_MAX_CONC_ALARM);
            if( xTimerStart( xTimers[ TIMER4_RIT_ALL_MIN_CONC ], 0 ) != pdPASS )
            {}// The timer could not be set into the Active state. 
          }
          else
          {
            generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax-
                            (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
            //generic_float/=100;
            if(*c_float<generic_float)
            {
              // DP CleanArea_Ram_and_Screen(2,62,40,64);
              MARK_PRINT_CONC_LIMITS;
              MARK_OVER_CONC_NORMAL;
              CLEAR_ALARM_CONC_MAX;
              
              //DP Cancello segnalazione allarme
              CleanArea_Ram_and_Screen(AL_CONC_X_START,AL_CONC_X_END ,AL_CONC_Y_START, AL_CONC_Y_END);
              CleanArea_Ram_and_Screen(AL2r_CONC_X_START,AL2r_CONC_X_END ,AL2r_CONC_Y_START, AL2r_CONC_Y_END); //2 riga
              //DP END
              
	      IOEXP_clr(IOEXP0_MAX_CONC_ALARM);
            }
         }
       break;


      case(OVER_CONC_MIN)://è già over,guardo se sfora soglie altro lato,o rientra normale
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax +
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
          //generic_float/=100;
          if(*c_float > generic_float)
          {
            MARK_OVER_CONC_MAX;
            CLEAR_OVER_CONC_MIN;//se è min non può e non deve essere max
            CLEAR_ALARM_CONC_MIN;
            IOEXP_clr(IOEXP0_MIN_CONC_ALARM);
            if( xTimerStart( xTimers[ TIMER5_RIT_ALL_MAX_CONC ], 0 ) != pdPASS )
            {}// The timer could not be set into the Active state. 
          }
          else
          {
            generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMin+
                            (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
            //generic_float/=100;
            if(*c_float>generic_float)
            {
              MARK_OVER_CONC_NORMAL;
              CLEAR_ALARM_CONC_MIN;
              MARK_PRINT_CONC_LIMITS;
              
             //DP CleanArea_Ram_and_Screen(2,62,40,64);
             
             MARK_OVER_CONC_NORMAL;
             CLEAR_ALARM_CONC_MIN;
            //DP Cancello segnalazione allarme 
            CleanArea_Ram_and_Screen(AL_CONC_X_START,AL_CONC_X_END ,AL_CONC_Y_START, AL_CONC_Y_END);
            CleanArea_Ram_and_Screen(AL2r_CONC_X_START,AL2r_CONC_X_END ,AL2r_CONC_Y_START, AL2r_CONC_Y_END); //2 riga
            //DP END
            
             IOEXP_clr(IOEXP0_MIN_CONC_ALARM);
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
          CLEAR_ALARM_TEMP_MIN;//se è max non può e non deve essere min
          CLEAR_TIMER9_EXPIRED;//Timer Max temp..se era già partito sarebbe da fermare

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
            //CLEAR_OVER_TEMP_MAX;//se è min non può e non deve essere max
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
            CLEAR_OVER_TEMP_MAX;//se è min non può e non deve essere max
            CLEAR_ALARM_TEMP_MAX;
            IOEXP_clr(IOEXP0_MAX_TEMP_ALARM);
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
              MARK_PRINT_TEMP_LIMITS;
              CLEAR_ALARM_TEMP_MAX;
              CLEAR_OVER_TEMP_MAX;
             //DP CleanArea_Ram_and_Screen(66,126,40,64);
            
             //DP Cancello segnalazione allarme 
            CleanArea_Ram_and_Screen(AL_TEMP_X_START,AL_TEMP_X_END ,AL_TEMP_Y_START, AL_TEMP_Y_END);
            CleanArea_Ram_and_Screen(AL2r_TEMP_X_START,AL2r_TEMP_X_END ,AL2r_TEMP_Y_START, AL2r_TEMP_Y_END); //2 riga
            //DP END
	     
            IOEXP_clr(IOEXP0_MAX_TEMP_ALARM);
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
            
            IOEXP_clr(IOEXP0_MIN_TEMP_ALARM); //DP TEMP ERA CONC
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
              MARK_PRINT_TEMP_LIMITS;
              CLEAR_ALARM_TEMP_MIN;
              
             //DP CleanArea_Ram_and_Screen(66,126,40,64);
            
            //DP Cancello segnalazione allarme 
            CleanArea_Ram_and_Screen(AL_TEMP_X_START,AL_TEMP_X_END ,AL_TEMP_Y_START, AL_TEMP_Y_END);
            CleanArea_Ram_and_Screen(AL2r_TEMP_X_START,AL2r_TEMP_X_END ,AL2r_TEMP_Y_START, AL2r_TEMP_Y_END); //2 riga
            //DP END
              
	     IOEXP_clr(IOEXP0_MIN_TEMP_ALARM); //DP TEMP ERA CONC
            }
       }
      break;
  
   }//fine switch
  
}
//***************************************************************************************
void ControlloTimers(void)
{

    if(CHECK_TIMER1_EXPIRED )//Ritardo accensione conc(T6 analogo per temperatura)
    {
      CLEAR_TIMER1_EXPIRED;
      if(CHECK_ACCENSIONE_CONC)
      {
        CLEAR_ACCENSIONE_CONC;
       //esco da attesa iniziale,qualsiasi sia il valore mi metto in riposo
       //se è troppo basso,al prossimo controllo andrò in attivo
       CLEAR_PUMP_STATES;
       MARK_PUMP_STATE_RIPOSO;
      // MARK_PRINT_CONC_LIMITS;
       //cancella WAIT lato conc
      /* CleanArea_Ram_and_Screen(WAIT_CONC_AREA_X_START,WAIT_CONC_AREA_X_END
                                 ,WAIT_CONC_AREA_Y_START,WAIT_CONC_AREA_Y_END);//pulisco scritta WaitMix Time*/
      }
        //da power ON o casi che devono essere visti come power ON
        if(!CHECK_ATTIVAZIONE_EXT_CH_CONC)MARK_PRINT_CONC_CH_OFF;
        else  
        {
          MARK_PRINT_CONC_LIMITS;


        }
      
    }
    
    if(CHECK_TIMER6_EXPIRED)//Ritardo accensione temp(T1 analogo per concentrazione)
    {
      CLEAR_TIMER6_EXPIRED;
      if(CHECK_ACCENSIONE_TEMP)
      {
       CLEAR_ACCENSIONE_TEMP;
       //esco da attesa iniziale,qualsiasi sia il valore mi metto in riposo
       //se è troppo basso,al prossimo controllo andrò in attivo
       CLEAR_HEATER_STATE_ATTIVO;
       MARK_HEATER_STATE_RIPOSO;
       //MARK_PRINT_TEMP_LIMITS;
       //cancella WAIT lato temp 
       /*CleanArea_Ram_and_Screen(WAIT_TEMP_AREA_X_START,WAIT_TEMP_AREA_X_END
                                 ,WAIT_TEMP_AREA_Y_START,WAIT_TEMP_AREA_Y_END);*/
        if(!CHECK_ATTIVAZIONE_EXT_CH_TEMP)MARK_PRINT_TEMP_CH_OFF;
        else  
        {
          MARK_PRINT_TEMP_LIMITS;
        }
      }
    }
    
    
    
  
  
    //controllo allarme timeout conc(T7 analogo per temp)
    if(CHECK_TIMER2_EXPIRED)//se non ho raggiunto conc in tempo limite blocco ,mettere bit
    {
      CLEAR_TIMER2_EXPIRED;
      if(CHECK_PUMP_STATE_ATTIVO)
      {
        CLEAR_PUMP_STATES;//tanto non entro nemmeno nella funzione di lavoro pompa

        IOEXP_clr(IOEXP0_PUMP_ENABLE);
          
        MARK_TIMEOUT_CONC;
        MARK_PRINT_CONC_TO;
        CLEAR_PRINT_CONC_LIMITS;
        CLEAR_PRINT_PUMP;
      }
    }
     
    
    if(CHECK_TIMER7_EXPIRED)//Time out heater
    {
      CLEAR_TIMER7_EXPIRED;
      if(CHECK_HEATER_STATE_ATTIVO)
      {
        CLEAR_HEATER_STATES;//tanto non entro nemmeno nella funzione di lavoro heater
        IOEXP_clr(IOEXP0_HEATER_ENABLE);

        MARK_TIMEOUT_TEMP;
        MARK_PRINT_TEMP_TO;
        CLEAR_PRINT_TEMP_LIMITS;
        CLEAR_PRINT_HEATER;

      }
    }
    
    if(CHECK_TIMER3_EXPIRED)//ritardo dosaggio
    {
      CLEAR_TIMER3_EXPIRED;
      //se ho allarmi lascio scadere il timer e non faccio altro
      if((!CHECK_TIMEOUT_CONC)  && (!CHECK_ALARM_TANK))
      {
        if(CHECK_PUMP_STATE_WAIT)
        {
          CLEAR_PUMP_STATES;
          MARK_PUMP_STATE_ATTIVO;
          

          MARK_PRINT_PUMP;
          //MARK_PRINT_CONC_LIMITS;
          
          //prima disegnava pompa e setpoint qui
          if( xTimerStart( xTimers[ TIMER2_TOUT_DOSAGGIO ], 0 ) != pdPASS ){}
          //CLEAR_PRINT_PUMP;
          //
	  IOEXP_set(IOEXP0_PUMP_ENABLE);//>>>>>>>>>>>>>Enable_Pump();
        }
      }
    }
  
  
    //controllo allarmi min e max concentrazione
    
    if(CHECK_TIMER4_EXPIRED)//ritardo allarme min concentrazione(T8 analogo per temperatura)
    {
      CLEAR_TIMER4_EXPIRED;
      if(CHECK_OVER_CONC_MIN)
      {
        IOEXP_set(IOEXP0_MIN_CONC_ALARM);
        MARK_PRINT_CONC_AL_MIN;
        MARK_ALARM_CONC_MIN;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme

      }
    }
    
    
    if(CHECK_TIMER8_EXPIRED)//ritardo allarme min temperatura(T4 analogo per mi temp)
    {
      CLEAR_TIMER8_EXPIRED;
      
      if(CHECK_OVER_TEMP_MIN)
      {
        //xTimerStop( xTimers[TIMER7_TOUT_TEMP], 0 );
        IOEXP_set(IOEXP0_MIN_TEMP_ALARM);
        MARK_ALARM_TEMP_MIN;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        MARK_PRINT_TEMP_AL_MIN;
      }
    }
    
    

    if(CHECK_TIMER5_EXPIRED)//ritardo allarme max concentrazione(T9 analogo per temperatura)
    {
      CLEAR_TIMER5_EXPIRED;
      if(CHECK_OVER_CONC_MAX)
      {
        if(CHECK_PUMP_STATE_ATTIVO)
        {  
          IOEXP_clr(IOEXP0_PUMP_ENABLE);
          CLEAR_PUMP_STATE_ATTIVO;
        } 
        MARK_PRINT_CONC_AL_MAX;
        MARK_ALARM_CONC_MAX;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        IOEXP_set(IOEXP0_MAX_CONC_ALARM);
       }
    }
    
    
    if(CHECK_TIMER9_EXPIRED)//ritardo allarme max temperatura(T5 analogo per concentrazione)
    {
      CLEAR_TIMER9_EXPIRED;
      if(CHECK_OVER_TEMP_MAX)
      {
        if(CHECK_HEATER_STATE_ATTIVO)
        {  
         IOEXP_clr(IOEXP0_HEATER_ENABLE);
          CLEAR_HEATER_STATE_ATTIVO;
        }
        MARK_PRINT_TEMP_AL_MAX;
        MARK_ALARM_TEMP_MAX;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        IOEXP_set(IOEXP0_MAX_TEMP_ALARM);
      }
    }
 
}
//***************************************************************************************
void StopAllTimers(void)
{
  int32_t x;
  
  for( x = 0; x < NUM_TIMERS; x++ )
  {
    xTimerStop( xTimers[x], 0 );
  }
   xTimerStop( AuxTimer, 0 );
}
//***************************************************************************************
void Stop_ChConc_Timers(void)
{
  int32_t x;

#define TIMERS_CH_CONC_INDEX 5//i 5 timers da 1 a 5(0-4)sono per la concentrazione,uno in + di temp x ritardo dosaggio
  
  for( x = 0; x < TIMERS_CH_CONC_INDEX; x++ )
  {
    xTimerStop( xTimers[x], 0 );
  }
}
//***************************************************************************************
void Stop_ChTemp_Timers(void)
{
  int32_t x;

#define TIMERS_CH_CONC_INDEX 5//i 4 timers da 5 a 9(5-8)sono per la temperatura
  
  for( x = TIMERS_CH_CONC_INDEX; x < 9; x++ )
  {
    xTimerStop( xTimers[x], 0 );
  }
}
//***************************************************************************************