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

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;
extern unsigned char screen_image[1024];
extern const char StringsSubmenuSimboliConc   	    [5][4];
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;


extern  TimerHandle_t xTimers[ NUM_TIMERS ];

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
          CLEAR_ALARM_CONC_MIN;//s eè max non può e non deve essere min
          CLEAR_TIMER5_EXPIRED;//Timer Max temp..se era già partito sarebbe da fermare
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
            CLEAR_OVER_CONC_MAX;//se è min non può e non deve essere max
            CLEAR_ALARM_CONC_MAX;
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
              CleanArea_Ram_and_Screen(2,62,42,64);
              MARK_OVER_CONC_NORMAL;
              CLEAR_ALARM_CONC_MAX;
              CLEAR_OUT_MAX_CONC_ALARM;
              I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
            }
         }
       break;


      case(OVER_CONC_MIN)://è già over,guardo se sfora soglie altro lato,o rientra normale
          generic_float=(float)PROGR_IN_USO.setp_e_soglie.ses_struct.AllConcMax +
                              (float)PROGR_IN_USO.setp_e_soglie.ses_struct.IsteresiConc;
          generic_float/=100;
          if(*c_float > generic_float)
          {
            MARK_OVER_CONC_MIN;
            CLEAR_OVER_CONC_MAX;//se è min non può e non deve essere max
            CLEAR_ALARM_CONC_MAX;
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
              CLEAR_ALARM_CONC_MIN;
              
             CleanArea_Ram_and_Screen(2,62,42,64);
             MARK_OVER_CONC_NORMAL;
             CLEAR_ALARM_CONC_MIN;
             CLEAR_OUT_MIN_CONC_ALARM;
             I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
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
              CLEAR_OVER_TEMP_MAX;
              CleanArea_Ram_and_Screen(66,126,42,64);

              CLEAR_OUT_MAX_TEMP_ALARM;
              I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
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
              
             CleanArea_Ram_and_Screen(66,126,42,64);

             CLEAR_OUT_MIN_CONC_ALARM;
             I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
            }
       }
      break;

     
   }//fine switch
  
}
//***************************************************************************************
void ControlloRitardi(void)
{

    if(CHECK_TIMER1_EXPIRED)//Ritardo accensione conc(T6 analogo per temperatura)
    {
      CLEAR_TIMER1_EXPIRED;
      if(CHECK_ACCENSIONE_CONC)
      {
        CLEAR_ACCENSIONE_CONC;
       //esco da attesa iniziale,qualsiasi sia il valore mi metto in riposo
       //se è troppo basso,al prossimo controllo andrò in attivo
       CLEAR_PUMP_STATES;
       MARK_PUMP_STATE_RIPOSO;
       MARK_PRINT_CONC_LIMITS;
       
       //cancella WAIT lato conc
       CleanArea_Ram_and_Screen(2,62,38,58);
       
       
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
       MARK_PRINT_TEMP_LIMITS;
       
       //cancella WAIT lato temp 
       CleanArea_Ram_and_Screen(66,126,38,58);
      }
    }
    
    
    
  
  
    //controllo allarme timeout conc(T7 analogo per temp)
    if(CHECK_TIMER2_EXPIRED)//se non ho raggiunto conc in tempo limite blocco ,mettere bit
    {
      CLEAR_TIMER2_EXPIRED;
      if(CHECK_PUMP_STATE_ATTIVO)
      {
        CLEAR_PUMP_STATES;
        MARK_PUMP_STATE_WAIT;//tanto non entro nemmeno nella funzione di lavoro pompa
        //dovrei restare in wait perchè nessun timer(TIMER3) mi fa uscire
        //>>>>>>>>>>>>>Disable_Pump();
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(2,64,42,64);//cancella pompa e limiti
        LCDPrintString("TimeOut",4,42);
        LCD_CopyPartialScreen(2,64,42,58);
            
        MARK_TIMEOUT_CONC;
        CLEAR_PRINT_CONC_LIMITS;
        CLEAR_PRINT_PUMP;
        CLEAR_OUT_PUMP_ENABLE;
        I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
      }
    }
  
    if(CHECK_TIMER7_EXPIRED)//Time out heater
    {
      CLEAR_TIMER7_EXPIRED;
      if(CHECK_HEATER_STATE_ATTIVO)
      {
        MARK_HEATER_STATE_RIPOSO;//tanto non entro nemmeno nella funzione di lavoro pompa
        
        //>>>>>>>>>>>>>Disable_Heather();
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(66,128,42,64);//cancella pompa e limiti
        LCDPrintString("TimeOut",68,42);
        LCD_CopyPartialScreen(66,128,42,58);
        //MARK_PRINT_TIMOUT;
        
        MARK_TIMEOUT_TEMP;
        CLEAR_PRINT_TEMP_LIMITS;
        CLEAR_PRINT_HEATER;
        CLEAR_OUT_HEATER_ENABLE;
        I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
      }
    }
    
    if(CHECK_TIMER3_EXPIRED)//ritardo dosaggio
    {
      CLEAR_TIMER3_EXPIRED;
      //se ho allarmi lascio scadere il timer e non faccio altro
      if((!CHECK_CONC_ALARMS_MASK)  && (!CHECK_ALARM_FUSTO_VUOTO))
      {
        if(CHECK_PUMP_STATE_WAIT)
        {
          CLEAR_PUMP_STATES;
          MARK_PUMP_STATE_ATTIVO;
          
          //CleanArea_Ram_and_Screen(2,60,32,42);//pulisco scritta WaitMix Time
          CleanArea_Ram_and_Screen(2,62,42,64);
          mybmp_struct2.bmp_pointer=pompa_OK_bmp;
          mybmp_struct2.righe	 =pompa_OKHeightPixels;
          mybmp_struct2.colonne	 =pompa_OKWidthPages;
          mybmp_struct2.start_x=2;
          mybmp_struct2.start_y=42;
          GetBitmap();
          LCD_CopyPartialScreen(2,26,42,64);
          
          
          if( xTimerStart( xTimers[ TIMER2_TOUT_DOSAGGIO ], 0 ) != pdPASS ){}
          MARK_PRINT_PUMP;
          CLEAR_PRINT_CONC_LIMITS;
          MARK_OUT_PUMP_ENABLE;//>>>>>>>>>>>>>Enable_Pump();
          I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
        }
      }
    }
  
  
    //controllo allarmi min e max concentrazione
    
    if(CHECK_TIMER4_EXPIRED)//ritardo allarme min concentrazione(T8 analogo per temperatura)
    {
      CLEAR_TIMER4_EXPIRED;
      if(CHECK_OVER_CONC_MIN)
      {
       // CLEAR_CONTROL_CONC_ENA ;
        xTimerStop( xTimers[TIMER2_TOUT_DOSAGGIO], 0 );
        //cancello metà sx e scrivo(da decidere bene)
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(2,62,42,64);
        LCDPrintString("Alarm Min",2,42);
        LCD_CopyPartialScreen(2,62,42,64);
        
        MARK_OUT_MIN_CONC_ALARM;
        CLEAR_OUT_PUMP_ENABLE;
        I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
        MARK_ALARM_CONC_MIN;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        //SPEGNI POMPA
        //DISABILITA FUNZ LAV CONCENTRAZIONE
      }
    }
    
    
    if(CHECK_TIMER8_EXPIRED)//ritardo allarme min temperatura(T4 analogo per mi temp)
    {
      if(CHECK_OVER_TEMP_MIN)
      {
        xTimerStop( xTimers[TIMER7_TOUT_TEMP], 0 );
        //CLEAR_CONTROL_TEMP_ENA ;
        SelectFont(CALIBRI_10);

        CleanArea_Ram_and_Screen(66,126,42,64);
        LCDPrintString("Alarm Min",66,42);
        LCD_CopyPartialScreen(66,126,42,64);
        
        
        MARK_OUT_MIN_TEMP_ALARM;
        CLEAR_OUT_HEATER_ENABLE;
        I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1); 
        MARK_ALARM_TEMP_MIN;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        //SPEGNI HEATER
        //DISABILITA FUNZ LAV TEMPERATURA
      }
      CLEAR_TIMER8_EXPIRED;
    }
    
    

    if(CHECK_TIMER5_EXPIRED)//ritardo allarme max concentrazione(T9 analogo per temperatura)
    {
      CLEAR_TIMER5_EXPIRED;
      if(CHECK_OVER_CONC_MAX)
      {
        //cancello metà sx e scrivo(da decidere bene)
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(2,62,42,64);
        LCDPrintString("Alarm Max",2,42);
        LCD_CopyPartialScreen(2,62,42,64);
        
        
        MARK_ALARM_CONC_MAX;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        MARK_OUT_MAX_CONC_ALARM;
        CLEAR_OUT_PUMP_ENABLE;
        I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
        //SPEGNI POMPA
        //DISABILITA FUNZ LAV CONCENTRAZIONE
      }
    }
    
    
    if(CHECK_TIMER9_EXPIRED)//ritardo allarme max concentrazione(T5 analogo per concentrazione)
    {
     if(CHECK_OVER_TEMP_MAX)
      {
        //cancello metà sx e scrivo(da decidere bene)
        SelectFont(CALIBRI_10);
        CleanArea_Ram_and_Screen(66,126,42,64);
        LCDPrintString("Alarm Max",66,42);
        LCD_CopyPartialScreen(66,126,42,64);
        
        
        MARK_ALARM_TEMP_MAX;//se quando il timer di preallarme è scaduto la condizione è confermata allora marca allarme
        MARK_OUT_MAX_TEMP_ALARM;
        CLEAR_OUT_HEATER_ENABLE;
        I2C_RandWrite(0x20,0x01,1,&immagine_stato_uscite,1);
        //SPEGNI HEATER
        //DISABILITA FUNZ LAV TEMPERATURA
      }
      CLEAR_TIMER9_EXPIRED;
      
    }
 
}
//***************************************************************************************
void StopAllTimers(void)
{
  int32_t x;


  // Create then start some timers.  Starting the timers before the scheduler
  // has been started means the timers will start running immediately that
  // the scheduler starts.
  for( x = 0; x < NUM_TIMERS; x++ )
  {
    xTimerStop( xTimers[x], 0 );
  }
  
}
//***************************************************************************************