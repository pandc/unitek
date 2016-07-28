/*
 * Submenu.c
 *
 *  Created on: 11/apr/2016
 *      Author: Administrator
 */

#include "bsp.h"
#include "io.h"
#include "time.h"
#include "timers.h"
#include "stdio.h"
#include "string.h"

#include "freertos.h"
#include "task.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"
#include "Menu.h"
#include "keyboard.h"
#include "VariabiliGlobali_di_Lavoro.h"


/*
struct
{
	unsigned short SetConc;
	unsigned short AllConcMin;
	unsigned short AllConcMax;
	unsigned short IsteresiConc;
	unsigned short SetTemp;
	unsigned short AllTempMin;
	unsigned short AllTempMax;
	unsigned short IsteresiTemp;

}StructSoglie;
*/

int incr_step,incr_counter;
unsigned int  glob_key=0;

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

extern unsigned char MenuFunction_Index;

extern  TimerHandle_t AuxTimer; 


extern unsigned char array_line[128];//un array abbastanza grande da contenere la + lunga linea possibile
extern unsigned char char_size;

extern unsigned char screen_image[1024];

extern unsigned int key_flags;


extern unsigned char loop_flag;

extern const char StringsSubmenuProg      	[4][5][22];
extern const char StringsSubmenuSelezProg 	[4][5][20];
extern const char StringsSubmenuLingua    	   [4][10];
extern const char StringsSubmenuImpostaSimboli	[4][5][20];
extern const char StringsSubmenuImpostaSoglie 	[4][8][20];
extern const char StringsSubmenuImpostaTimer  	[4][9][20];
extern const char StringsSubmenuSimboliConc   	    [5][4];
extern const char StringsServizio               [4][5][22];
extern const char Strings_GGMMAA                [4][10];

extern setp_e_soglie_type setp_e_soglie;
extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;

