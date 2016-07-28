/*
 * my_definitions.h
 *
 *  Created on: 02/mar/2016
 *      Author: Die go
 */

#ifndef SOURCES_MY_DEFINITIONS_H_
#define SOURCES_MY_DEFINITIONS_H_

#include "FontsCambria.h"

void Delay_5us(void);
void Delay_x10ms(unsigned char decine_ms);
void MySpecificBoardInit(void);
void CurveLavoroInit(void);
void LoadSectorImage(void);
void SaveImageInFlash(void);

typedef struct
{
	 unsigned short  start_x;
	 unsigned short  stop_x;
	 unsigned short  start_y;
	 unsigned short  stop_y;
	 unsigned short thick;
	 unsigned short grey_level;

}coordinate_struct_type;

typedef struct
{
	 unsigned char  righe;
	 unsigned char  colonne;
	 unsigned char  start_x;
	 unsigned char  start_y;
	 const unsigned char* bmp_pointer;
	 FONT_INFO* font_info_pt;
}bitmap_struct_type;


typedef struct
{
	 unsigned int  min_lowrange;
	 unsigned int  max_lowrange;
	 unsigned int  min_hirange;
	 unsigned int  max_hirange;
	 unsigned char   decim_lowrange;
	 unsigned char   decim_hirange;
}modo_concentrazione_type;






#define PROVA_SHIFT             provashift &=~(1<<9)


#define ADC_WAIT_FOR_INTERRUPT_ON_END 0

#define TRIANG_W	6
#define TRIANG_H	16

#define ABILITA 	1
#define DISABILITA 	0

#define PENULTIMO_INDICE_CURVA 14
#define ULTIMO_INDICE_CURVA    15


#define MENU_STAY 128

#define DUE_DECIMALI 2
#define UN_DECIMALE  1
#define INTERO       0


#define CURVA_LAVORO_1PT 0
#define CURVA_LAVORO_2PT 1
#define CURVA_LAVORO_3PT 2



#define H_RIGA_CALIBRI10 	     12
#define PRIMA_RIGA_CALIBRI10 	      2
#define TEMP_MAX_LIMIT		   1300
#define TEMP_MIN_LIMIT		     10

#define MAX_CONCENT_LIMIT_9990     9990
#define LIMITE_CAVO_APERTO_TEMP     180  // a 129°C leggo 149,a cavo aperto leggo 204

void SchermataDiLavoro(void);
void MenuProg(void);
void SubmenuINOUT(void);
void SubmenuSelProgr(void);
void SubmenuSelectedProgr(void);
void SubmenuComunic(void);
void SubmenuSetClock(void);
void SumMenuSelLingua(void);
void SubmenuServizio(void);
void Sub2Menu_Cal_PT100(void);
void Sub2Menu_Cal_Cable(void);
void Sub2Menu_ResSerie_Cable(void);
void Sub2Menu_MisuraDiretta(void);
void Sub2Menu_Licenza(void);


void Sub2MenuImpostaSimboli(void);
void Sub2MenuTK(void);
void Sub2MenuCurvadiLavoro(void);
void Sub2MenuCurvadiLavoro3Punti(void);
void Sub2MenuSelTipoCurvaLavoro(void);
void Sub2MenuImpostaSoglie(void);
void Sub2MenuImpostaTimer (void);
void Sub2Sel_L_C_H(void);

void PrintSoglia(unsigned short index,unsigned short x ,unsigned short y);
void IncrSoglia(unsigned short index,unsigned short incr);
void DecrSoglia(unsigned short index,unsigned short incr);//viene chiamata riga per riga quindi stampa solo 1 valore
void PrintUnitMis(unsigned short index,unsigned short x ,unsigned short y);
void IncrParamConc(unsigned int* par_pt,unsigned short incr);
void DecrParamConc(unsigned int* par_pt,unsigned short incr);


void MoveTriangolinoDown(void);
void MoveTriangolinoUp  (void);
void MoveTriangolinoDx(void);
void MoveTriangolinoSx(void);
void DisegnaTriangolinoMenu(unsigned short triang_x,unsigned short triang_y);
void DisegnaCornice (void);
void DisegnaMarker(unsigned short x,unsigned short y,unsigned short y_old);
void DisegnaOK(unsigned short x,unsigned short y,unsigned short y_old);

void PrintTimeDate(void);

void AumentaIncrDecrStep(int * step,int * counter);
void RiduciIncrDecrStep(int * step,int * counter);

unsigned char ControlloCongruita_CurvaLav(void);

void DisegnaCarattereBlink(char char_to_blink,unsigned short x,unsigned short y,unsigned char *toggler);
void RicalcolaCurvaLavoro(void);
void RicalcolaCurvaLavoro3pt(void);

void MenuInit(void);
void WriteMyFlashSector(void);





#define SCHERM_LAV  			0
#define MENU_PROGR 			1
#define SUBMENU_INOUT  			2
#define SUBMENU_SELEZIONA_PROG 		3
#define SUBMENU_COMUNIC			4
#define SUBMENU_SETCLOCK 		5
#define SUBMENU_SEL_LINGUA		6
#define SUBMENU_SERVIZIO  		7
#define SUBMENU_SELECTED_PROGR		8
#define SUB2MENU_CAL_PT100              9  
#define SUB2MENU_CABLE_COMPENS         10 
#define SUB2MENU_RES_SERIE_CAVO        11          
#define SUB2MENU_MISURA_DIRETTA        12  
#define SUB2MENU_LICENZA               13 


