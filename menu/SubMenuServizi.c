//calibrazione PT100



#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"
#include "meas.h"
#include "eeprom.h"
#include "eepmap.h"

#include <math.h>

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"
#include "Menu.h"
#include "keyboard.h"
#include "VariabiliGlobali_di_Lavoro.h"



extern const char StringsServizio               [4][4][22];
extern const char Strings_InsertCalRes          [4][25];
extern const char Strings_RemovePT100           [4][20];    
extern const char Strings_RemoveProbe           [4][20];
extern const char StringsCableCalibOK           [4][20];
extern const float calib_resist[6];

//***********************************************************************
void Sub2Menu_Cal_PT100(void)
{
  uint8_t key;
  int item,i;
  float res[2],sp;
  
  unsigned char loop_flag=1,stato;

  LCD_Fill_ImageRAM(0x00);
  //stampo nella lingua selezionata "Calibrazione PT100)
  LCDPrintString(StringsServizio[RamSettings.Linguaggio][0],14,2);
  LCDPrintString(Strings_RemovePT100[RamSettings.Linguaggio],14,38);
  LCDPrintString("->PRESS OK",68,54);
  LCD_CopyScreen();
  
  item = CALIB_Ptc;
  i=0;
  stato=0;
  
  while(loop_flag)
  {
          key_getstroke(&key,portMAX_DELAY);
          
          if (key == KEY_PROG)
          {
                  if(stato==8)
                  {
                    MenuFunction_Index=SUB2MENU_CAL_PT100;
                    return;
                  }
                  else
                  {  
                    MenuFunction_Index=MENU_PROGR;
                    return;
                  }  
          }
          
          
          if (key == KEY_OK)
          {
            switch(stato) 
            {
              
                case 0: 
                CleanArea_Ram_and_Screen(14,120,38,52);
                LCDPrintString(Strings_InsertCalRes[RamSettings.Linguaggio],14,26);
                LCDPrintString("150 ohm 0.1%",14,38);
                LCD_CopyPartialScreen(2,120,26,54);
                stato++;
                
                break;
            
            
            
                case 1 :
                if (!domeas(CALIB_Ptc,res+i,(i == 1)? NULL:&sp))
                {
                         CleanArea_Ram_and_Screen(14,120,38,52);
                         LCDPrintString("Error!",2,38);
                         stato=8;
                }
                else//se la misura va a buon fine
                {
                  i++;
                  CleanArea_Ram_and_Screen(14,120,38,52);
                  LCDPrintString(Strings_InsertCalRes[RamSettings.Linguaggio],14,26);
                  LCDPrintString("100 ohm 0.1%",14,38);
                  LCD_CopyPartialScreen(2,120,26,54);
                  
                  stato++;
                } 
                break;
                
                case 2 :
                if (!domeas(CALIB_Ptc,res+i,(i == 1)? NULL:&sp))
                {
                         CleanArea_Ram_and_Screen(14,120,38,52);
                         LCDPrintString("Error!",2,38);
                         stato=8;
                }
                else//se la misura va a buon fine
                {
                  float rh = calib_resist[item+1];
                  float rl = calib_resist[item+2];
                  float nh = res[0];
                  float nl = res[1];
                  float gf = (rh - rl) / (nh - nl);
                  float nos = nh - (rh / gf);
                  /*COM_Printf("Nch=%.3f Ncl=%.3f GF=%.10e Nos=%.10e sp=%.10e\r\n",
                          nh,
                          nl,
                          gf,
                          nos,
                          sp);*/
                  EEP_Write(EEP_PNch_addr,&nh,sizeof(float));
                  EEP_Write(EEP_PNcl_addr,&nl,sizeof(float));
                  EEP_Write(EEP_PGF_addr,&gf,sizeof(float));
                  EEP_Write(EEP_PNos_addr,&nos,sizeof(float));
                  EEP_Write(EEP_PPhase_addr,&sp,sizeof(float));
                  
                  meas_loadParams();
                  
                  CleanArea_Ram_and_Screen(14,128,26,64);
                  //LCDPrintString(Strings_InsertCalRes[RamSettings.Linguaggio],14,26);
                  LCDPrintString(StringsCableCalibOK[RamSettings.Linguaggio],40,26);
                  LCD_CopyPartialScreen(2,128,26,54);
                  
                  stato++;
                } 
                break;
                
                
             }//fine switch(stato)
                
                
              
      
          }
  
           
  }
  
  


}
//***********************************************************************
void Sub2Menu_Cal_Cable(void)
{

  uint8_t key;

  float res[2],sp;
  
  unsigned char loop_flag=1,stato;

  LCD_Fill_ImageRAM(0x00);
  //stampo nella lingua selezionata "Calibrazione PT100)
  LCDPrintString(StringsServizio[RamSettings.Linguaggio][1],14,2);
  LCDPrintString(Strings_RemoveProbe[RamSettings.Linguaggio],14,38);
  LCDPrintString("->PRESS OK",68,54);
  LCD_CopyScreen();
  
  

  stato=0;
  
  while(loop_flag)
  {
          key_getstroke(&key,portMAX_DELAY);
          
          if (key == KEY_PROG)
          {
                  if(stato==8)
                  {
                    MenuFunction_Index=SUB2MENU_CABLE_COMPENS;
                    return;
                  }
                  else
                  {  
                    MenuFunction_Index=MENU_PROGR;
                    return;
                  }  
          }
          
          
          if (key == KEY_OK)
          {
            switch(stato) 
            {
              
                case 0: 
                  CleanArea_Ram_and_Screen(14,120,38,52);
                  LCDPrintString(Strings_InsertCalRes[RamSettings.Linguaggio],14,26);
                  LCDPrintString("100 Kohm 0.1%",14,38);
                  LCD_CopyPartialScreen(2,120,26,54);
                  stato++;
                  loop_flag=1;
                  break;
            
                 
                case 1 :
                    if (!domeas(CALIB_High,res,&sp))
                    {
                            CleanArea_Ram_and_Screen(14,120,38,52);
                            LCDPrintString("Error!",2,38);
                            stato=8;
                    }
                    sp -= mcp[CALIB_High].sp;
                    float gx = ((res[0] - mcp[CALIB_High].nos) * mcp[CALIB_High].gf) * cos(sp);
                    float gc =gx-(1.0 / 100300.0);
                    //COM_Printf("gx=%.10e G(100300Ohm)=%.10e GCable=%.10e\r\n",gx,1.0 / 100300.0,gc);
                    EEP_Write(EEP_GCable_addr,&gc,4);
                    //return CALLBACKRET_Ok;
                     
                    meas_loadParams();
                      
                    CleanArea_Ram_and_Screen(14,128,26,64);
                    //LCDPrintString(Strings_InsertCalRes[RamSettings.Linguaggio],14,26);
                    LCDPrintString(StringsCableCalibOK[RamSettings.Linguaggio],20,26);
                    LCD_CopyPartialScreen(2,128,26,54);
                    break;
               }//fine switch(stato) 
                    
                
                
         }//fine if(key ==KEY_OK)
    }//fine while(loop_flag)
  

}
//***********************************************************************
void Sub2Menu_MisuraDiretta(void)
{
  uint8_t key;

  float t_float;
  
  unsigned char loop_flag=1;

  LCD_Fill_ImageRAM(0x00);
  //stampo nella lingua selezionata "Calibrazione PT100)
  LCDPrintString(StringsServizio[RamSettings.Linguaggio][2],14,2);
  //LCDPrintString("T=",0,14);
  LCDPrintString("R=",62,14);
  //LCDPrintString("G=",0,26);
  LCDPrintString("R=",62,26);

  LCD_CopyScreen();
  
   while(loop_flag)
   {
         
          
          if (key_getstroke(&key,200/*portMAX_DELAY*/) && (key == KEY_PROG))
          {
              MenuFunction_Index=MENU_PROGR;
              return;
          }
          
          //temperatura
          if(measures.temp_ok)
          {
              measures.temp_ok=0;
              CalcPrintTemperaturaXY_10(&t_float,0,14);
              PrintResXY_10(&measures.temp_resist,72,14);
                
          }
          //conduttanza
          if(measures.meas_ok)
          {
              measures.meas_ok=0;
              PrintConduttXY_10(&measures.condut,0,26);
              PrintResXY_10    (&measures.resist,72,26);
                
          }
          
   }
  
}
//***********************************************************************
void Sub2Menu_Licenza(void)
{
  
  
  
  
  
}