struct tm *tm;
struct tm  my_tm;
//***************************************************************************************
void SubmenuINOUT(void)
{
	uint8_t key;
	

	unsigned char loop_flag=1;
	unsigned short submenuINOUT_index,y_old;

        
        LCD_Fill_ImageRAM(0x00);
	menu_triang_limit_up=26;
	menu_triang_limit_dn=38;
	

	if(RamSettings.abilita_disabilita)	submenuINOUT_index=0;
	else 			                submenuINOUT_index=1;

	menu_triang_y=26+(submenuINOUT_index*12);

	y_old=menu_triang_y;



	DisegnaTriangolinoMenu(0,menu_triang_y);

	LCDPrintString("IN - OUT",	44,2);

	LCDPrintString("ABILITA",	46,26);
	LCDPrintString("DISABILITA",38,38);

	LCD_CopyScreen();

	DisegnaMarker(92,menu_triang_y,menu_triang_y);

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);
		
		if (key == KEY_PROG)
		//
		{
			;
			MenuFunction_Index=MENU_PROGR;
			return;

		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			
			if(submenuINOUT_index==0)
			{
                            RamSettings.abilita_disabilita=ABILITA;
                            {
                                  //controllo se arrivo da stato abilita
                                  //in tal caso azzero i timeouts
                                  if(!CHECK_STATE_ABILITATO)//se arrivo da disabilitato
                                  {
                                    ResetChConc_ON();
                                    ResetChTemp_ON();
                                  }
                             }
			}//chiudi i relay ecc
			else
			{
				RamSettings.abilita_disabilita=DISABILITA ;
                                ResetChConc_OFF();
                                ResetChTemp_OFF();
			}//apri i relay ecc

			DisegnaMarker(92,menu_triang_y,y_old);
			y_old=menu_triang_y;
			//RamSettings.abilita_disabilita=abilita_disabilita;
			//SaveImageInFlash();


		}

		if (key == KEY_DOWNRIGHT)
		//if(CHECK_TASTO_DN_DX_PRESSED)
		{
			MoveTriangolinoDown();
			if(submenuINOUT_index<1)submenuINOUT_index+=1;
			

		}

		if (key == KEY_UPLEFT)
		//if(CHECK_TASTO_UP_SX_PRESSED)
		{
			MoveTriangolinoUp();
			if(submenuINOUT_index)submenuINOUT_index-=1;
			
		}
	}
}
//***************************************************************************************
void SubmenuSelProgr(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=2,y_old;
	unsigned char loop_flag=1;
/*
	unsigned char x_prova=0,half_x=0;
	unsigned char y_prova=0,half_y=0;
*/
	unsigned short submenuSelProg_index,sel_progr_old;

	if(RamSettings.selected_program_id>NUM_PROGRAMMI_MAX_INDEX)RamSettings.selected_program_id=0;
	submenuSelProg_index=RamSettings.selected_program_id;
        sel_progr_old=submenuSelProg_index;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(submenuSelProg_index*12);

	LCD_Fill_ImageRAM(0x00);

	DisegnaTriangolinoMenu(0,menu_triang_y);
	DisegnaMarker(72,menu_triang_y,menu_triang_y);
        y_old=menu_triang_y;
	SelectFont(CALIBRI_10);


	for(string_index=0;string_index<5;string_index++)
	{
		LCDPrintString("Progr",12,strings_y);
		BinToBCDisp(string_index+1  ,INTERO,40,strings_y);
		strings_y+=12;
	}

	LCD_CopyScreen();

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);

		if (key == KEY_PROG)
		//
		{
			
			MenuFunction_Index=MENU_PROGR;
			loop_flag=0;
		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			
			
			//program_arr=saved_prog_arr[selected_progr_index];
			DisegnaMarker(72,menu_triang_y,y_old);
			y_old=menu_triang_y;
			MenuFunction_Index=SUBMENU_SELECTED_PROGR;
			loop_flag=0;
			
                        RamSettings.selected_program_id=submenuSelProg_index;
                        if(sel_progr_old!=submenuSelProg_index)SaveInFlash();//salvo solo se ho cambiato
		}


		if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
		{
			if (key == KEY_DOWNRIGHT)
			//if(CHECK_TASTO_DN_DX_PRESSED)
			{
				MoveTriangolinoDown();
				if(submenuSelProg_index<5)submenuSelProg_index+=1;
				
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(submenuSelProg_index)submenuSelProg_index-=1;
				
			}
		}

		
		{



		}
	}
}
//***************************************************************************************
void SubmenuSelectedProgr(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=2;
	unsigned char loop_flag=1;
/*
	unsigned char x_prova=0,half_x=0;
	unsigned char y_prova=0,half_y=0;
*/
	unsigned short submenuSelProg_index=0;


	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(submenuSelProg_index*12);

	LCD_Fill_ImageRAM(0x00);
#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);




	SelectFont(CALIBRI_10);


	for(string_index=0;string_index<5;string_index++)
	{
		LCDPrintString(StringsSubmenuSelezProg[RamSettings.Linguaggio][string_index],12,strings_y);
		strings_y+=12;
	}

	LCD_CopyScreen();

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);

		if (key == KEY_PROG)
		//
		{
			;
			MenuFunction_Index=SUBMENU_SELEZIONA_PROG;
			loop_flag=0;

		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			
			switch(submenuSelProg_index)
			{
				case 0:
					MenuFunction_Index=SUB2MENU_IMPOSTA_SIMBOLI;
					break;
				case 1:
					MenuFunction_Index=SUB2MENU_TK;
					break;

				case 2:
					MenuFunction_Index=SUB2MENU_IMPOSTA_SOGLIE ;
					break;

				case 3:
					MenuFunction_Index=SUB2MENU_SEL_TIPO_CURV_LAV;
					break;

				case 4:
					MenuFunction_Index=SUB2MENU_IMPOSTA_TIMER;
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
				if(submenuSelProg_index<5)submenuSelProg_index+=1;
				
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(submenuSelProg_index)submenuSelProg_index-=1;
				
			}
		}

		
		{



		}
	}


}
//***************************************************************************************
void SubmenuComunic(void)
{
	uint8_t key;
	unsigned char loop_flag=1,to_print=1;
	static unsigned short submenuComunic_index=0;

	LCD_Fill_ImageRAM(0x00);

	stato_RS485=RamSettings.ComunRS485;
	stato_USB  =RamSettings.ComunUSB;


	menu_triang_y=26+(submenuComunic_index*12);



#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);
	LCDPrintString("Comunicazioni",	20,2);
	LCDPrintString("RS 485 PLC"	,	12,26);
	LCDPrintString("USB",			12,38);
	LCD_CopyScreen();


	menu_triang_limit_up=26;
	menu_triang_limit_dn=38;
	menu_triang_limit_dx=76;
	menu_triang_limit_sx=0;


	#ifdef DISEGNA_CORNICE
	DisegnaCornice();
	#endif

	//DisegnaTriangolinoMenu(0,menu_triang_y);



	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);
		
		{
			
			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuComunic_index<1)submenuComunic_index+=1;
					else				 submenuComunic_index=1;
					
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuComunic_index)submenuComunic_index-=1;
					else 			   submenuComunic_index =0;
					
				}
			}
			

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				


				if(menu_triang_x==0)
				{
					MoveTriangolinoDx();
					//non marcare MOVE_SXDX!!
					CLEAR_ARROW_KEYS_MOVE_UPDOWN;
					MARK_PIU_MENO_ENABLED;
				}
				else
				{
					MoveTriangolinoSx();
					MARK_ARROW_KEYS_MOVE_UPDOWN;
					
					CLEAR_PIU_MENO_ENABLED;

					//RamSettings.ComunRS485=stato_RS485;
					//RamSettings.ComunUSB=stato_USB;
					//SaveImageInFlash();
				}
			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if (key == KEY_PLUS)
				//if(CHECK_TASTO_PLUS_PRESSED)
					{

						if(menu_triang_x==76)
						{

							if(submenuComunic_index==0)
							{
								if((stato_RS485)<12)
								{
									stato_RS485++;
									to_print=1;
								}
							}

							else
							{
								if(stato_USB==0)stato_USB=1;

								to_print=1;
							}
						}
					}




					if (key == KEY_MINUS)
					//if(CHECK_TASTO_MENO_PRESSED)
					{

						if(submenuComunic_index==0)
						{
							if((stato_RS485)>0)
							{
								stato_RS485--;
								to_print=1;
							}
						}

						else
						{
							if(stato_USB)stato_USB=0;
							to_print=1;
						}
					}


				}

			if (key == KEY_PROG)
			
			{
				;
				MenuFunction_Index=MENU_PROGR;
				loop_flag=0;
			}


			if(to_print)
			{
				CleanArea_Ram_and_Screen(90,124,26,50);
				BinToBCDisp(stato_RS485,INTERO,90,26);
				BinToBCDisp(stato_USB  ,INTERO,90,38);
				LCD_CopyPartialScreen(90,124,26,50);

				to_print=0;
			}

		}
	}
}
//***************************************************************************************
void SubmenuSetClock(void)
{
  uint8_t key,modificato,last_key;
  unsigned char loop_flag=1,to_print;
  
  unsigned char time_date_to_change=0;
  unsigned char mark_change=0;
  unsigned char day_limit;
  


  LCD_Fill_ImageRAM(0x00);
  


  SelectFont(CALIBRI_10);
  
  
  LCDPrintString("hh  :  mm",HOUR_X-4,HMIN_Y-12);
  LCDPrintString(Strings_GGMMAA[RamSettings.Linguaggio],DAY_X+8,DATE_Y-12);
  LCD_CopyScreen();
  
  
  
  uint32_t t;
  time_t my_t = time(NULL);
  tm = localtime(&my_t); 
  
  
  
  if( xTimerStart( AuxTimer, 0 ) != pdPASS ){}//timer blink

  //LCD_CopyScreen();

  key = last_key = 0;
  while(loop_flag)
  {
  if (key_getstroke(&key,kDec*2))last_key = key;
      else	key = 0;
  
  
          
  if (key == KEY_OK)
  //if(CHECK_TASTO_OK_PRESSED)
  {
    my_tm=*tm;
    t = mktime(&my_tm);
    RTC_SetCounter(t);      
  }

  if (key == KEY_PROG)
  
  {
          if(modificato)RTC_SetCounter(0);;//salvo solo se ho premuto i tasti  + o -
          MenuFunction_Index=MENU_PROGR;
          CLEAR_PIU_MENO_ENABLED ;  
          xTimerStop( AuxTimer, 0 );//spengo timer per lampeggio
          loop_flag=0;
          

  }

  if(CHECK_PIU_MENO_ENABLED)
  {
    if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
    //if(CHECK_TASTO_PLUS_PRESSED)
    {
     modificato=1;
     to_print=1;
     switch(time_date_to_change)
     {
       case 0:
         
         break;
         
       case CHANGE_HOUR:
         if(tm->tm_hour <23)tm->tm_hour++;
         break;
         
       case CHANGE_MIN:
         if(tm->tm_min <59)tm->tm_min++;
         break;
         
       case CHANGE_YEAR:
         if(tm->tm_year<2100)tm->tm_year++;
         break;
         
       case CHANGE_MONTH:
         if(tm->tm_mon<11)tm->tm_mon++; 
         break;
         
       case CHANGE_DAY:
         day_limit=31;
         if(tm->tm_mon==3) day_limit=30;//Aprile
         if(tm->tm_mon==5) day_limit=30;//Giugno
         if(tm->tm_mon==8) day_limit=30;//Settembre
         if(tm->tm_mon==10)day_limit=30;//Novembre
         if(tm->tm_mon==1)              //Febbraio
         {
           if((tm->tm_year+1900)%4)day_limit=28;//non bisestile
           else                    day_limit=29;//    bisestile   
         }
         
         if(tm->tm_mday<day_limit)tm->tm_mday++;
         break;
                    
     }//fine switch
              
    }
    else if (key == (KEY_PLUS | KEY_RELEASED))

    {
            incr_step=1;
            incr_counter=0;
            
    }

    if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
    //if(CHECK_TASTO_MENO_PRESSED)
    {
     modificato=1;
     to_print=1;
     switch(time_date_to_change)
     {
       case 0:
         
         break;
         
       case CHANGE_HOUR:
         if(tm->tm_hour )tm->tm_hour--;
         break;
         
       case CHANGE_MIN:
         if(tm->tm_min )tm->tm_min--;
         break;
         
       case CHANGE_YEAR:
         if(tm->tm_year)tm->tm_year--;
         break;
         
       case CHANGE_MONTH:
         if(tm->tm_mon)tm->tm_mon--; 
         break;
         
       case CHANGE_DAY:
        
         if(tm->tm_mday)tm->tm_mday--;
         break;
                    
     }//fine switch
              
      }
      else if (key == (KEY_MINUS | KEY_RELEASED))
      //if(CHECK_TASTO_MENO_RELEASED)
      {
              incr_step=1;
              incr_counter=0;

      }

  }


  if (key == KEY_DOWNRIGHT)
  {
     mark_change=1; 
     MARK_PIU_MENO_ENABLED ;    
  }

  if (key == KEY_UPLEFT)
  {
     mark_change=2;
     MARK_PIU_MENO_ENABLED ;  
  }

  if(CHECK_BLINK_TIMER1_EXPIRED)
  {
      CLEAR_BLINK_TIMER1_EXPIRED;
      if(CHECK_BLINK_SHOW)//disegno
      {
       CLEAR_BLINK_SHOW; 
       
       switch(time_date_to_change)
       {
         case 0:
           
           break;
           
         case CHANGE_HOUR:
           LCD_CopyPartialScreen(HOUR_X,H_MIN_SEP_X-2,HMIN_Y,HMIN_Y+12);
           break;
           
         case CHANGE_MIN:
           LCD_CopyPartialScreen(MIN_X,MIN_X+14,HMIN_Y,HMIN_Y+12);
           break;
           

           
          case CHANGE_DAY:
            LCD_CopyPartialScreen(DAY_X,DAY_X+14,DATE_Y,DATE_Y+12); 
            break;         

         case CHANGE_MONTH:
           LCD_CopyPartialScreen(MONTH_X,MONTH_X+14,DATE_Y,DATE_Y+12); 
           break;
            
        case CHANGE_YEAR:
           LCD_CopyPartialScreen(YEAR_X,YEAR_X+24,DATE_Y,DATE_Y+12); 
           break;
                      
       }//fine switch
       //per evitare di cambiare parametro col precedente spento
       if(mark_change==2) 
       {
         if(time_date_to_change<5)time_date_to_change++; 
         mark_change=0;
       }
       
        if(mark_change==1)
        {
         if(time_date_to_change)time_date_to_change--;  ; 
         mark_change=0;
        }  
       
      }
      else
      {
        MARK_BLINK_SHOW;
        
        switch(time_date_to_change)
       {
         case 0:
           
           break;
           
         case CHANGE_HOUR:
           CleanAreaScreenOnly(HOUR_X,H_MIN_SEP_X-2,HMIN_Y,HMIN_Y+12);
           break;
           
         case CHANGE_MIN:
           CleanAreaScreenOnly(MIN_X,MIN_X+12,HMIN_Y,HMIN_Y+12);
           break;
         
         case CHANGE_DAY:
           CleanAreaScreenOnly(DAY_X,DAY_X+14,DATE_Y,DATE_Y+12);
           break;
            
         case CHANGE_MONTH:
           CleanAreaScreenOnly(MONTH_X,MONTH_X+14,DATE_Y,DATE_Y+12); 
           break;
      
          case CHANGE_YEAR:
           CleanAreaScreenOnly(YEAR_X,YEAR_X+48,DATE_Y,DATE_Y+12); 
           break;           
        
                      
       }//fine switch
      }
  }


  if(to_print)
  {
          to_print=0;

          PrintTimeDate();

  }



	}
}
//***************************************************************************************
void SumMenuSelLingua(void)
{
	uint8_t key;

	unsigned short string_index=0,strings_y=2,y_old;
        unsigned char loop_flag=1;
/*
        unsigned char x_prova=0,half_x=0;
        unsigned char y_prova=0,half_y=0;
*/
        unsigned short submenuSelLing_index,ling_index_old;



        submenuSelLing_index=RamSettings.Linguaggio;
        ling_index_old=submenuSelLing_index;
          
          
        menu_triang_limit_up=2;
        menu_triang_limit_dn=38;

        menu_triang_y=2+(submenuSelLing_index*12);

		LCD_Fill_ImageRAM(0x00);
	#ifdef DISEGNA_CORNICE
		DisegnaCornice();
	#endif
		DisegnaTriangolinoMenu(0,menu_triang_y);
		DisegnaMarker(72,menu_triang_y,menu_triang_y);
                y_old=menu_triang_y;
		SelectFont(CALIBRI_10);


		for(string_index=0;string_index<4;string_index++)
		{
                    LCDPrintString(StringsSubmenuLingua[string_index],12,strings_y);
                    strings_y+=12;
		}

		LCD_CopyScreen();

		while(loop_flag)
		{
			key_getstroke(&key,portMAX_DELAY);
			if (key == KEY_PROG)
			//
			{
				if(ling_index_old!=submenuSelLing_index)SaveInFlash();
				MenuFunction_Index=MENU_PROGR;
				loop_flag=0;

			}

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				
				RamSettings.Linguaggio=submenuSelLing_index;
                                                              
				DisegnaMarker(72,menu_triang_y,y_old);
				y_old=menu_triang_y;

				loop_flag=0;
				//RamSettings.Linguaggio=linguaggio;
				//SaveImageInFlash();
			}


			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuSelLing_index<3)submenuSelLing_index+=1;
					
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuSelLing_index)submenuSelLing_index-=1;
					
				}
			}

			
		}
}
//***************************************************************************************
void SubmenuServizio(void)
{
  uint8_t key;
  
  unsigned short string_index=0,strings_y=2/*,y_old*/;
  unsigned char loop_flag=1;

  unsigned short submenuServizio_index=0;
   


  

  LCD_Fill_ImageRAM(0x00);

  

  SelectFont(CALIBRI_10);

  LCDPrintString("PASSWORD?",40,26);
  LCD_CopyScreen();
  
  unsigned char stato=glob_key;
  
  while(loop_flag)
  {
      key_getstroke(&key,portMAX_DELAY);
      if (key == KEY_PROG)
      {
         
          MenuFunction_Index=MENU_PROGR;
          loop_flag=0;
          glob_key=0;
          return;

      }
      
      if (key == KEY_OK)
      {
        if(stato==1)stato++;
        else
        {
          MenuFunction_Index=MENU_PROGR;
          loop_flag=0;
          glob_key=0;
          return;
        }
      }
      
      if (key == KEY_MINUS)
      {
        if(stato==0)stato++;
        if(stato==3)stato++;
      }
           
      if (key == KEY_PLUS)
      {
        if(stato==2)stato++;
        else
        {
          MenuFunction_Index=MENU_PROGR;
          glob_key=0;
          loop_flag=0;
          return;
        }
      }
           
      if(stato==4)loop_flag=0;      
  
  }
  loop_flag=1;
  glob_key=4;//ormai sono dentro
  
  
  LCD_Fill_ImageRAM(0x00);
  menu_triang_limit_up=2;
  menu_triang_limit_dn=38;
  menu_triang_y=2;
  
  DisegnaTriangolinoMenu(0,menu_triang_y);
  
  
  for(string_index=0;string_index<5;string_index++)
  {
          LCDPrintString(StringsServizio[RamSettings.Linguaggio][string_index],12,strings_y);
          strings_y+=12;
  }
  
  LCD_CopyScreen();
  
  
  
  while(loop_flag)
  {
      key_getstroke(&key,portMAX_DELAY);
      if (key == KEY_PROG)
      {
         
          MenuFunction_Index=MENU_PROGR;
          glob_key=0;
          loop_flag=0;

      }
      
      if (key == KEY_OK)
      {
         
        switch(submenuServizio_index)
        {
        case 0:
              MenuFunction_Index=SUB2MENU_CAL_PT100;
            break;
          
        case 1:
             MenuFunction_Index=SUB2MENU_CABLE_COMPENS;
            break;
            
        case 2:
              MenuFunction_Index=SUB2MENU_MISURA_DIRETTA;
            break;
            
        case 3:
          MenuFunction_Index=SUB2MENU_RES_SERIE_CAVO;
        break;
            
        case 4:
              MenuFunction_Index=SUB2MENU_LICENZA;
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
                      if( submenuServizio_index<3) submenuServizio_index+=1;
                      
              }

              if (key == KEY_UPLEFT)
              //if(CHECK_TASTO_UP_SX_PRESSED)
              {
                      MoveTriangolinoUp();
                      if( submenuServizio_index) submenuServizio_index-=1;
                      
              }
      }
  }
 }