#define SUB2MENU_IMPOSTA_SIMBOLI	14
#define SUB2MENU_TK			15
#define SUB2MENU_SEL_TIPO_CURV_LAV      16
#define SUB2MENU_IMPOSTA_SOGLIE		17
#define SUB2MENU_IMPOSTA_TIMER		18
#define SUB3MENU_CURVA_DI_LAVORO	19
#define SUB3MENU_CURVA_DI_LAVORO3pt	20
#define SUB3MENU_SEL_LCH		21




#define SPOSTA_TRIANGOLINO  10

#define UNIT_MIS_CONCENTR_PERCENTUALE 0
#define UNIT_MIS_CONCENTR_PUNT_TITOL  1
#define UNIT_MIS_CONCENTR_GRAMMILITRO 2
#define UNIT_MIS_CONCENTR_uSIEMENS    3
#define UNIT_MIS_CONCENTR_mSIEMENS    4

#define UN_MIS_Y_START 2

#define SOGLIE_SET_CONC_INDEX 	    0
#define SOGLIE_ALL_CONC_MIN_INDEX   1
#define SOGLIE_ALL_CONC_MAX_INDEX   2
#define SOGLIE_ISTERESI_CONC_INDEX  3
#define SOGLIE_SET_TEMP_INDEX 	    4
#define SOGLIE_ALL_TEMP_MIN_INDEX   5
#define SOGLIE_ALL_TEMP_MAX_INDEX   6
#define SOGLIE_ISTERESI_TEMP_INDEX  7

#define LETTURA_CONC	1
#define LETTURA_TEMP	0

#define CALIBR_CENTR 0
#define CALIBR_3pt   1

#define TIMER1_RIT_ACC_CONC     0
#define TIMER2_TOUT_DOSAGGIO    1
#define TIMER3_RIT_DOSAGGIO     2
#define TIMER4_RIT_ALL_MIN_CONC 3
#define TIMER5_RIT_ALL_MAX_CONC 4
#define TIMER6_RIT_ACC_TEMP     5
#define TIMER7_TOUT_TEMP        6
#define TIMER8_RIT_ALL_MIN_TEMP 7
#define TIMER9_RIT_ALL_MAX_TEMP 8




                   /*+-+-+-+-+-+-+-+-+-+-+-+-+
                     |g|l|o|b|a|l|_|f|l|a|g|s|
                     +-+-+-+-+-+-+-+-+-+-+-+-+*/

#define OVER_CONC_MAX 						0x1
#define MARK_OVER_CONC_MAX  	(global_flags |= OVER_CONC_MAX )
#define CLEAR_OVER_CONC_MAX 	(global_flags &=~OVER_CONC_MAX )
#define CHECK_OVER_CONC_MAX 	(global_flags &  OVER_CONC_MAX )

#define OVER_CONC_MIN 						0x2
#define MARK_OVER_CONC_MIN  	(global_flags |= OVER_CONC_MIN )
#define CLEAR_OVER_CONC_MIN 	(global_flags &=~OVER_CONC_MIN )
#define CHECK_OVER_CONC_MIN 	(global_flags &  OVER_CONC_MIN )

#define OVER_CONC_STATE         (OVER_CONC_MAX | OVER_CONC_MIN )  
#define CHECK_OVER_CONC_STATE   (global_flags &  OVER_CONC_STATE )

#define MARK_OVER_CONC_NORMAL (global_flags &=~(OVER_CONC_STATE ))
#define OVER_CONC_NORMAL      0



#define OVER_TEMP_MAX 						0x4
#define MARK_OVER_TEMP_MAX  	(global_flags |= OVER_TEMP_MAX )
#define CLEAR_OVER_TEMP_MAX 	(global_flags &=~OVER_TEMP_MAX )
#define CHECK_OVER_TEMP_MAX 	(global_flags &  OVER_TEMP_MAX )

#define OVER_TEMP_MIN 						0x8
#define MARK_OVER_TEMP_MIN  	(global_flags |= OVER_TEMP_MIN )
#define CLEAR_OVER_TEMP_MIN 	(global_flags &=~OVER_TEMP_MIN )
#define CHECK_OVER_TEMP_MIN 	(global_flags &  OVER_TEMP_MIN )

#define OVER_TEMP_STATE         (OVER_TEMP_MAX | OVER_TEMP_MIN )  
#define CHECK_OVER_TEMP_STATE   (global_flags & OVER_TEMP_STATE)

#define MARK_OVER_TEMP_NORMAL   (global_flags &=~(OVER_TEMP_STATE ))
#define OVER_TEMP_NORMAL                                           0

//-----------------ALLARMI CONCENTRAZIONE-------------------------------------
#define ALARM_CONC_MAX 							0x10
#define MARK_ALARM_CONC_MAX  		(global_flags |= ALARM_CONC_MAX )
#define CLEAR_ALARM_CONC_MAX 		(global_flags &=~ALARM_CONC_MAX )
#define CHECK_ALARM_CONC_MAX 		(global_flags &  ALARM_CONC_MAX )

#define ALARM_CONC_MIN 							0x20
#define MARK_ALARM_CONC_MIN  		(global_flags |= ALARM_CONC_MIN )
#define CLEAR_ALARM_CONC_MIN 		(global_flags &=~ALARM_CONC_MIN )
#define CHECK_ALARM_CONC_MIN 		(global_flags &  ALARM_CONC_MIN )

#define TIMEOUT_CONC 							0x40
#define MARK_TIMEOUT_CONC  		(global_flags |= TIMEOUT_CONC )
#define CLEAR_TIMEOUT_CONC 		(global_flags &=~TIMEOUT_CONC )
#define CHECK_TIMEOUT_CONC 		(global_flags &  TIMEOUT_CONC )

#define ALARM_TANK 						        0x80
#define MARK_ALARM_TANK  	        (global_flags |= ALARM_TANK )
#define CLEAR_ALARM_TANK 	        (global_flags &=~ALARM_TANK )
#define CHECK_ALARM_TANK 	        (global_flags &  ALARM_TANK )
                       
