/*
*/

#include "bsp.h"
#include "io.h"
#include "stdio.h"
#include "string.h"
#include "timers.h"
#include "meas.h"
/*
#include "freertos.h"
#include "task.h"

#include "meas.h"*/
#include "ffile.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"

#include "VariabiliGlobali_di_Lavoro.h"
#include "keyboard.h"
#include "menu.h"
#include "my_types.h"

//extern print_conc_var struct_conc_print;
extern const char StringsSubmenuSimboliConc   	    [5][4];
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;
extern unsigned char width_font;


extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

extern  TimerHandle_t xTimers[ NUM_TIMERS ];
//extern int32_t lExpireCounters[ NUM_TIMERS ] ;
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
//****************************************************************************************************************************************************


/* _____      _      _____      _       _                                                __            _          __  ___    
  / ____|    | |    |  __ \    (_)     | |                                              / _|          | |        /_ |/ _ \   
 | |     __ _| | ___| |__) | __ _ _ __ | |_     ___ ___  _ __   ___    __  __  _   _   | |_ ___  _ __ | |_ ___    | | | | |  
 | |    / _` | |/ __|  ___/ '__| | '_ \| __|   / __/ _ \| '_ \ / __|   \ \/ / | | | |  |  _/ _ \| '_ \| __/ __|   | | | | |  
 | |___| (_| | | (__| |   | |  | | | | | |_   | (_| (_) | | | | (__     >  <  | |_| |  | || (_) | | | | |_\__ \   | | |_| |  
  \_____\__,_|_|\___|_|   |_|  |_|_| |_|\__|   \___\___/|_| |_|\___|   /_/\_\  \__, |  |_| \___/|_| |_|\__|___/   |_|\___/   
                                                                                __/ |                                        
                                                                               |___/   */   

#define LARGH_CON_UNITA_MISURA  44
#define START_UNITA_MISURA  26
//****************************************************************************************************************************************************
void CalcPrint_Percent_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
     Formula_ConcConvers_Percent(bin);
  
     SelectFont(CALIBRI_10);
     
     CleanArea_Ram_and_Screen(x,x+LARGH_CON_UNITA_MISURA,y,y+10);
     BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
     LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_PERCENTUALE],x+START_UNITA_MISURA,y);
     LCD_CopyPartialScreen(x,x+LARGH_CON_UNITA_MISURA,y,y+12);
 
}
//****************************************************************************************************************************************************
void CalcPrint_PuntTitol_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
      Formula_ConcConvers_PuntTitol(bin);
   
      SelectFont(CALIBRI_10);
     
      CleanArea_Ram_and_Screen(x,x+LARGH_CON_UNITA_MISURA,y,y+10);
      BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
      LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_PUNT_TITOL],x+START_UNITA_MISURA,y);
      LCD_CopyPartialScreen(x,x+LARGH_CON_UNITA_MISURA,y,y+12);
}
//****************************************************************************************************************************************************
void CalcPrint_GrammiLitro_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
      Formula_ConcConvers_grammiLitro(bin);
  
      SelectFont(CALIBRI_10);
       
      CleanArea_Ram_and_Screen(x,x+LARGH_CON_UNITA_MISURA,y,y+10);
      BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
      LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_GRAMMILITRO],x+START_UNITA_MISURA,y);
      LCD_CopyPartialScreen(x,x+LARGH_CON_UNITA_MISURA,y,y+12);

}
//****************************************************************************************************************************************************
void CalcPrint_uSiemens_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
    Formula_ConcConvers_uSiemens(bin);  
    SelectFont(CALIBRI_10);
   
    CleanArea_Ram_and_Screen(x,x+LARGH_CON_UNITA_MISURA,y,y+10);
    BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
    LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_uSIEMENS],x+START_UNITA_MISURA,y);
    LCD_CopyPartialScreen(x,x+LARGH_CON_UNITA_MISURA,y,y+12);
}
//****************************************************************************************************************************************************
void CalcPrint_milliSiemens_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
    Formula_ConcConvers_milliSiemens(bin);
  
    SelectFont(CALIBRI_10);
   
    CleanArea_Ram_and_Screen(x,x+LARGH_CON_UNITA_MISURA,y,y+10);
    BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
    LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_mSIEMENS],x+START_UNITA_MISURA,y);
    LCD_CopyPartialScreen(x,x+LARGH_CON_UNITA_MISURA,y,y+12);
}
//





