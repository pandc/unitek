/*
 * SubSubMenu.c
 *
 *  Created on: 22/apr/2016
 *      Author: Administrator
 */

#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"
#include "meas.h"

#include "my_definitions.h"
#include "Display_128x64.h"
#include "SSD1322.h"
#include "BitmapsImmagini.h"
#include "Conversioni.h"
#include "keyboard.h"
#include "Menu.h"
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


extern unsigned char incr_step,incr_counter;

extern bitmap_struct_type mybmp_struct1,mybmp_struct2;

extern unsigned char MenuFunction_Index;
extern unsigned char unita_mis_concentr;


extern unsigned int keyold_flags;

extern  TimerHandle_t xTimers[ NUM_TIMERS ];

extern unsigned char array_line[128];//un array abbastanza grande da contenere la + lunga linea possibile
extern unsigned char char_size;

extern unsigned char screen_image[1024];
extern unsigned int key_flags;


extern unsigned char loop_flag;
extern unsigned int blink_timer_on;


extern const char StringsSubmenuProg      	[4][5][20];
extern const char StringsSubmenuSelezProg 	[4][5][20];
extern const char StringsSubmenuLingua    	   [4][10];
extern const char StringsSubmenuImpostaSimboli	[4][5][20];
extern const char StringsSubmenuImpostaSoglie 	[4][8][20];
extern const char StringsSubmenuImpostaTimer  	[4][9][20];
extern const char StringsSubmenuSimboliConc   	[5][4];
extern const char StringsSubmenuCurvaLavoro  	[16][4];
extern const char StringsSubmenuTipoCurvLav     [4][2][20];