#define ALARMS_CONC_MASK                (ALARM_CONC_MAX | ALARM_CONC_MIN | ALARM_TANK) 
#define CLEAR_CONC_ALARMS_MASK 		(global_flags &=~ALARMS_CONC_MASK )
#define CHECK_CONC_ALARMS_MASK		(global_flags &  ALARMS_CONC_MASK )

//-----------------ALLARMI TEMPERATURA   -------------------------------------
#define ALARM_TEMP_MAX 							0x100
#define MARK_ALARM_TEMP_MAX  		(global_flags |= ALARM_TEMP_MAX )
#define CLEAR_ALARM_TEMP_MAX 		(global_flags &=~ALARM_TEMP_MAX )
#define CHECK_ALARM_TEMP_MAX 		(global_flags &  ALARM_TEMP_MAX )

#define ALARM_TEMP_MIN 							0x200
#define MARK_ALARM_TEMP_MIN  		(global_flags |= ALARM_TEMP_MIN )
#define CLEAR_ALARM_TEMP_MIN 		(global_flags &=~ALARM_TEMP_MIN )
#define CHECK_ALARM_TEMP_MIN 		(global_flags &  ALARM_TEMP_MIN )

#define TIMEOUT_TEMP 							0x400
#define MARK_TIMEOUT_TEMP  		(global_flags |= TIMEOUT_TEMP )
#define CLEAR_TIMEOUT_TEMP 		(global_flags &=~TIMEOUT_TEMP )
#define CHECK_TIMEOUT_TEMP 		(global_flags &  TIMEOUT_TEMP )

#define ALARMS_TEMP_MASK                (ALARM_TEMP_MAX | ALARM_TEMP_MIN ) 
#define CLEAR_TEMP_ALARMS_MASK 		(global_flags &=~ALARMS_TEMP_MASK )
#define CHECK_TEMP_ALARMS_MASK		(global_flags &  ALARMS_TEMP_MASK )



#define PUMP_STATE_RIPOSO 						0x800
#define MARK_PUMP_STATE_RIPOSO  	(global_flags |= PUMP_STATE_RIPOSO )
#define CLEAR_PUMP_STATE_RIPOSO 	(global_flags &=~PUMP_STATE_RIPOSO )
#define CHECK_PUMP_STATE_RIPOSO 	(global_flags &  PUMP_STATE_RIPOSO )

#define PUMP_STATE_ATTIVO 						0x1000
#define MARK_PUMP_STATE_ATTIVO  	(global_flags |= PUMP_STATE_ATTIVO )
#define CLEAR_PUMP_STATE_ATTIVO 	(global_flags &=~PUMP_STATE_ATTIVO )
#define CHECK_PUMP_STATE_ATTIVO 	(global_flags &  PUMP_STATE_ATTIVO )

#define PUMP_STATE_WAIT 						0x2000
#define MARK_PUMP_STATE_WAIT  	(global_flags |= PUMP_STATE_WAIT )
#define CLEAR_PUMP_STATE_WAIT 	(global_flags &=~PUMP_STATE_WAIT )
#define CHECK_PUMP_STATE_WAIT 	(global_flags &  PUMP_STATE_WAIT )

#define PUMP_STATE (PUMP_STATE_RIPOSO | PUMP_STATE_ATTIVO | PUMP_STATE_WAIT)
#define CLEAR_PUMP_STATES       (global_flags &=~(PUMP_STATE_RIPOSO | PUMP_STATE_ATTIVO | PUMP_STATE_WAIT))
#define CHECK_PUMP_STATE        (global_flags &  PUMP_STATE)          




#define HEATER_STATE_RIPOSO 						0x4000
#define MARK_HEATER_STATE_RIPOSO  	(global_flags |= HEATER_STATE_RIPOSO )
#define CLEAR_HEATER_STATE_RIPOSO 	(global_flags &=~HEATER_STATE_RIPOSO )
#define CHECK_HEATER_STATE_RIPOSO 	(global_flags &  HEATER_STATE_RIPOSO )


#define HEATER_STATE_ATTIVO 						0x8000
#define MARK_HEATER_STATE_ATTIVO  	(global_flags |= HEATER_STATE_ATTIVO )
#define CLEAR_HEATER_STATE_ATTIVO 	(global_flags &=~HEATER_STATE_ATTIVO )
#define CHECK_HEATER_STATE_ATTIVO 	(global_flags &  HEATER_STATE_ATTIVO )

#define HEATER_STATES (HEATER_STATE_RIPOSO | HEATER_STATE_ATTIVO)
#define CLEAR_HEATER_STATES                (global_flags &=~ (HEATER_STATE_RIPOSO | HEATER_STATE_ATTIVO))                          
#define CHECK_HEATER_STATES                (global_flags &  HEATER_STATES)          

//mettere flag indicante uscita da menu,considerata come riaccensione,riazzera timers


#define CONTROL_CONC_ENA 						0x10000
#define MARK_CONTROL_CONC_ENA  		        (global_flags |= CONTROL_CONC_ENA )
#define CLEAR_CONTROL_CONC_ENA 		        (global_flags &=~CONTROL_CONC_ENA )
#define CHECK_CONTROL_CONC_ENA 		        (global_flags &  CONTROL_CONC_ENA )

#define CONTROL_TEMP_ENA 						0x20000
#define MARK_CONTROL_TEMP_ENA  			(global_flags |= CONTROL_TEMP_ENA )
#define CLEAR_CONTROL_TEMP_ENA 			(global_flags &=~CONTROL_TEMP_ENA )
#define CHECK_CONTROL_TEMP_ENA 			(global_flags &  CONTROL_TEMP_ENA )