//****************************************************************************************************************************************************
void CalcPrintOnly_Percent_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
     Formula_ConcConvers_Percent(bin);
  
     SelectFont(CALIBRI_10);
     
     CleanArea_Ram_and_Screen(x,x+28,y,y+10);
     BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
     //LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_PERCENTUALE],x+START_UNITA_MISURA,y);
     LCD_CopyPartialScreen(x,x+28,y,y+12);
 
}
//****************************************************************************************************************************************************
void CalcPrintOnly_PuntTitol_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
      Formula_ConcConvers_PuntTitol(bin);
   
      SelectFont(CALIBRI_10);
     
      CleanArea_Ram_and_Screen(x,x+28,y,y+10);
      BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
      //LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_PUNT_TITOL],x+START_UNITA_MISURA,y);
      LCD_CopyPartialScreen(x,x+24,y,y+12);
}
//****************************************************************************************************************************************************
void CalcPrintOnly_GrammiLitro_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
      Formula_ConcConvers_grammiLitro(bin);
  
      SelectFont(CALIBRI_10);
       
      CleanArea_Ram_and_Screen(x,x+28,y,y+10);
      BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
      //LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_GRAMMILITRO],x+START_UNITA_MISURA,y);
      LCD_CopyPartialScreen(x,x+28,y,y+12);

}
//****************************************************************************************************************************************************
void CalcPrintOnly_uSiemens_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
    Formula_ConcConvers_uSiemens(bin);  
    SelectFont(CALIBRI_10);
   
    CleanArea_Ram_and_Screen(x,x+28,y,y+10);
    BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
    //LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_uSIEMENS],x+START_UNITA_MISURA,y);
    LCD_CopyPartialScreen(x,x+28,y,y+12);
}
//****************************************************************************************************************************************************
void CalcPrintOnly_milliSiemens_xy(unsigned int  bin,unsigned int x,unsigned int y)
{
    Formula_ConcConvers_milliSiemens(bin);
  
    SelectFont(CALIBRI_10);
   
    CleanArea_Ram_and_Screen(x,x+28,y,y+10);
    BinToBCDisp(struct_conc_print.conc_to_print,struct_conc_print.decimali_to_print,x,y);
    //LCDPrintString(StringsSubmenuSimboliConc[UNIT_MIS_CONCENTR_mSIEMENS],x+START_UNITA_MISURA,y);
    LCD_CopyPartialScreen(x,x+28,y,y+12);
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
void PrintConc_WorkMenu(void)
{
  char string_to_print[16];
  unsigned int len;
   
   switch (struct_conc_print.decimali_to_print)
   {
     case INTERO:
        sprintf(string_to_print,"%.0f",struct_conc_print.fconc_to_print);//pot=206,5 ohm
        break;
                          
      case UN_DECIMALE:
        sprintf(string_to_print,"%.1f",struct_conc_print.fconc_to_print);//pot=206,5 ohm
      break;
      
      case DUE_DECIMALI:
        sprintf(string_to_print,"%.2f",struct_conc_print.fconc_to_print);//pot=206,5 ohm
      break;
      
      default:
      break;
    }
   CleanArea_Ram_and_Screen(00,62,14,36);
   len=strlen(string_to_print);
   LCDPrintString(string_to_print,62-(width_font*len)-5,14);
   LCD_CopyPartialScreen(00,62,14,36);

  
}


//
//***************************************************************************************************************************************
void DecrParamConc(unsigned int* par_pt,unsigned short decr)//il valore da stampare si troverà nella struttura struct_conc_to_print
{


  switch(PROGR_IN_USO.unita_mis_concentr)
  {
    case UNIT_MIS_CONCENTR_PERCENTUALE:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_Percent(*par_pt);
            if(!(struct_conc_print.conc_to_print<decr))struct_conc_print.conc_to_print-=decr;
            *par_pt=FormulaInversa_Conc_Percent();

            break;

    case UNIT_MIS_CONCENTR_PUNT_TITOL:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_PuntTitol(*par_pt);
            if(!(struct_conc_print.conc_to_print<decr))struct_conc_print.conc_to_print-=decr;
            *par_pt=FormulaInversa_Conc_PuntTitol();

            break;

    case UNIT_MIS_CONCENTR_GRAMMILITRO:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_grammiLitro(*par_pt);
            if(!(struct_conc_print.conc_to_print<decr))struct_conc_print.conc_to_print-=decr;
            *par_pt=FormulaInversa_Conc_grammiLitro();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            
            break;

    case UNIT_MIS_CONCENTR_uSIEMENS:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_uSiemens(*par_pt);
            if(!(struct_conc_print.conc_to_print<decr))struct_conc_print.conc_to_print-=decr;
            *par_pt=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            break;

    case UNIT_MIS_CONCENTR_mSIEMENS:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_milliSiemens(*par_pt);
            if(!(struct_conc_print.conc_to_print<decr))struct_conc_print.conc_to_print-=decr;
            *par_pt=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            break;

    default:
            break;
  }
          
        
	
}
//***************************************************************************************************************************************
void IncrParamConc(unsigned int* par_pt,unsigned short incr)//il valore da stampare si troverà nella struttura struct_conc_to_print
{


  switch(PROGR_IN_USO.unita_mis_concentr)
  {
    case UNIT_MIS_CONCENTR_PERCENTUALE:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_Percent(*par_pt);
            struct_conc_print.conc_to_print+=incr;
            *par_pt=FormulaInversa_Conc_Percent();

            break;

    case UNIT_MIS_CONCENTR_PUNT_TITOL:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_PuntTitol(*par_pt);
            struct_conc_print.conc_to_print+=incr;
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
            struct_conc_print.conc_to_print+=incr;
            *par_pt=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            break;

    case UNIT_MIS_CONCENTR_mSIEMENS:
            //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
            Formula_ConcConvers_milliSiemens(*par_pt);
            struct_conc_print.conc_to_print+=incr;
            *par_pt=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
            break;

    default:
            break;
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
		if(!(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MIN_INDEX] <
                      PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MAX_INDEX] ))
                {
			return;
		}
	}

	if(index==SOGLIE_ALL_TEMP_MIN_INDEX)
	{
		if(!(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MIN_INDEX] <
                      PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MAX_INDEX] ))
		{
			return;
		}
	}

	SelectFont(CALIBRI_10);
	if(index<4)//solo per le concentrazioni
	{       //potrei usare puntatore a funzione ,ma per comodità di debug uso lo switch
		switch(PROGR_IN_USO. unita_mis_concentr)
		{
		  case UNIT_MIS_CONCENTR_PERCENTUALE:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_Percent(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print+=incr;
                          temp_test=FormulaInversa_Conc_Percent();
			 //prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_PUNT_TITOL:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
                          Formula_ConcConvers_PuntTitol(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print+=incr;
                          temp_test=FormulaInversa_Conc_PuntTitol();
 			  break;

		  case UNIT_MIS_CONCENTR_GRAMMILITRO:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_grammiLitro(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_grammiLitro();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_uSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_uSiemens(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_mSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_milliSiemens(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  default:
			  break;
		}
          if(!(temp_test>conc_soglie_limit_up.setp_e_soglie_arr[index]))
          {
                  PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]= temp_test;
          }
          else
          {}      
                
	}
	else//temperature
	{
		multiplied=PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index];
		multiplied/=10;
		resto=multiplied % 10;

		if(multiplied<TEMP_MAX_LIMIT)multiplied++;

		multiplied*=10;
		multiplied+=resto;
		PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]=multiplied ;

	}
}
//***************************************************************************************
void DecrSoglia(unsigned short index,unsigned short incr)//viene chiamata riga per riga quindi stampa solo 1 valore
{

	unsigned int   multiplied;
	unsigned short resto;
        unsigned int  temp_test;



	//controllo che il valore minimo delle soglie di allarme non sia superiore al massimo
	if(index==SOGLIE_ALL_CONC_MAX_INDEX)
	{
		if(!(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MAX_INDEX]> PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_CONC_MIN_INDEX]))
		{
			return;
		}
	}

	if(index==SOGLIE_ALL_TEMP_MIN_INDEX)
	{
		if(!(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MAX_INDEX]> PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[SOGLIE_ALL_TEMP_MIN_INDEX]))
		{
			return;
		}
	}


	SelectFont(CALIBRI_10);
	if(index<4)//solo per le concentrazioni
	{
         
          switch(PROGR_IN_USO.unita_mis_concentr)
		{
		  case UNIT_MIS_CONCENTR_PERCENTUALE:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_Percent(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print-=incr;
                          temp_test=FormulaInversa_Conc_Percent();

			  break;

		  case UNIT_MIS_CONCENTR_PUNT_TITOL:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_PuntTitol(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
                          struct_conc_print.conc_to_print-=incr;
                          temp_test=FormulaInversa_Conc_PuntTitol();

			  break;

		  case UNIT_MIS_CONCENTR_GRAMMILITRO:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_grammiLitro(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print+=incr;
			  temp_test=FormulaInversa_Conc_grammiLitro();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  
			  break;

		  case UNIT_MIS_CONCENTR_uSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_uSiemens(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print-=incr;
			  temp_test=FormulaInversa_Conc_uSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  case UNIT_MIS_CONCENTR_mSIEMENS:
			  //64000=100%  qui vogliono la risoluzione dello 0,1% quindi 64000=>10000
			  Formula_ConcConvers_milliSiemens(PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]);
			  struct_conc_print.conc_to_print-=incr;
			  temp_test=FormulaInversa_Conc_milliSiemens();//prima di assegnare il nuovo valore alla varibile alla quale il programma farà riferimento lo testo
			  break;

		  default:
			  break;
		}
                
              if(!(temp_test>conc_soglie_limit_up.setp_e_soglie_arr[index]))
              {
                      PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]= temp_test;
              }
              else
              {

              }

	}
	else
	{
		multiplied=PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index];
		multiplied/=10;
		resto=multiplied % 10;

		if(multiplied>TEMP_MIN_LIMIT)multiplied--;

		multiplied*=10;
		multiplied+=resto;
		PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]=multiplied ;
	}
}
//***************************************************************************************
void PrintSoglia(unsigned short index,unsigned short x ,unsigned short y)//viene chiamata riga per riga quindi stampa solo 1 valore
{
	unsigned int   multiplied;
        unsigned int selected_progr=RamSettings.selected_program_id;
	SelectFont(CALIBRI_10);
	if(index<4)//solo concentrazioni
	{
		switch(PROGR_IN_USO.unita_mis_concentr)
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
		multiplied=PROGR_IN_USO.setp_e_soglie.setp_e_soglie_arr[index]; ;
		multiplied/=10;

		CleanArea_Ram_and_Screen(x,x+30,y,y+10);
		BinToBCDisp(multiplied,UN_DECIMALE,x,y);
	}
}
//***************************************************************************************
void PrintUnitMis(unsigned short index,unsigned short x ,unsigned short y)//viene chiamata riga per riga quindi stampa solo 1 valore
{
	SelectFont(CALIBRI_10);
	if(index<4)
	{
		LCDPrintString(StringsSubmenuSimboliConc[PROGR_IN_USO.unita_mis_concentr],x,y);
	}
	else
	{
              mybmp_struct2.bmp_pointer=gradi_small_bmp;
              mybmp_struct2.righe   =gradi_small_HeightPixels;
              mybmp_struct2.colonne =gradi_small_WidthPages;
              mybmp_struct2.start_x=x;
              mybmp_struct2.start_y=y;
              GetBitmap();	
              LCDPrintString("C",x+4,y);
	}
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
void LoadDisplay_Logo(void)
{
    mybmp_struct2.bmp_pointer=unitekLogo_bmp;
    mybmp_struct2.righe	     =unitekLogoHeightPixels;
    mybmp_struct2.colonne    =unitekLogoWidthPages;
    mybmp_struct2.start_x=18;
    mybmp_struct2.start_y=2;
    GetBitmap();
    LCD_CopyPartialScreen(0,128,0,64);
    
    vTaskDelay(2000);
}
//***************************************************************************************
void LoadRamSettingsFrom_uC_Flash(void)
{
	const unsigned char* cuipt=(const unsigned char *)&ucFlash_Settings;//0xFF000;
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
  
  
  
  
  if(RamSettings.selected_program_id>NUM_PROGRAMMI_MAX_INDEX)RamSettings.selected_program_id=0;
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
                                                                                                     
//***************************************************************************************                                                                                                     
void MyCreateTimers(void)
{
  int32_t x;
  
 
      // Create then start some timers.  Starting the timers before the scheduler
      // has been started means the timers will start running immediately that
      // the scheduler starts.
      for( x = 0; x < NUM_TIMERS; x++ )
      {
          xTimers[ x ] = xTimerCreate(    "Timer",       // Just a text name, not used by the kernel.
                                          (PROGR_IN_USO.Timers.Timers_values[x]*1000),   // The timer period in ticks.
                                          pdFALSE,        // The timers will auto-reload themselves when they expire.
                                          ( void * ) x,  // Assign each timer a unique id equal to its array index.
                                          vTimerCallback // Each timer calls the same callback when it expires.
                                      );
 
          if( xTimers[ x ] == NULL )
          {
              // The timer was not created.
          }
          else
          {
              // Start the timer.  No block time is specified, and even if one was
              // it would be ignored because the scheduler has not yet been
              // started.
             // if( xTimerStart( xTimers[ x ], 0 ) != pdPASS )
              /*{
                  // The timer could not be set into the Active state.
              }*/
          }
      }
      
      
  
}
//***************************************************************************************   
// Define a callback function that will be used by multiple timer instances.
  // The callback function does nothing but count the number of times the
  // associated timer expires, and stop the timer once the timer has expired
  // 10 times.
  void vTimerCallback( TimerHandle_t pxTimer )
  {
  int32_t lArrayIndex;
  //const int32_t xMaxExpiryCountBeforeStopping = 10;
 
  	   // Optionally do something if the pxTimer parameter is NULL.
  	   configASSERT( pxTimer );
 
      // Which timer expired?
      lArrayIndex = ( int32_t ) pvTimerGetTimerID( pxTimer );
      timer_flags |=1<<lArrayIndex;//uno dei 9 lsb di timer flags va a 1
      //xTimerStop( pxTimer, 0 );//ANGELO
 
      // Increment the number of times that pxTimer has expired.
     // lExpireCounters[ lArrayIndex ] += 1;
 
      // If the timer has expired 10 times then stop it from running.
      //if( lExpireCounters[ lArrayIndex ] == xMaxExpiryCountBeforeStopping )
     // {
          // Do not use a block time if calling a timer API function from a
          // timer callback function, as doing so could cause a deadlock!
         // xTimerStop( pxTimer, 0 );
     // }
  }

//***************************************************************************************
void DisegnaMarker(unsigned short x,unsigned short y,unsigned short y_old)
{
	mybmp_struct1.start_x=x;
	mybmp_struct1.start_y=y;
	CleanArea_Ram_and_Screen(x,x+10,y_old,y_old+10);
	CleanArea_Ram_and_Screen(x,x+10,y,y+10);
	LCD_DrawChar('*');
	LCD_CopyPartialScreen(x,x+10,y,y+10);
}
//***************************************************************************************
void DisegnaCarattereBlink(char char_to_blink,unsigned short x,unsigned short y,unsigned char *toggler)
{
	//if(!blink_timer_on)
	{
		//blink_timer_on=50;
		if(*toggler)
		{
			*toggler=0;
			CleanArea_Ram_and_Screen(x,x+10,y,y+10);
		}
		else
		{
			*toggler=1;
			mybmp_struct1.start_x=x;
			mybmp_struct1.start_y=y;
			LCD_DrawChar(char_to_blink);
			LCD_CopyPartialScreen(x,x+10,y,y+10);
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
//viene chiamata quando ci si avvicina al limite della variabile da decrementare
void RiduciIncrDecrStep(int * step,int * counter)
{
  if(*step==1)return;
  
  if(*step==100)
  {
    *step=10;
    *counter=11;
  }
  else
  {
    if(*step==10)*step=1;
    *counter=0;
  }
 
}
//***************************************************************************************
void AumentaIncrDecrStep(int * step,int * counter)
{
    if(*step==100)
    {
      *counter=21;
      return;
    }
    if(*counter>10)
    {
          *step=10;
    }
    if(*counter>20)
    {
          *step=100;
          *counter=21;
    }
}
//***************************************************************************************                                                                 
unsigned char ControlloCongruita_CurvaLav(void)
{

  //controllo indici
  if(PROGR_IN_USO.curva_lav_L_index<PROGR_IN_USO.curva_lav_C_index){}
   else return FALSE;
   
  if(PROGR_IN_USO.curva_lav_C_index<PROGR_IN_USO.curva_lav_H_index){}
   else return FALSE;
  
  
  //controllo valori immessi
   if(PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_L_index]  < 
      PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]){}
   else return FALSE; 
  
   if(PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]  < 
      PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index]){}
   else return FALSE; 
   
   //controllo valori misurati all'ok
    if(PROGR_IN_USO.curva_lav_XconducL  < PROGR_IN_USO.curva_lav_XconducC  ){}
   else return FALSE;
   
   //controllo valori misurati all'ok
    if(PROGR_IN_USO.curva_lav_XconducC  < PROGR_IN_USO.curva_lav_XconducH  ){}
   else return FALSE;
   
   
  
  return TRUE;
}