//***************************************************************************************   
void PrintTimeDate(void)
{
  char string_to_print[16];

  CleanArea_Ram_and_Screen(HOUR_X,MIN_X+14,HMIN_Y,HMIN_Y+12);
  
  sprintf(string_to_print,"%02u",tm->tm_hour);
  LCDPrintString(string_to_print,HOUR_X,HMIN_Y);
  
  LCDPrintString(":",H_MIN_SEP_X,12);
  
  sprintf(string_to_print,"%02u",tm->tm_min);
  LCDPrintString(string_to_print,MIN_X,HMIN_Y);
  
  LCD_CopyPartialScreen(HOUR_X,MIN_X+24,HMIN_Y,HMIN_Y+12);
  
  
  //riga data
  CleanArea_Ram_and_Screen(DAY_X,YEAR_X+24,DATE_Y,DATE_Y+12);
  
  sprintf(string_to_print,"%02u",tm->tm_mday);
  LCDPrintString(string_to_print,DAY_X,DATE_Y);
   
  
  LCDPrintString("/",MONTH_DAY_SEP_X,DATE_Y);
  
  sprintf(string_to_print,"%02u",tm->tm_mon+1);
  LCDPrintString(string_to_print,MONTH_X,DATE_Y);
 
  LCDPrintString("/",Y_MONTH_SEP_X,DATE_Y);
  
  sprintf(string_to_print,"%u",tm->tm_year+1900);
  LCDPrintString(string_to_print,YEAR_X,DATE_Y);
  
 
  
  LCD_CopyPartialScreen(DAY_X,YEAR_X+24,DATE_Y,DATE_Y+12);
}