#define ARROW_KEYS_MOVE_UPDOWN 						0x40000
#define MARK_ARROW_KEYS_MOVE_UPDOWN  	(global_flags |= ARROW_KEYS_MOVE_UPDOWN )
#define CLEAR_ARROW_KEYS_MOVE_UPDOWN 	(global_flags &=~ARROW_KEYS_MOVE_UPDOWN )
#define CHECK_ARROW_KEYS_MOVE_UPDOWN 	(global_flags &  ARROW_KEYS_MOVE_UPDOWN )


#define PIU_MENO_ENABLED 						0x80000
#define MARK_PIU_MENO_ENABLED  		(global_flags |= PIU_MENO_ENABLED )
#define CLEAR_PIU_MENO_ENABLED 		(global_flags &=~PIU_MENO_ENABLED )
#define CHECK_PIU_MENO_ENABLED 		(global_flags &  PIU_MENO_ENABLED )

#define ACCENSIONE_CONC 						0x100000
#define MARK_ACCENSIONE_CONC  	(global_flags |= ACCENSIONE_CONC )
#define CLEAR_ACCENSIONE_CONC 	(global_flags &=~ACCENSIONE_CONC )
#define CHECK_ACCENSIONE_CONC 	(global_flags &  ACCENSIONE_CONC )

#define ACCENSIONE_TEMP 						0x200000
#define MARK_ACCENSIONE_TEMP  	(global_flags |= ACCENSIONE_TEMP )
#define CLEAR_ACCENSIONE_TEMP 	(global_flags &=~ACCENSIONE_TEMP )
#define CHECK_ACCENSIONE_TEMP 	(global_flags &  ACCENSIONE_TEMP )


#define STATE_ABILITATO 						0x400000
#define MARK_STATE_ABILITATO  	(global_flags |= STATE_ABILITATO )
#define CLEAR_STATE_ABILITATO 	(global_flags &=~STATE_ABILITATO )
#define CHECK_STATE_ABILITATO 	(global_flags &  STATE_ABILITATO )
                       
#define INCOMP_UNIT_MISUR 						0x800000
#define MARK_INCOMP_UNIT_MISUR  	(global_flags |= INCOMP_UNIT_MISUR )
#define CLEAR_INCOMP_UNIT_MISUR 	(global_flags &=~INCOMP_UNIT_MISUR )
#define CHECK_INCOMP_UNIT_MISUR 	(global_flags &  INCOMP_UNIT_MISUR )    

#define ATTIVAZIONE_EXT_CH_CONC 					0x1000000
#define MARK_ATTIVAZIONE_EXT_CH_CONC  	(global_flags |= ATTIVAZIONE_EXT_CH_CONC )
#define CLEAR_ATTIVAZIONE_EXT_CH_CONC 	(global_flags &=~ATTIVAZIONE_EXT_CH_CONC )
#define CHECK_ATTIVAZIONE_EXT_CH_CONC 	(global_flags &  ATTIVAZIONE_EXT_CH_CONC )

#define ATTIVAZIONE_EXT_CH_TEMP 					0x2000000
#define MARK_ATTIVAZIONE_EXT_CH_TEMP  	(global_flags |= ATTIVAZIONE_EXT_CH_TEMP )
#define CLEAR_ATTIVAZIONE_EXT_CH_TEMP 	(global_flags &=~ATTIVAZIONE_EXT_CH_TEMP )
#define CHECK_ATTIVAZIONE_EXT_CH_TEMP 	(global_flags &  ATTIVAZIONE_EXT_CH_TEMP )


#define CABLE_OPEN 						        0x4000000
#define MARK_CABLE_OPEN  	(global_flags |= CABLE_OPEN )
#define CLEAR_CABLE_OPEN 	(global_flags &=~CABLE_OPEN )
#define CHECK_CABLE_OPEN 	(global_flags &  CABLE_OPEN )


                   /*  +-+-+-+-+-+-+-+-+-+-+-+
                       |p|r|i|n|t|_|f|l|a|g|s|
                       +-+-+-+-+-+-+-+-+-+-+-+ */

#define BLINK 								        0x1
#define MARK_BLINK  			(print_flags |= BLINK )
#define CLEAR_BLINK 			(print_flags &=~BLINK )
#define CHECK_BLINK 			(print_flags &  BLINK )

#define PRINT_PUMP 								0x2
#define MARK_PRINT_PUMP  		(print_flags |= PRINT_PUMP )
#define CLEAR_PRINT_PUMP 		(print_flags &=~PRINT_PUMP )
#define CHECK_PRINT_PUMP 		(print_flags &  PRINT_PUMP )

#define PRINT_HEATER 								0x4
#define MARK_PRINT_HEATER  		(print_flags |= PRINT_HEATER )
#define CLEAR_PRINT_HEATER 		(print_flags &=~PRINT_HEATER )
#define CHECK_PRINT_HEATER 		(print_flags &  PRINT_HEATER )

#define PRINT_CONC_LIMITS 							0x8
#define MARK_PRINT_CONC_LIMITS  	(print_flags |= PRINT_CONC_LIMITS )
#define CLEAR_PRINT_CONC_LIMITS 	(print_flags &=~PRINT_CONC_LIMITS )
#define CHECK_PRINT_CONC_LIMITS 	(print_flags &  PRINT_CONC_LIMITS )

#define PRINT_TEMP_LIMITS 							0x10
#define MARK_PRINT_TEMP_LIMITS  	(print_flags |= PRINT_TEMP_LIMITS )
#define CLEAR_PRINT_TEMP_LIMITS 	(print_flags &=~PRINT_TEMP_LIMITS )
#define CHECK_PRINT_TEMP_LIMITS 	(print_flags &  PRINT_TEMP_LIMITS )