//***************************************************************************************
void  Sub2MenuSelTipoCurvaLavoro(void)
{
	uint8_t key;
	unsigned short string_index=0,strings_y=14,y_old;
        unsigned char loop_flag=1,test;
        unsigned char selected_progr=RamSettings.selected_program_id;
        static unsigned short submenuCurLavType_index;
         
        submenuCurLavType_index=RamSettings.ptype_arr[selected_progr].curva_lav_cal_type;

        menu_triang_limit_up=14;
        menu_triang_limit_dn=26;
        menu_triang_y=14+(submenuCurLavType_index*12);

        LCD_Fill_ImageRAM(0x00);
	#ifdef DISEGNA_CORNICE
	DisegnaCornice();
	#endif
        DisegnaTriangolinoMenu(0,menu_triang_y);
        DisegnaMarker(102,menu_triang_y,menu_triang_y);
        y_old=menu_triang_y;
        SelectFont(CALIBRI_10);


        for(string_index=0;string_index<2;string_index++)
        {
                LCDPrintString(StringsSubmenuTipoCurvLav[RamSettings.Linguaggio][string_index],12,strings_y);
                strings_y+=12;
        }

        LCD_CopyScreen();

        while(loop_flag)
        {
                key_getstroke(&key,portMAX_DELAY);
                if (key == KEY_PROG)
                
                {
                        
                        MenuFunction_Index=SUBMENU_SELECTED_PROGR;
                        loop_flag=0;

                }

                if (key == KEY_OK)
                //if(CHECK_TASTO_OK_PRESSED)
                {
                        
                        RamSettings.ptype_arr[selected_progr].curva_lav_cal_type=submenuCurLavType_index;
                        test=SaveRamSettings_in_External_DataFlash();
                        if(!test)
                        {
                              LCD_Fill_ImageRAM(0x00);
                              SelectFont(CALIBRI_10);
                              LCDPrintString("File system error",4,24);
                              LCD_CopyPartialScreen(4,80,24,36);
                        }

                        DisegnaMarker(102,menu_triang_y,y_old);
                        y_old=menu_triang_y;
                        if(submenuCurLavType_index==CALIBR_CENTR)MenuFunction_Index=SUB3MENU_CURVA_DI_LAVORO;
                        else					 MenuFunction_Index=SUB3MENU_SEL_LCH;

                        loop_flag=0;
                }


                if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
                {
                        if (key == KEY_DOWNRIGHT)
                        //if(CHECK_TASTO_DN_DX_PRESSED)
                        {
                                MoveTriangolinoDown();
                                if(submenuCurLavType_index<5)submenuCurLavType_index+=1;
                                
                        }

                        if (key == KEY_UPLEFT)
                        //if(CHECK_TASTO_UP_SX_PRESSED)
                        {
                                MoveTriangolinoUp();
                                if(submenuCurLavType_index)submenuCurLavType_index-=1;
                                
                        }
                }

                //
                {



                }
        }
}
//***************************************************************************************
void Sub2Sel_L_C_H(void)
{
	uint8_t key/*,last_key*/;
        unsigned short string_index=0;
	unsigned short menu_L_C_H=0;
	unsigned char loop_flag=1,test;
	unsigned char to_print=1;
	unsigned char *L_index;
        unsigned char *C_index;
        unsigned char *H_index;
        
        //unsigned int sel_progr_num=RamSettings.selected_program_id;
        unsigned int un_misura=PROGR_IN_USO.unita_mis_concentr;

	L_index=&PROGR_IN_USO.curva_lav_L_index;
	C_index=&PROGR_IN_USO.curva_lav_C_index;
	H_index=&PROGR_IN_USO.curva_lav_H_index;

	LCD_Fill_ImageRAM(0x00);
	SelectFont(CALIBRI_10);
        
	LCDPrintString("L=",68,2);
	LCDPrintString("C=",68,26);
	LCDPrintString("H=",68,50);

	LCDPrintString("0x",8,2);
	LCDPrintString("0x",8,26);
	LCDPrintString("0x",8,50);

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
        menu_triang_limit_dx=60;
        menu_triang_limit_sx=0;
	menu_triang_y=menu_triang_limit_up;
        menu_triang_index=0;
	DisegnaTriangolinoMenu(0,menu_triang_y);
        
        LCD_CopyScreen();
        
        MARK_PIU_MENO_ENABLED;

    while(loop_flag)
    {
    	//key_getstroke(&key,portMAX_DELAY);
        if (key_getstroke(&key, kDec*2))
			//last_key = key;
	
          
          
          
        if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
        {
               
          if(key==KEY_DOWNRIGHT)
          {
                  MoveTriangolinoDown();
                  MoveTriangolinoDown();
                  if(menu_L_C_H<15)menu_L_C_H+=1;
                  
          }

          if(key==KEY_UPLEFT)
          {
                  MoveTriangolinoUp();
                  MoveTriangolinoUp();
                  if(menu_L_C_H)menu_L_C_H-=1;
                  
          }
        }
        else//cioè 
        {
                
                
        }

    	if(CHECK_PIU_MENO_ENABLED)
        {
            if(key==KEY_PLUS)
            {

                    if(menu_triang_x==menu_triang_limit_dx)//se devo variare il valore
                    {
                            incr_counter++;
                            if(incr_counter>10)
                            {
                                    incr_step=10;
                            }
                            if(incr_counter>20)
                            {
                                    incr_step=100;
                                    incr_counter=21;
                            }


                            string_index=menu_triang_index;

                             if(menu_triang_index==0)
                            {  
                              IncrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_L_index],incr_step);
                              CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,menu_triang_y);
                              
                            }
                            
                           if(menu_triang_index==2)
                            {  
                              IncrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index],incr_step);
                              CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,menu_triang_y);
                            }
                            
                           if(menu_triang_index==4)
                            {  
                               IncrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index],incr_step);
                               CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,menu_triang_y);
                            } 
                            
                            to_print=1;
                    }
                    else
                    {  
                      if(menu_triang_x==menu_triang_limit_sx)//se devo variare la posizione
                      {
                        incr_step=1;
                        if(menu_triang_index==0)
                        {  
                          if(*L_index<*C_index)*L_index+=incr_step;
                        }
                        
                       if(menu_triang_index==2)
                        {  
                           if(*C_index<*H_index)*C_index+=incr_step;
                        }
                        
                       if(menu_triang_index==4)
                        {  
                           if(*H_index<15)*H_index+=incr_step;
                        } 
                        
                        to_print=1;
                        
                      } 
                  } 
            }

            if(key == (KEY_PLUS | KEY_RELEASED))
            {
                    incr_step=1;
                    incr_counter=0;
                    
            }




            if(key == KEY_MINUS)
            {

                    if(menu_triang_x==menu_triang_limit_dx)//se devo variare il valore
                    {
                            incr_counter++;
                            if(incr_counter>10)
                            {
                                    incr_step=10;
                            }
                            if(incr_counter>20)
                            {
                                    incr_step=100;
                                    incr_counter=21;
                            }


                            string_index=menu_triang_index;

                            //sistemare...per step>1 può scendere sotto 0
                                                  
                            if(menu_triang_index==0)
                            {  
                              DecrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_L_index],incr_step);
                              CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,menu_triang_y);
                              
                            }
                            
                           if(menu_triang_index==2)
                            {  
                              DecrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index],incr_step);
                              CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,menu_triang_y);
                            }
                            
                           if(menu_triang_index==4)
                            {  
                               DecrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index],incr_step);
                               CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,menu_triang_y);
                            } 
                            
                            to_print=1;
                            
                            
                            
                         
                    }
                    else
                    {  
                      if(menu_triang_x==menu_triang_limit_sx)//se devo variare la posizione
                      {
                        incr_step=1;
                        if(menu_triang_index==0)
                        {  
                          if(*L_index)*L_index-=incr_step;
                        }
                        
                       if(menu_triang_index==2)
                        {  
                           if(*C_index>*L_index)*C_index-=incr_step;
                        }
                        
                       if(menu_triang_index==4)
                        {  
                           if(*H_index>*C_index)*H_index-=incr_step;
                        } 
                        
                        to_print=1;
                        
                      }  
                    } 
                    
                }

                if(key == (KEY_MINUS | KEY_RELEASED))
                {
                        incr_step=1;
                        incr_counter=0;

                }
                
                if (key == KEY_PROG)
                
                {
                        
                        MenuFunction_Index=SUB2MENU_SEL_TIPO_CURV_LAV;
                        loop_flag=0;

                }
        }
        
        
        
        if(key == KEY_OK)
	{
	       
               
               if(menu_triang_x==menu_triang_limit_sx)//se devo variare la posizione
               {
                  MoveTriangolinoDx();
          
               }
               else
               {  
               
               
                   if(menu_triang_x==menu_triang_limit_dx)//se devo variare la posizione
                   {
                        
                       RicalcolaCurvaLavoro3pt();
                       if(measures.temp_ok)
                       {
                            measures.temp_ok=0;
                            
                            

                            if(menu_triang_index==0)
                            {  
                            PROGR_IN_USO.curva_lav_XconducL=measures.conduc;
                            }

                            if(menu_triang_index==2)
                            {  
                             PROGR_IN_USO.curva_lav_XconducC=measures.conduc;
                             PROGR_IN_USO.temp_acq_curva_lav=measures.temp_resist;
                             Convers_Res_to_Temp(&PROGR_IN_USO.temp_acq_curva_lav);//NB questa funzione modifica il parametro stesso che le viene passato
                            }

                            if(menu_triang_index==4)
                            {  
                             PROGR_IN_USO.curva_lav_XconducH=measures.conduc;
                            } 
                            MoveTriangolinoSx();
                            
                            MenuFunction_Index=SUB3MENU_CURVA_DI_LAVORO3pt;
                            
                            test=SaveRamSettings_in_External_DataFlash();
                            if(!test)
                            {
                                LCD_Fill_ImageRAM(0x00);
                                SelectFont(CALIBRI_10);
                                LCDPrintString("File system error",4,24);
                                LCD_CopyPartialScreen(4,80,24,36);
                            }
                            return;
  
                        
                        
                        }
                     
                     
                     
                     
                     
                        
              
                   }
               }
               
	}



	if(to_print)
	{
		 to_print=0; 
                 
                 CleanArea_Ram_and_Screen(20,40,2,12);
                 BinToBCDisp(*L_index,INTERO,8,2);
                 LCD_CopyPartialScreen(20,40,2,12);
                 
                 CleanArea_Ram_and_Screen(20,40,26,36);
                 BinToBCDisp(*C_index,INTERO,8,26);
                 LCD_CopyPartialScreen(20,40,26,36);
                 
                 CleanArea_Ram_and_Screen(20,40,50,60);
                 BinToBCDisp(*H_index,INTERO,8,50);
                 LCD_CopyPartialScreen(20,40,50,60);
                 
                 //scelgo funzione da puntare a seconda dell unità misura concentrazione 
                 CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[*L_index],80,2);
                 CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[*C_index],80,26);
                 CalcPrint_UnMisura_Conc[un_misura](PROGR_IN_USO.curva_lav_Yconcent[*H_index],80,50);
         
	}
    }

	
}
//***************************************************************************************
void Sub2MenuCurvadiLavoro(void)  
{
	uint8_t key,last_key;
	unsigned short string_index=0,strings_y=2;
	unsigned short menu_CurvaLavoro_index=0;
	unsigned short page=0,page_old;
	unsigned char loop_flag=1,test;
	unsigned char first_string_to_print=0,last_string_to_print;
	unsigned char to_print=1,resto;
        //unsigned int sel_progr_num=RamSettings.selected_program_id;
        unsigned int un_mis_conc;
        
        unsigned int prova=80;
	//unsigned char blink_enable=0,toggler = 0;

	//menu_CurvaLavoro_index=PROGR_IN_USO.curva_lav_C_index;
        menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_limit_dx=74;
	menu_triang_y=2+(menu_CurvaLavoro_index*H_RIGA_CALIBRI10);
	menu_triang_index=menu_CurvaLavoro_index;
	incr_step=1;
        
        
        un_mis_conc=PROGR_IN_USO.unita_mis_concentr;
        
        RicalcolaCurvaLavoro();

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key, kDec*2))
			last_key = key;
		else
		{
			//if (blink_enable)DisegnaCarattereBlink('R',40,menu_triang_y,&toggler);
			//key = 0;
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
                        else  //se quindi confermo la scelta
                        {
                                MoveTriangolinoSx();
                                MARK_ARROW_KEYS_MOVE_UPDOWN;
                                CLEAR_PIU_MENO_ENABLED;
                                PROGR_IN_USO.curva_lav_C_index=page*5+menu_triang_index;
                                
                                RicalcolaCurvaLavoro();
                                if(measures.temp_ok)
                                {
                                    measures.temp_ok=0;
                                    
                                
                                     PROGR_IN_USO.curva_lav_XconducC=measures.conduc;
                                     PROGR_IN_USO.temp_acq_curva_lav=measures.temp_resist;
                                     Convers_Res_to_Temp(&PROGR_IN_USO.temp_acq_curva_lav);//NB questa funzione modifica il parametro stesso che le viene passato
                                    
                               
                                }
                                
                                test=SaveRamSettings_in_External_DataFlash();
                                if(!test)
                                {
                                      LCD_Fill_ImageRAM(0x00);
                                      SelectFont(CALIBRI_10);
                                      LCDPrintString("File system error",4,24);
                                      LCD_CopyPartialScreen(4,80,24,36);
                                }

                                


                                to_print=1;
                        }
                }

                if (key == KEY_PROG)
                
                {
                        
                        MenuFunction_Index=SUB2MENU_SEL_TIPO_CURV_LAV;
                        loop_flag=0;

                }

                if(CHECK_PIU_MENO_ENABLED)
                {
                        if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
                        //if(CHECK_TASTO_PLUS_PRESSED)
                        {

                                if(menu_triang_x==menu_triang_limit_dx)
                                {
                                        incr_counter++;
                                        if(incr_counter>10) incr_step=10;
                                        if(incr_counter>20)
                                        {
                                                incr_step=100;
                                                incr_counter=21;
                                        }

                                        string_index=menu_triang_index+ page*5;
                                        //incremento

                                        IncrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[string_index],incr_step);
                                        CalcPrint_UnMisura_Conc[un_mis_conc](PROGR_IN_USO.curva_lav_Yconcent[string_index],prova,menu_triang_y);
                                        LCD_CopyPartialScreen(92,120,menu_triang_y,menu_triang_y+10);
                                }
                        }
                        else if (key == (KEY_PLUS | KEY_RELEASED))
                        
                        {
                                incr_step=1;
                                incr_counter=0;
                                
                        }

                        if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
                        //if(CHECK_TASTO_MENO_PRESSED)
                        {

                                if(menu_triang_x==menu_triang_limit_dx)
                                {
                                        incr_counter++;
                                        if(incr_counter>10)
                                        {
                                                incr_step=10;
                                        }
                                        if(incr_counter>20)
                                        {
                                                incr_step=100;
                                                incr_counter=21;
                                        }


                                        string_index=menu_triang_index+ page*5;

                                        //sistemare...per step>1 può scendere sotto 0
                                        DecrParamConc(&PROGR_IN_USO.curva_lav_Yconcent[string_index],incr_step);
                                        CalcPrint_UnMisura_Conc[un_mis_conc](PROGR_IN_USO.curva_lav_Yconcent[string_index],prova,menu_triang_y);
                                        LCD_CopyPartialScreen(92,120,menu_triang_y,menu_triang_y+10);
                                        
                                }
                        }
                        else if (key == (KEY_MINUS | KEY_RELEASED))
                        //if(CHECK_TASTO_MENO_RELEASED)
                        {
                                incr_step=1;
                                incr_counter=0;

                        }
                }

                if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
                {
                        if (key == KEY_DOWNRIGHT)
                        //if(CHECK_TASTO_DN_DX_PRESSED)
                        {
                                MoveTriangolinoDown();
                                if(menu_CurvaLavoro_index<15)menu_CurvaLavoro_index+=1;
                                page=(menu_CurvaLavoro_index)/5;

                                if(page!=page_old)
                                {
                                        to_print=1;
                                        if(page==3)menu_triang_limit_dn=2;
                                        else 	  menu_triang_limit_dn=50;
                                        menu_triang_index=0;
                                        menu_triang_y=menu_triang_limit_up;
                                }
                                else              to_print=0;


                                
                        }

                        if (key == KEY_UPLEFT)
                        //if(CHECK_TASTO_UP_SX_PRESSED)
                        {
                                MoveTriangolinoUp();
                                if(menu_CurvaLavoro_index)menu_CurvaLavoro_index-=1;

                                page=(menu_CurvaLavoro_index)/5;

                                if(page!=page_old)
                                {
                                        to_print=1;
                                        if(page==3)menu_triang_limit_dn=2;
                                        else 	  menu_triang_limit_dn=50;
                                        menu_triang_index=4;
                                        menu_triang_y=menu_triang_limit_dn;

                                }
                                else              to_print=0;

                                
                        }
                }
                else
                {
                        
                }
		



		if(to_print)
		{
			to_print=0;

			switch(page)
			{
				case 0:
					first_string_to_print=0;
					last_string_to_print =5;//sarebbe 4

					break;

				case 1:
					first_string_to_print=5;
					last_string_to_print =10;//sarebbe 9

					break;

				case 2:
					first_string_to_print=10;
					last_string_to_print =15;//sarebbe 14

					break;

				case 3:
					first_string_to_print=15;
					last_string_to_print= 16;//sarebbe 15

					break;

				default:

					break;
			}

			LCD_Fill_ImageRAM(0x00);


			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;

			for(string_index=first_string_to_print;string_index < last_string_to_print;string_index++)
			{
				LCDPrintString(StringsSubmenuCurvaLavoro[string_index],10,strings_y);
                                
                                CalcPrint_UnMisura_Conc[un_mis_conc](PROGR_IN_USO.curva_lav_Yconcent[string_index],prova,strings_y);
        
				//BinToBCDisp(PROGR_IN_USO.curva_lavoro[string_index],DUE_DECIMALI,92,strings_y);
				//PrintUnitMis(string_index,118,strings_y);
				//LCDPrintString("%",118,strings_y);


				strings_y+=H_RIGA_CALIBRI10;
			}
			//uso la variabile page_old perchè tanto verrà aggiornata tra poco
			page_old=	PROGR_IN_USO.curva_lav_C_index / 5;
			resto=		PROGR_IN_USO.curva_lav_C_index % 5;//es scelgo 7,è nella pag 1 riga 2(0,1,2)
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("C",65,strings_y);
			}


			page_old=page;

			LCD_CopyScreen();

		}
	}
}//fine Sub2MenuCurvaDiLavoro();
/*
I menu curva di lavoro servono per inserire una concentrazione misurata e legarla alla conducibilità misurata
Quindi al momento dell'inserimento della concentrazione va aanche fatta partire l'acquisizione di conducibilità che andrà salvata

*/
//***************************************************************************************
void Sub2MenuCurvadiLavoro3Punti(void)
{
	uint8_t key,last_key;
	unsigned short string_index=0,strings_y=2;
	unsigned short menu_CurvaLavoro_index=0;
	unsigned short page=0,page_old;
	unsigned char loop_flag=1;
	unsigned char first_string_to_print=0,last_string_to_print;
	unsigned char to_print=1,resto,test;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_limit_dx=74;
	menu_triang_y=2+(menu_CurvaLavoro_index*H_RIGA_CALIBRI10);
	menu_triang_index=menu_CurvaLavoro_index;
	incr_step=1;
        
        unsigned char un_mis_conc=PROGR_IN_USO.unita_mis_concentr;

        RicalcolaCurvaLavoro3pt();
        
	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,kDec * 2))
			last_key = key;
		else
			key = 0;
		
		
		{
			

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				

				switch(menu_triang_y)
				{
				}

				if(menu_triang_x==0)
				{
					MoveTriangolinoDx();
					//non marcare MOVE_SXDX!!
					CLEAR_ARROW_KEYS_MOVE_UPDOWN;
					MARK_PIU_MENO_ENABLED;
				}
				else  //se quindi confermo la scelta
				{
					MoveTriangolinoSx();
					MARK_ARROW_KEYS_MOVE_UPDOWN;
					
					CLEAR_PIU_MENO_ENABLED;
					PROGR_IN_USO.curva_lav_C_index=page*5+menu_triang_index;

					RicalcolaCurvaLavoro();
                                        
                                        test=SaveRamSettings_in_External_DataFlash();
                                        if(!test)
                                        {
                                        LCD_Fill_ImageRAM(0x00);
                                        SelectFont(CALIBRI_10);
                                        LCDPrintString("File system error",4,24);
                                        LCD_CopyPartialScreen(4,80,24,36);
                                        }

					to_print=1;
				}
			}

			if (key == KEY_PROG)
			
			{
				
				MenuFunction_Index=SUB3MENU_SEL_LCH;
				loop_flag=0;

			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
				//if(CHECK_TASTO_PLUS_PRESSED)
				{

					
				}
				else if (key == (KEY_PLUS | KEY_RELEASED))
				
				{
					incr_step=1;
					incr_counter=0;
					
				}

				if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
				//if(CHECK_TASTO_MENO_PRESSED)
				{

					
				}
				else if (key == (KEY_MINUS | KEY_RELEASED))
				//if(CHECK_TASTO_MENO_RELEASED)
				{
					incr_step=1;
					incr_counter=0;

				}

			}

			if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(menu_CurvaLavoro_index<15)menu_CurvaLavoro_index+=1;
					page=(menu_CurvaLavoro_index)/5;

					if(page!=page_old)
					{
						to_print=1;
						if(page==3)menu_triang_limit_dn=2;
						else 	  menu_triang_limit_dn=50;
						menu_triang_index=0;
						menu_triang_y=menu_triang_limit_up;
					}
					else              to_print=0;


					
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(menu_CurvaLavoro_index)menu_CurvaLavoro_index-=1;

					page=(menu_CurvaLavoro_index)/5;

					if(page!=page_old)
					{
						to_print=1;
						if(page==3)menu_triang_limit_dn=2;
						else 	  menu_triang_limit_dn=50;
						menu_triang_index=4;
						menu_triang_y=menu_triang_limit_dn;

					}
					else              to_print=0;

					
				}
			}
			else//cioè 
			{
				//if(CHECK_TASTO_DN_DX_PRESSED)
				//if(CHECK_TASTO_UP_SX_PRESSED)
			}
		}//fine if(CHECK_KEY_READY)



		if(to_print)
		{
			to_print=0;

			switch(page)
			{
				case 0:
					first_string_to_print=0;
					last_string_to_print =5;//sarebbe 4

					break;

				case 1:
					first_string_to_print=5;
					last_string_to_print =10;//sarebbe 9

					break;

				case 2:
					first_string_to_print=10;
					last_string_to_print =15;//sarebbe 14

					break;

				case 3:
					first_string_to_print=15;
					last_string_to_print= 16;//sarebbe 15

					break;

				default:

					break;
			}

			LCD_Fill_ImageRAM(0x00);

#ifdef DISEGNA_CORNICE
			DisegnaCornice();
#endif
			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;

			for(string_index=first_string_to_print;string_index < last_string_to_print;string_index++)
			{
				LCDPrintString(StringsSubmenuCurvaLavoro[string_index],10,strings_y);
                                CalcPrint_UnMisura_Conc[un_mis_conc](PROGR_IN_USO.curva_lav_Yconcent[string_index],80,strings_y);

				strings_y+=H_RIGA_CALIBRI10;
			}
			//uso la variabile page_old perchè tanto verrà aggiornata tra poco
			page_old=	PROGR_IN_USO.curva_lav_L_index / 5;
			resto=		PROGR_IN_USO.curva_lav_L_index % 5;
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("L",74,strings_y);
			}

			page_old=	PROGR_IN_USO.curva_lav_C_index / 5;
			resto=		PROGR_IN_USO.curva_lav_C_index % 5;
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("C",74,strings_y);
			}

			page_old=	PROGR_IN_USO.curva_lav_H_index / 5;
			resto=		PROGR_IN_USO.curva_lav_H_index % 5;
			if(page==page_old)
			{
				strings_y=(resto*H_RIGA_CALIBRI10)+2;
				LCDPrintString("H",74,strings_y);
			}


			page_old=page;

			LCD_CopyScreen();

		}
	}
}//fine Sub2MenuCurvaDiLavoro3Punti();
//***************************************************************************************
void Sub2MenuImpostaSoglie(void)
{
	uint8_t key,last_key;
	unsigned short string_index=0,strings_y=2;
	static unsigned short menu_ImpostaSoglie_index=0;
	static unsigned short scroll_old=0;
	unsigned char loop_flag=1,test;
	unsigned char first_string_to_print,last_string_to_print;
	unsigned char to_print=1;
        //unsigned int sel_progr_num=RamSettings.selected_program_id;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_limit_dx=76;
	menu_triang_y=2+(menu_ImpostaSoglie_index*H_RIGA_CALIBRI10);
	menu_triang_index=menu_ImpostaSoglie_index;
	incr_step=1;

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,kDec*2))
			last_key = key;
		else	key = 0;
		
		
			
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
                                
                                
                                test=SaveRamSettings_in_External_DataFlash();
                                if(!test)
                                {
                                      LCD_Fill_ImageRAM(0x00);
                                      SelectFont(CALIBRI_10);
                                      LCDPrintString("File system error",4,24);
                                      LCD_CopyPartialScreen(4,80,24,36);
                                }

                        }
                }

                if (key == KEY_PROG)
                
                {
                        
                        MenuFunction_Index=SUBMENU_SELECTED_PROGR;
                        loop_flag=0;

                }

                if(CHECK_PIU_MENO_ENABLED)
                {
                        if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
                        //if(CHECK_TASTO_PLUS_PRESSED)
                        {

                                if(menu_triang_x==menu_triang_limit_dx)
                                {
                                        incr_counter++;
                                        if(incr_counter>10) incr_step=10;
                                        if(incr_counter>20)
                                        {
                                                incr_step=100;
                                                incr_counter=21;
                                        }


                                        string_index=menu_triang_index;
                                        IncrSoglia(string_index,incr_step);
                                        //LCDPrintString(StringsSubmenuImpostaSoglie[string_index],10,strings_y);
                                        CleanArea_Ram_and_Screen(82,110,menu_triang_y,menu_triang_y+10);
                                        PrintSoglia(string_index,82,menu_triang_y);
                                        LCD_CopyPartialScreen(82,110,menu_triang_y,menu_triang_y+10);
                                        //PrintUnitMis(string_index,112,strings_y);
                                }
                        }
                        else if (key == (KEY_PLUS | KEY_RELEASED))
                        
                        {
                                incr_step=1;
                                incr_counter=0;
                                
                        }

                        if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
                        //if(CHECK_TASTO_MENO_PRESSED)
                        {

                                if(menu_triang_x==menu_triang_limit_dx)
                                {
                                        incr_counter++;
                                        if(incr_counter>10)
                                        {
                                                incr_step=10;
                                        }
                                        if(incr_counter>20)
                                        {
                                                incr_step=100;
                                                incr_counter=21;
                                        }


                                        string_index=menu_triang_index;
                                        DecrSoglia(string_index,incr_step);
                                        //LCDPrintString(StringsSubmenuImpostaSoglie[string_index],10,strings_y);
                                        CleanArea_Ram_and_Screen(82,110,menu_triang_y,menu_triang_y+10);
                                        PrintSoglia(string_index,82,menu_triang_y);
                                        LCD_CopyPartialScreen(82,110,menu_triang_y,menu_triang_y+10);
                                        //PrintUnitMis(string_index,112,strings_y);
                                }
                        }
                        else if (key == (KEY_MINUS | KEY_RELEASED))
                        //if(CHECK_TASTO_MENO_RELEASED)
                        {
                                incr_step=1;
                                incr_counter=0;

                        }

                }

                if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
                {
                        if (key == KEY_DOWNRIGHT)
                        //if(CHECK_TASTO_DN_DX_PRESSED)
                        {
                                MoveTriangolinoDown();
                                if(menu_ImpostaSoglie_index<7)menu_ImpostaSoglie_index+=1;
                                if(menu_ImpostaSoglie_index>4)//lo scroll in basso serve solo nei casi che debba mostrare oltre la 5a stringa
                                {
                                        if(scroll_old==menu_ImpostaSoglie_index)to_print=0;
                                        else                           to_print=1;

                                }
                                
                        }

                        if (key == KEY_UPLEFT)
                        //if(CHECK_TASTO_UP_SX_PRESSED)
                        {
                                MoveTriangolinoUp();
                                if(menu_ImpostaSoglie_index)menu_ImpostaSoglie_index-=1;
                                if(menu_ImpostaSoglie_index<3)//lo scroll in alto può servire se sono prima della 4 stringa
                                {
                                        if(scroll_old==menu_ImpostaSoglie_index)to_print=0;
                                        else                           to_print=1;
                                }
                                
                        }
                }
                else//cioè 
                {



                }
		



		if(to_print)
		{
			to_print=0;

			if(menu_ImpostaSoglie_index==0)
			{
				first_string_to_print=0;
			}

			if(menu_ImpostaSoglie_index==1)
			{
				first_string_to_print=1;

			}

			if(menu_ImpostaSoglie_index==2)
			{
				first_string_to_print=2;
			}

			if(menu_ImpostaSoglie_index==5)
			{
				first_string_to_print=1;
			}

			if(menu_ImpostaSoglie_index==6)
			{
				first_string_to_print=2;
			}

			if(menu_ImpostaSoglie_index==7)
			{
				first_string_to_print=3;
			}

			LCD_Fill_ImageRAM(0x00);


			DisegnaTriangolinoMenu(0,menu_triang_y);
			SelectFont(CALIBRI_10);

			strings_y=2;
			last_string_to_print=first_string_to_print + 5;
			for(string_index=first_string_to_print;string_index<last_string_to_print;string_index++)
			{
				LCDPrintString(StringsSubmenuImpostaSoglie[RamSettings.Linguaggio][string_index],10,strings_y);
				PrintSoglia(string_index,82,strings_y);
				PrintUnitMis(string_index,112,strings_y);


				strings_y+=H_RIGA_CALIBRI10;
			}


			LCD_CopyScreen();
			scroll_old=menu_ImpostaSoglie_index;
		}



	}

}
//***************************************************************************************
void Sub2MenuImpostaTimer(void)
{
  uint8_t key,last_key;
  unsigned char loop_flag=1,to_print=1,resto,test;
  unsigned short page=0,page_old;
  unsigned short string_index=0,strings_y=2;
  unsigned short menu_ImpostaTimers_index=0;
  unsigned char first_string_to_print,last_string_to_print;
  unsigned int prova=80;
  

  menu_triang_limit_up=2;
  menu_triang_limit_dn=50;
  menu_triang_limit_dx=74;
  menu_triang_y=2+(menu_ImpostaTimers_index*H_RIGA_CALIBRI10);
  menu_triang_index=menu_ImpostaTimers_index;
  incr_step=1;

  LCD_Fill_ImageRAM(0x00);
 
  SelectFont(CALIBRI_10);
  
  
 
  key = last_key = 0;
  while(loop_flag)
  {
      //if (key_getstroke(&key,portMAX_DELAY) && (key == KEY_PROG))
        if (key_getstroke(&key,kDec*2))
			last_key = key;
		else	key = 0;
                
                if (key_getstroke(&key,kDec*2))
			last_key = key;
		else	key = 0;
		
      
    if (key == KEY_PROG)
    {
            
            MenuFunction_Index=SUBMENU_SELECTED_PROGR;
            loop_flag=0;

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
                      
                      
                      test=SaveRamSettings_in_External_DataFlash();
                      if(!test)
                      {
                            LCD_Fill_ImageRAM(0x00);
                            SelectFont(CALIBRI_10);
                            LCDPrintString("File system error",4,24);
                            LCD_CopyPartialScreen(4,80,24,36);
                      }

              }
      }
      
      if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
      {
              if (key == KEY_DOWNRIGHT)
              //if(CHECK_TASTO_DN_DX_PRESSED)
              {
                      MoveTriangolinoDown();
                      if(menu_ImpostaTimers_index<15)menu_ImpostaTimers_index+=1;
                      page=(menu_ImpostaTimers_index)/5;

                      if(page!=page_old)
                      {
                              to_print=1;
                              if(page==3)menu_triang_limit_dn=2;
                              else 	  menu_triang_limit_dn=50;
                              menu_triang_index=0;
                              menu_triang_y=menu_triang_limit_up;
                      }
                      else              to_print=0;


                      
              }

              if (key == KEY_UPLEFT)
              //if(CHECK_TASTO_UP_SX_PRESSED)
              {
                      MoveTriangolinoUp();
                      if(menu_ImpostaTimers_index)menu_ImpostaTimers_index-=1;

                      page=(menu_ImpostaTimers_index)/5;

                      if(page!=page_old)
                      {
                              to_print=1;
                              if(page==3)menu_triang_limit_dn=2;
                              else 	  menu_triang_limit_dn=50;
                              menu_triang_index=4;
                              menu_triang_y=menu_triang_limit_dn;

                      }
                      else              to_print=0;

                      
              }
      }
      else
      {
              
      }
	
      if(CHECK_PIU_MENO_ENABLED)
      {
              if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
              //if(CHECK_TASTO_PLUS_PRESSED)
              {

                      if(menu_triang_x==menu_triang_limit_dx)
                      {
                              incr_counter++;
                              if(incr_counter>10) incr_step=10;
                              if(incr_counter>20)
                              {
                                      incr_step=100;
                                      incr_counter=21;
                              }

                              string_index=menu_triang_index+ page*5;
                              //incremento
                              prova=PROGR_IN_USO.Timers.Timers_values[string_index];
                              if((prova+incr_step)<TIMERS_MAX_VAL)
                              {
                                CleanArea_Ram_and_Screen(92,120,menu_triang_y,menu_triang_y+10);
                                prova+=incr_step;
                                PROGR_IN_USO.Timers.Timers_values[string_index]=prova;
                                BinToBCDisp(PROGR_IN_USO.Timers.Timers_values[string_index],INTERO,96,menu_triang_y);
                                LCD_CopyPartialScreen(92,120,menu_triang_y,menu_triang_y+10);
                              }
                              
                              
                              
                      }
              }
              else if (key == (KEY_PLUS | KEY_RELEASED))
              
              {
                      incr_step=1;
                      incr_counter=0;
                      
              }

              if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
              //if(CHECK_TASTO_MENO_PRESSED)
              {

                      if(menu_triang_x==menu_triang_limit_dx)
                      {
                              incr_counter++;
                              if(incr_counter>10)
                              {
                                      incr_step=10;
                              }
                              if(incr_counter>20)
                              {
                                      incr_step=100;
                                      incr_counter=21;
                              }


                              string_index=menu_triang_index+ page*5;

                              //sistemare...per step>1 può scendere sotto 0
                              prova=PROGR_IN_USO.Timers.Timers_values[string_index];
                              if((prova-incr_step)>0)
                              {
                                prova-=incr_step;
                                CleanArea_Ram_and_Screen(92,120,menu_triang_y,menu_triang_y+10);
                                PROGR_IN_USO.Timers.Timers_values[string_index]=prova;
                                BinToBCDisp(PROGR_IN_USO.Timers.Timers_values[string_index],INTERO,96,menu_triang_y);
                                LCD_CopyPartialScreen(92,120,menu_triang_y,menu_triang_y+10);
                              }
                              
                      }
              }
              else if (key == (KEY_MINUS | KEY_RELEASED))
              //if(CHECK_TASTO_MENO_RELEASED)
              {
                      incr_step=1;
                      incr_counter=0;

              }
      }

   
      if(to_print)
      {
        to_print=0;
        switch(page)
      {
        case 0:
                first_string_to_print=0;
                last_string_to_print =5;//sarebbe 4
                menu_triang_limit_dn=50;
                break;

        case 1:
                first_string_to_print=5;
                last_string_to_print =8;//sarebbe 9
                menu_triang_limit_dn=26;
                break;

        default:
            break;
        }
        
        if(page!=page_old)
        {
          LCD_Fill_ImageRAM(0x00);
          strings_y=2;
        }
        
        DisegnaTriangolinoMenu(0,menu_triang_y);
        for(string_index=first_string_to_print;string_index<last_string_to_print;string_index++)
        {
            LCDPrintString(StringsSubmenuImpostaTimer[RamSettings.Linguaggio][string_index],10,strings_y);
            BinToBCDisp(PROGR_IN_USO.Timers.Timers_values[string_index],INTERO,96,strings_y);
            LCDPrintString("s",122,strings_y);
            strings_y+=H_RIGA_CALIBRI10;
                
        }
        
        page_old=	PROGR_IN_USO.curva_lav_C_index / 5;
        resto=		PROGR_IN_USO.curva_lav_C_index % 5;//es scelgo 7,è nella pag 1 riga 2(0,1,2)
        if(page==page_old)
        {
                strings_y=(resto*H_RIGA_CALIBRI10)+2;
                
        }


        page_old=page;
        
        LCD_CopyScreen();
        
        
        
        
      }
          
          
  }

}
//***************************************************************************************
void Sub2MenuImpostaSimboli(void)
{
	uint8_t key;

	unsigned short string_index=0,strings_y=2,y_old;


	unsigned short submenuImpostaSimboli_index;
	unsigned char loop_flag=1,test;

	submenuImpostaSimboli_index=PROGR_IN_USO.unita_mis_concentr;

	loop_flag=1;

	menu_triang_limit_up=2;
	menu_triang_limit_dn=50;
	menu_triang_y=2+(H_RIGA_CALIBRI10*submenuImpostaSimboli_index);
	y_old=menu_triang_y;


	LCD_Fill_ImageRAM(0x00);

	DisegnaMarker(80,menu_triang_y,y_old);
#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif
	DisegnaTriangolinoMenu(0,menu_triang_y);


	SelectFont(CALIBRI_10);
	for(string_index=0;string_index<5;string_index++)
	{
		LCDPrintString(StringsSubmenuImpostaSimboli[RamSettings.Linguaggio][string_index],H_RIGA_CALIBRI10,strings_y);
		strings_y+=H_RIGA_CALIBRI10;
	}

	LCD_CopyScreen();

	while(loop_flag)
	{
		key_getstroke(&key,portMAX_DELAY);




		if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
		{
			if (key == KEY_DOWNRIGHT)
			//if(CHECK_TASTO_DN_DX_PRESSED)
			{
				MoveTriangolinoDown();
				if(submenuImpostaSimboli_index<4)submenuImpostaSimboli_index+=1;
				else				 submenuImpostaSimboli_index=5;
				
			}

			if (key == KEY_UPLEFT)
			//if(CHECK_TASTO_UP_SX_PRESSED)
			{
				MoveTriangolinoUp();
				if(submenuImpostaSimboli_index)submenuImpostaSimboli_index-=1;
				else 						   submenuImpostaSimboli_index =0;
				
			}
		}
		else//cioè 
		{



		}

		if (key == KEY_OK)
		//if(CHECK_TASTO_OK_PRESSED)
		{
			PROGR_IN_USO.unita_mis_concentr=submenuImpostaSimboli_index;
			
                        test=SaveRamSettings_in_External_DataFlash();
                        if(!test)
                        {
                              LCD_Fill_ImageRAM(0x00);
                              SelectFont(CALIBRI_10);
                              LCDPrintString("File system error",4,24);
                              LCD_CopyPartialScreen(4,80,24,36);
                        }
			DisegnaMarker(80,menu_triang_y,y_old);//il 3° parametro è la y del triangolino da cancellare

			y_old=menu_triang_y;
			
		}

		if (key == KEY_PROG)
		
		{
			
			MenuFunction_Index=SUBMENU_SELECTED_PROGR;
			loop_flag=0;

		}
	}
}
//***************************************************************************************
void Sub2MenuTK(void)
{
	uint8_t key,last_key;
	unsigned char loop_flag=1,to_print=1,test;
	static unsigned short submenuTK_index=0;
	//unsigned int nuovo_TK=2345;

	LCD_Fill_ImageRAM(0x00);

	menu_triang_limit_up=14;
	menu_triang_limit_dn=26;
	menu_triang_limit_dx=76;
	menu_triang_limit_sx=0;

	menu_triang_y=menu_triang_limit_up+(H_RIGA_CALIBRI10*submenuTK_index);

	PROGR_IN_USO.TK.tk2.nuovo_TK=PROGR_IN_USO.TK.tk2.old_TK;
        


#ifdef DISEGNA_CORNICE
	DisegnaCornice();
#endif

	DisegnaTriangolinoMenu(0,menu_triang_y);

	LCDPrintString("Modifica Valore TK",	20,2);

	LCDPrintString("Attuale",20,14);
	LCDPrintString("Nuovo",	 20,26);

	LCD_CopyScreen();

	key = last_key = 0;
	while(loop_flag)
	{
		if (key_getstroke(&key,kDec*2))
			last_key = key;
		else
			key = 0;
		
		{
			//if(CHECK_ARROW_KEYS_MOVE_UPDOWN)
			{
				if (key == KEY_DOWNRIGHT)
				//if(CHECK_TASTO_DN_DX_PRESSED)
				{
					MoveTriangolinoDown();
					if(submenuTK_index<1)submenuTK_index+=1;
					else		     submenuTK_index=1;
					
				}

				if (key == KEY_UPLEFT)
				//if(CHECK_TASTO_UP_SX_PRESSED)
				{
					MoveTriangolinoUp();
					if(submenuTK_index)submenuTK_index-=1;
					else 		   submenuTK_index =0;
					
				}
			}
			//else//cioè 
			{



			}

			if (key == KEY_OK)
			//if(CHECK_TASTO_OK_PRESSED)
			{
				
                                if(menu_triang_y==menu_triang_limit_dn)
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

						PROGR_IN_USO.TK.tk2.old_TK=PROGR_IN_USO.TK.tk2.nuovo_TK;
                                                test=SaveRamSettings_in_External_DataFlash();
                                                if(!test)
                                                {
                                                      LCD_Fill_ImageRAM(0x00);
                                                      SelectFont(CALIBRI_10);
                                                      LCDPrintString("File system error",4,24);
                                                      LCD_CopyPartialScreen(4,80,24,36);
                                                }

					}
                            }
			}

			if(CHECK_PIU_MENO_ENABLED)
			{
				if ((key == KEY_PLUS) || (last_key == KEY_PLUS))
				//if(CHECK_TASTO_PLUS_PRESSED)
					{

						if(menu_triang_x==76)
						{
							incr_counter++;
							if(incr_counter>10) incr_step=10;
							if(incr_counter>20)
							{
								incr_step=100;
								incr_counter=21;
							}
							if(PROGR_IN_USO.TK.TK_array[submenuTK_index]<9999)
                                                        {
                                                              PROGR_IN_USO.TK.TK_array[submenuTK_index]+=incr_step;
                                                              to_print=1;
							}


						}
					}
					else if (key == (KEY_PLUS | KEY_RELEASED))
					
					{
						incr_step=1;
						incr_counter=0;
						
					}



					if ((key == KEY_MINUS) || (last_key == KEY_MINUS))
					//if(CHECK_TASTO_MENO_PRESSED)
					{

						if(menu_triang_x==76)
						{
							incr_counter++;
							if(incr_counter>10)
							{
								incr_step=10;
							}
							if(incr_counter>20)
							{
								incr_step=100;
								incr_counter=21;
							}


							if(!((PROGR_IN_USO.TK.TK_array[submenuTK_index]-incr_step)<0))
							{
								PROGR_IN_USO.TK.TK_array[submenuTK_index]-=incr_step;
								to_print=1;
							}
						}
					}
					else if (key == (KEY_MINUS | KEY_RELEASED))
					//if(CHECK_TASTO_MENO_RELEASED)
					{
						incr_step=1;
						incr_counter=0;

					}

				}

			if (key == KEY_PROG)
			
			{
				
				MenuFunction_Index=SUBMENU_SELECTED_PROGR;
				loop_flag=0;
			}


			if(to_print)
			{
				CleanArea_Ram_and_Screen(90,124,14,40);
				BinToBCDisp(PROGR_IN_USO.TK.tk2.old_TK,DUE_DECIMALI,90,14);
				BinToBCDisp(PROGR_IN_USO.TK.tk2.nuovo_TK  ,DUE_DECIMALI,90,26);
				LCD_CopyPartialScreen(90,124,14,40);

				to_print=0;
			}

		}
	}
}
//***************************************************************************************   