#define PRINT_DISABILITA 							0x20
#define MARK_PRINT_DISABILITA  		(print_flags |= PRINT_DISABILITA )
#define CLEAR_PRINT_DISABILITA 		(print_flags &=~PRINT_DISABILITA )
#define CHECK_PRINT_DISABILITA 		(print_flags &  PRINT_DISABILITA )

#define PRINT_CONC_WAIT 							0x40
#define MARK_PRINT_CONC_WAIT  		(print_flags |= PRINT_CONC_WAIT )
#define CLEAR_PRINT_CONC_WAIT 		(print_flags &=~PRINT_CONC_WAIT )
#define CHECK_PRINT_CONC_WAIT 		(print_flags &  PRINT_CONC_WAIT )

#define PRINT_TEMP_WAIT 							0x80
#define MARK_PRINT_TEMP_WAIT  		(print_flags |= PRINT_TEMP_WAIT )
#define CLEAR_PRINT_TEMP_WAIT 		(print_flags &=~PRINT_TEMP_WAIT )
#define CHECK_PRINT_TEMP_WAIT 		(print_flags &  PRINT_TEMP_WAIT )

#define BLINK_SHOW								0x100
#define MARK_BLINK_SHOW  		(print_flags |= BLINK_SHOW )
#define CLEAR_BLINK_SHOW 		(print_flags &=~BLINK_SHOW )
#define CHECK_BLINK_SHOW 		(print_flags &  BLINK_SHOW )

#define PRINT_CONC_CH_OFF 							0x200
#define MARK_PRINT_CONC_CH_OFF  		(print_flags |= PRINT_CONC_CH_OFF )
#define CLEAR_PRINT_CONC_CH_OFF 		(print_flags &=~PRINT_CONC_CH_OFF )
#define CHECK_PRINT_CONC_CH_OFF 		(print_flags &  PRINT_CONC_CH_OFF )

#define PRINT_TEMP_CH_OFF 							0x400
#define MARK_PRINT_TEMP_CH_OFF  		(print_flags |= PRINT_TEMP_CH_OFF )
#define CLEAR_PRINT_TEMP_CH_OFF 		(print_flags &=~PRINT_TEMP_CH_OFF )
#define CHECK_PRINT_TEMP_CH_OFF 		(print_flags &  PRINT_TEMP_CH_OFF )                         
                         
                         
                         
                         
#define CONC_PRINT_STATE_BITS          0xF0000000   //4 bits per 16 possibili situazioni 
#define TEMP_PRINT_STATE_BITS          0x0F000000   //4 bits per 16 possibili situazioni 
                       

#define PRINT_CONC_AL_MIN 							0x1000
#define MARK_PRINT_CONC_AL_MIN  	(print_flags |= PRINT_CONC_AL_MIN )
#define CLEAR_PRINT_CONC_AL_MIN 	(print_flags &=~PRINT_CONC_AL_MIN )
#define CHECK_PRINT_CONC_AL_MIN 	(print_flags &  PRINT_CONC_AL_MIN )

#define PRINT_CONC_AL_MAX 							0x2000
#define MARK_PRINT_CONC_AL_MAX  	(print_flags |= PRINT_CONC_AL_MAX )
#define CLEAR_PRINT_CONC_AL_MAX 	(print_flags &=~PRINT_CONC_AL_MAX )
#define CHECK_PRINT_CONC_AL_MAX 	(print_flags &  PRINT_CONC_AL_MAX )

#define PRINT_CONC_ET 							        0x4000
#define MARK_PRINT_CONC_ET  		(print_flags |= PRINT_CONC_ET )
#define CLEAR_PRINT_CONC_ET 		(print_flags &=~PRINT_CONC_ET )
#define CHECK_PRINT_CONC_ET 		(print_flags &  PRINT_CONC_ET )

#define PRINT_CONC_TO 							        0x8000
#define MARK_PRINT_CONC_TO  		(print_flags |= PRINT_CONC_TO )
#define CLEAR_PRINT_CONC_TO 		(print_flags &=~PRINT_CONC_TO )
#define CHECK_PRINT_CONC_TO 		(print_flags &  PRINT_CONC_TO )                       
                       
#define PRINT_TEMP_AL_MIN 							0x10000
#define MARK_PRINT_TEMP_AL_MIN  	(print_flags |= PRINT_TEMP_AL_MIN )
#define CLEAR_PRINT_TEMP_AL_MIN 	(print_flags &=~PRINT_TEMP_AL_MIN )
#define CHECK_PRINT_TEMP_AL_MIN 	(print_flags &  PRINT_TEMP_AL_MIN )

#define PRINT_TEMP_AL_MAX 							0x20000
#define MARK_PRINT_TEMP_AL_MAX  	(print_flags |= PRINT_TEMP_AL_MAX )
#define CLEAR_PRINT_TEMP_AL_MAX 	(print_flags &=~PRINT_TEMP_AL_MAX )
#define CHECK_PRINT_TEMP_AL_MAX 	(print_flags &  PRINT_TEMP_AL_MAX )

#define PRINT_TEMP_TO 							        0x40000
#define MARK_PRINT_TEMP_TO  		(print_flags |= PRINT_TEMP_TO )
#define CLEAR_PRINT_TEMP_TO 		(print_flags &=~PRINT_TEMP_TO )
#define CHECK_PRINT_TEMP_TO 		(print_flags &  PRINT_TEMP_TO )                                              

#define PRINT_ALARMS_CONC_MASK (PRINT_CONC_AL_MIN | PRINT_CONC_AL_MAX | PRINT_CONC_ET )                            
#define CLEAR_PRINT_ALARMS_CONC_MASK (print_flags &=~ PRINT_ALARMS_CONC_MASK )
#define PRINT_ALARMS_TEMP_MASK (PRINT_TEMP_AL_MIN | PRINT_TEMP_AL_MAX )                       
#define CLEAR_PRINT_ALARMS_TEMP_MASK (print_flags &=~ PRINT_ALARMS_TEMP_MASK )
                         
#define FULL_SCREEN_CONC 		        			        0x80000
#define MARK_FULL_SCREEN_CONC  		(print_flags |= FULL_SCREEN_CONC )
#define CLEAR_FULL_SCREEN_CONC 		(print_flags &=~FULL_SCREEN_CONC )
#define CHECK_FULL_SCREEN_CONC 		(print_flags &  FULL_SCREEN_CONC )

#define FULL_SCREEN_TEMP 		        			        0x100000
#define MARK_FULL_SCREEN_TEMP  		(print_flags |= FULL_SCREEN_TEMP )
#define CLEAR_FULL_SCREEN_TEMP 		(print_flags &=~FULL_SCREEN_TEMP )
#define CHECK_FULL_SCREEN_TEMP 		(print_flags &  FULL_SCREEN_TEMP )

#define CABLE_OPEN_PRINT 		        			        0x200000
#define MARK_CABLE_OPEN_PRINT  		(print_flags |= CABLE_OPEN_PRINT )
#define CLEAR_CABLE_OPEN_PRINT 		(print_flags &=~CABLE_OPEN_PRINT )
#define CHECK_CABLE_OPEN_PRINT 		(print_flags &  CABLE_OPEN_PRINT )

                   /*  +-+-+-+-+-+-+-+-+-+-+-+
                       |t|i|m|e|r|_|f|l|a|g|s|
                       +-+-+-+-+-+-+-+-+-+-+-+  */



#define TIMER1_EXPIRED 						        0x1
#define MARK_TIMER1_EXPIRED  		(timer_flags |= TIMER1_EXPIRED )
#define CLEAR_TIMER1_EXPIRED 		(timer_flags &=~TIMER1_EXPIRED )
#define CHECK_TIMER1_EXPIRED 		(timer_flags &  TIMER1_EXPIRED )

#define TIMER2_EXPIRED 						        0x2
#define MARK_TIMER2_EXPIRED  		(timer_flags |= TIMER2_EXPIRED )
#define CLEAR_TIMER2_EXPIRED 		(timer_flags &=~TIMER2_EXPIRED )
#define CHECK_TIMER2_EXPIRED 		(timer_flags &  TIMER2_EXPIRED )

#define TIMER3_EXPIRED 						        0x4
#define MARK_TIMER3_EXPIRED  		(timer_flags |= TIMER3_EXPIRED )
#define CLEAR_TIMER3_EXPIRED 		(timer_flags &=~TIMER3_EXPIRED )
#define CHECK_TIMER3_EXPIRED 		(timer_flags &  TIMER3_EXPIRED )

#define TIMER4_EXPIRED 						        0x8
#define MARK_TIMER4_EXPIRED  		(timer_flags |= TIMER4_EXPIRED )
#define CLEAR_TIMER4_EXPIRED 		(timer_flags &=~TIMER4_EXPIRED )
#define CHECK_TIMER4_EXPIRED 		(timer_flags &  TIMER4_EXPIRED )

#define TIMER5_EXPIRED 						        0x10
#define MARK_TIMER5_EXPIRED  		(timer_flags |= TIMER5_EXPIRED )
#define CLEAR_TIMER5_EXPIRED 		(timer_flags &=~TIMER5_EXPIRED )
#define CHECK_TIMER5_EXPIRED 		(timer_flags &  TIMER5_EXPIRED )

#define TIMER6_EXPIRED 						        0x20
#define MARK_TIMER6_EXPIRED  		(timer_flags |= TIMER6_EXPIRED )
#define CLEAR_TIMER6_EXPIRED 		(timer_flags &=~TIMER6_EXPIRED )
#define CHECK_TIMER6_EXPIRED 		(timer_flags &  TIMER6_EXPIRED )

#define TIMER7_EXPIRED 						        0x40
#define MARK_TIMER7_EXPIRED  		(timer_flags |= TIMER7_EXPIRED )
#define CLEAR_TIMER7_EXPIRED 		(timer_flags &=~TIMER7_EXPIRED )
#define CHECK_TIMER7_EXPIRED 		(timer_flags &  TIMER7_EXPIRED )

#define TIMER8_EXPIRED 						        0x80
#define MARK_TIMER8_EXPIRED  		(timer_flags |= TIMER8_EXPIRED )
#define CLEAR_TIMER8_EXPIRED 		(timer_flags &=~TIMER8_EXPIRED )
#define CHECK_TIMER8_EXPIRED 		(timer_flags &  TIMER8_EXPIRED )

#define TIMER9_EXPIRED 						        0x100
#define MARK_TIMER9_EXPIRED  		(timer_flags |= TIMER9_EXPIRED )
#define CLEAR_TIMER9_EXPIRED 		(timer_flags &=~TIMER9_EXPIRED )
#define CHECK_TIMER9_EXPIRED 		(timer_flags &  TIMER9_EXPIRED )
                         
#define BLINK_TIMER1_EXPIRED 						0x200
#define MARK_BLINK_TIMER1_EXPIRED  	(timer_flags |= BLINK_TIMER1_EXPIRED )
#define CLEAR_BLINK_TIMER1_EXPIRED 	(timer_flags &=~BLINK_TIMER1_EXPIRED )
#define CHECK_BLINK_TIMER1_EXPIRED 	(timer_flags &  BLINK_TIMER1_EXPIRED )                      


      /*        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
                |o|u|t|p|u|t|_|i|m|a|g|e|_|b|i|t|s|
                +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
#if 0
#define OUT_BUZZER_ENA 					        0x1
#define MARK_OUT_BUZZER_ENA  		(immagine_stato_uscite |= OUT_BUZZER_ENA )
#define CLEAR_OUT_BUZZER_ENA 		(immagine_stato_uscite &=~OUT_BUZZER_ENA )
#define CHECK_OUT_BUZZER_ENA 		(immagine_stato_uscite &  OUT_BUZZER_ENA ) 
                       
#define OUT_TANK_ALARM_ENA 		        	                0x2
#define MARK_OUT_TANK_ALARM_ENA  	(immagine_stato_uscite |= OUT_TANK_ALARM_ENA )
#define CLEAR_OUT_TANK_ALARM_ENA 	(immagine_stato_uscite &=~OUT_TANK_ALARM_ENA )
#define CHECK_OUT_TANK_ALARM_ENA 	(immagine_stato_uscite &  OUT_TANK_ALARM_ENA )                       
                       
#define OUT_MIN_CONC_ALARM 						        0x4
#define MARK_OUT_MIN_CONC_ALARM  	(immagine_stato_uscite |= OUT_MIN_CONC_ALARM )
#define CLEAR_OUT_MIN_CONC_ALARM 	(immagine_stato_uscite &=~OUT_MIN_CONC_ALARM )
#define CHECK_OUT_MIN_CONC_ALARM 	(immagine_stato_uscite &  OUT_MIN_CONC_ALARM )                       

#define OUT_PUMP_ENABLE 						        0x8
#define MARK_OUT_PUMP_ENABLE  		(immagine_stato_uscite |= OUT_PUMP_ENABLE )
#define CLEAR_OUT_PUMP_ENABLE 		(immagine_stato_uscite &=~OUT_PUMP_ENABLE )
#define CHECK_OUT_PUMP_ENABLE 		(immagine_stato_uscite &  OUT_PUMP_ENABLE )

#define OUT_MAX_CONC_ALARM 						        0x10
#define MARK_OUT_MAX_CONC_ALARM  	(immagine_stato_uscite |= OUT_MAX_CONC_ALARM )
#define CLEAR_OUT_MAX_CONC_ALARM 	(immagine_stato_uscite &=~OUT_MAX_CONC_ALARM )
#define CHECK_OUT_MAX_CONC_ALARM 	(immagine_stato_uscite &  OUT_MAX_CONC_ALARM )

#define OUT_MIN_TEMP_ALARM 						        0x20
#define MARK_OUT_MIN_TEMP_ALARM  	(immagine_stato_uscite |= OUT_MIN_TEMP_ALARM )
#define CLEAR_OUT_MIN_TEMP_ALARM 	(immagine_stato_uscite &=~OUT_MIN_TEMP_ALARM )
#define CHECK_OUT_MIN_TEMP_ALARM 	(immagine_stato_uscite &  OUT_MIN_TEMP_ALARM )

#define OUT_HEATER_ENABLE 						        0x40
#define MARK_OUT_HEATER_ENABLE  	(immagine_stato_uscite |= OUT_HEATER_ENABLE )
#define CLEAR_OUT_HEATER_ENABLE 	(immagine_stato_uscite &=~OUT_HEATER_ENABLE )
#define CHECK_OUT_HEATER_ENABLE 	(immagine_stato_uscite &  OUT_HEATER_ENABLE )

#define OUT_MAX_TEMP_ALARM 						        0x80
#define MARK_OUT_MAX_TEMP_ALARM  	(immagine_stato_uscite |= OUT_MAX_TEMP_ALARM )
#define CLEAR_OUT_MAX_TEMP_ALARM 	(immagine_stato_uscite &=~OUT_MAX_TEMP_ALARM )
#define CHECK_OUT_MAX_TEMP_ALARM 	(immagine_stato_uscite &  MAX_TEMP_ALARM )                       
#endif




#define STATO_POMPA_RIPOSO 			0
#define STATO_POMPA_ON_CONC_SCARSA	        1
#define STATO_POMPA_TIMEOUT			2
#define STATO_INIZIALE			        3

#define STATO_RISC_RIPOSO 			0
#define STATO_RISC_ON_TEMP_SCARSA	        1
#define STATO_RISC_TIMEOUT			2




#define CONDUC_H2O_RUBINETTO 0.000661
#define CONDUC_H20_DISTILL   0.0000055      //     CONDUC_H2O_RUBINETTO//5.5us
#define SCALED_TK_DIV      10000
                       
#define MAX_CONCENTR_x100 10000                       
                       
#define MULTIPLIER_uS  1000000
#define MULTIPLIER_mS  1000

#define TIMERS_MAX_VAL     10000
#define TK_MAX_VAL          1000                 

                  
#define NUM_PROGRAMMI_MAX_INDEX 4 //l'indice va da 0 a 4,quindi 5 programmi
#define NUM_UN_MIS_MAX_INDEX    4




#define TASTO_PROG_OLD 						0x2
#define MARK_TASTO_PROG_OLD  	(keyold_flags |= TASTO_PROG_OLD )
#define CLEAR_TASTO_PROG_OLD 	(keyold_flags &=~TASTO_PROG_OLD )
#define CHECK_TASTO_PROG_OLD 	(keyold_flags &  TASTO_PROG_OLD )

#define TASTO_UP_SX_OLD 					0x4
#define MARK_TASTO_UP_SX_OLD  	(keyold_flags |= TASTO_UP_SX_OLD )
#define CLEAR_TASTO_UP_SX_OLD 	(keyold_flags &=~TASTO_UP_SX_OLD )
#define CHECK_TASTO_UP_SX_OLD 	(keyold_flags &  TASTO_UP_SX_OLD )


#define TASTO_DN_DX_OLD 					0x8
#define MARK_TASTO_DN_DX_OLD  	(keyold_flags |= TASTO_DN_DX_OLD )
#define CLEAR_TASTO_DN_DX_OLD 	(keyold_flags &=~TASTO_DN_DX_OLD )
#define CHECK_TASTO_DN_DX_OLD 	(keyold_flags &  TASTO_DN_DX_OLD )

#define TASTO_PLUS_OLD 						0x10
#define MARK_TASTO_PLUS_OLD  	(keyold_flags |= TASTO_PLUS_OLD )
#define CLEAR_TASTO_PLUS_OLD 	(keyold_flags &=~TASTO_PLUS_OLD )
#define CHECK_TASTO_PLUS_OLD 	(keyold_flags &  TASTO_PLUS_OLD )

#define TASTO_MENO_OLD 						0x20
#define MARK_TASTO_MENO_OLD  	(keyold_flags |= TASTO_MENO_OLD )
#define CLEAR_TASTO_MENO_OLD 	(keyold_flags &=~TASTO_MENO_OLD )
#define CHECK_TASTO_MENO_OLD 	(keyold_flags &  TASTO_MENO_OLD )

#define TASTO_OK_OLD 						0x40
#define MARK_TASTO_OK_OLD  	(keyold_flags |= TASTO_OK_OLD )
#define CLEAR_TASTO_OK_OLD 	(keyold_flags &=~TASTO_OK_OLD )
#define CHECK_TASTO_OK_OLD 	(keyold_flags &  TASTO_OK_OLD )






#define NUM_TIMERS              9
#define AUX_TIMER_ID           10 
#define TIMER_CHECK_TANK       10
#define CHECK_TANK_PERIOD     100  //in ms
#define BLINK_TIME_ms         500  //in ms
#endif /* SOURCES_MY_DEFINITIONS_H_ */


#define CALIBRI_10_ALTEZZA 12

#define TO_CONC_X_START  6
#define TO_CONC_X_END   20
#define TO_CONC_Y_START 40
#define TO_CONC_Y_END   TO_CONC_Y_START + CALIBRI_10_ALTEZZA


#define TO_TEMP_X_START  70
#define TO_TEMP_X_END    84
#define TO_TEMP_Y_START  40
#define TO_TEMP_Y_END    TO_TEMP_Y_START + CALIBRI_10_ALTEZZA


#define AL_CONC_X_START 22
#define AL_CONC_X_END   38
#define AL_CONC_Y_START 40
#define AL_CONC_Y_END   AL_CONC_Y_START + CALIBRI_10_ALTEZZA

//DP
#define AL2r_CONC_X_START 4
#define AL2r_CONC_X_END   62
#define AL2r_CONC_Y_START 54
#define AL2r_CONC_Y_END   64

#define AL2r_TEMP_X_START 68
#define AL2r_TEMP_X_END   126
#define AL2r_TEMP_Y_START 54
#define AL2r_TEMP_Y_END   64
                       
#define WAIT_CONC_AREA_X_START  2
#define WAIT_CONC_AREA_X_END   62
#define WAIT_CONC_AREA_Y_START 40
#define WAIT_CONC_AREA_Y_END   64 

#define WAIT_TEMP_AREA_X_START 66
#define WAIT_TEMP_AREA_X_END   128
#define WAIT_TEMP_AREA_Y_START 40
#define WAIT_TEMP_AREA_Y_END   64 
                       
                       
//DP END

#define AL_TEMP_X_START  86
#define AL_TEMP_X_END   100
#define AL_TEMP_Y_START  40
#define AL_TEMP_Y_END    AL_TEMP_Y_START + CALIBRI_10_ALTEZZA

#define ET_X_START      38
#define ET_X_END        52
#define ET_Y_START      40
#define ET_Y_END        ET_Y_START + CALIBRI_10_ALTEZZA

#define PUMP_AREA_X_START  2
#define PUMP_AREA_X_END   26
#define PUMP_AREA_Y_START 42
#define PUMP_AREA_Y_END   64 

#define HEATER_AREA_X_START 66
#define HEATER_AREA_X_END   92
#define HEATER_AREA_Y_START 42
#define HEATER_AREA_Y_END   64 
                  
                  
#define PUMP_E_SETPOINT_AREA_X_START  2
#define PUMP_E_SETPOINT_AREA_X_END   64
#define PUMP_E_SETPOINT_AREA_Y_START 40
#define PUMP_E_SETPOINT_AREA_Y_END   64 
                  
                  
#define HEATER_E_SETPOINT_AREA_X_START 66
#define HEATER_E_SETPOINT_AREA_X_END  128
#define HEATER_E_SETPOINT_AREA_Y_START 40
#define HEATER_E_SETPOINT_AREA_Y_END   64 

#define TO 0
#define AL 1
#define ET 2      

#define DISABIL_CONC_EXT     (1<<0)
#define DISABIL_TEMP_EXT     (1<<1)                
#define TANK_ALARM_INPUT     (1<<2)   

#define PT100_A_COEFF 0.0039083
#define PT100_B_COEFF -0.0000005775
#define PT100_Ro       100       
//#define C_COEFF
#define INTERPOLAZIONE_DIEGO 
                  
#define HOUR_X          10
#define H_MIN_SEP_X     24  
#define MIN_X           28
  
#define YEAR_X          10  
#define Y_MONTH_SEP_X   36  
#define MONTH_X         42  
#define MONTH_DAY_SEP_X 54 
#define DAY_X           60

#define CHANGE_HOUR  1
#define CHANGE_MIN   2
#define CHANGE_YEAR  3
#define CHANGE_MONTH 4
#define CHANGE_DAY   5                  