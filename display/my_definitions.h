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






#define OLED_CS_AS_OUTPUT 		GPIOC_PDDR |=1<<2;
#define OLED_CS_HIGH 			GPIOC_PSOR  =1<<2;
#define OLED_CS_LOW  			GPIOC_PCOR  =1<<2;

#define OLED_DC_AS_OUTPUT 		GPIOC_PDDR |=1<<3;
#define OLED_DC_HIGH 			GPIOC_PSOR  =1<<3;
#define OLED_DC_LOW  			GPIOC_PCOR  =1<<3;

#define OLED_RST_AS_OUTPUT 		GPIOB_PDDR |=1<<23;
#define OLED_RST_HIGH 			GPIOB_PSOR  =1<<23;
#define OLED_RST_LOW  			GPIOB_PCOR  =1<<23;

#define DCDC_ENABLE_AS_OUTPUT 	GPIOB_PDDR |=1<<9;
#define DCDC_ENABLE_HIGH 		GPIOB_PSOR  =1<<9;
#define DCDC_ENABLE_LOW  		GPIOB_PCOR  =1<<9;

#define PROVA_SHIFT             provashift &=~(1<<9)
#define TASTO_PROG_AS_INPUT     GPIOC_PDDR &=~ 0x80   	//PTC7 era PTC0
#define TASTO_UPSX_AS_INPUT     GPIOC_PDDR &=~ 0x200   	//PTC9
#define TASTO_DNDX_AS_INPUT     GPIOC_PDDR &=~ 0x100   	//PTC8
#define TASTO_PLUS_AS_INPUT     GPIOC_PDDR &=~ 0x20   	//PTC5 era PTC1
#define TASTO_MENO_AS_INPUT     GPIOB_PDDR &=~ 0x80000  //PTB19
#define TASTO_OK_AS_INPUT       GPIOB_PDDR &=~ 0x40000  //PTB18

#define TASTO_PROG_LOW     (!(GPIOC_PDIR & 1<< 7))   //PTC7
#define TASTO_UP_SX_LOW    (!(GPIOC_PDIR & 1<< 9))   //PTC9
#define TASTO_DN_DX_LOW    (!(GPIOC_PDIR & 1<< 8))   //PTC8
#define TASTO_PLUS_LOW     (!(GPIOC_PDIR & 1<<5))    //PTC5
#define TASTO_MENO_LOW     (!(GPIOB_PDIR & 1<<19))   //PTB19
#define TASTO_OK_LOW       (!(GPIOB_PDIR & 1<<18))   //PTB18

#define BUZZER_OUT_AS_OUTPUT 		GPIOB_PDDR |=1<<20;
#define BUZZER_OUT_HIGH 			GPIOB_PCOR  =1<<20;//era PSOR
#define BUZZER_OUT_LOW  			GPIOB_PCOR  =1<<20;



#define ADC_WAIT_FOR_INTERRUPT_ON_END 0

#define TRIANG_W	6
#define TRIANG_H	16

#define ABILITA 	1
#define DISABILITA 	0




#define MENU_STAY 128

#define DUE_DECIMALI 2
#define UN_DECIMALE  1
#define INTERO       0

#define H_RIGA_CALIBRI10 		12
#define PRIMA_RIGA_CALIBRI10 	 2
#define TEMP_MAX_LIMIT		   1300
#define TEMP_MIN_LIMIT		    100

void MenuTempHum(void);
void MenuProg(void);
void SubmenuINOUT(void);
void SubmenuSelProgr(void);
void SubmenuSelectedProgr(void);
void SubmenuComunic(void);
void SubmenuSetClock(void);
void SumMenuSelLingua(void);
void SubmenuServizio(void);

void Sub2MenuImpostaSimboli(void);
void Sub2MenuTK(void);
void Sub2MenuCurvadiLavoro(void);
void Sub2MenuCurvadiLavoro3Punti(void);
void Sub2MenuSelTipoCurvaLavoro(void);
void Sub2MenuImpostaSoglie(void);
void Sub2MenuImpostaTimer (void);

void PrintSoglia(unsigned short index,unsigned short x ,unsigned short y);
void IncrSoglia(unsigned short index,unsigned short incr);
void DecrSoglia(unsigned short index,unsigned short incr);//viene chiamata riga per riga quindi stampa solo 1 valore
void PrintUnitMis(unsigned short index,unsigned short x ,unsigned short y);


void MoveTriangolinoDown(void);
void MoveTriangolinoUp  (void);
void MoveTriangolinoDx(void);
void MoveTriangolinoSx(void);
void DisegnaTriangolinoMenu(unsigned short triang_x,unsigned short triang_y);
void DisegnaCornice (void);
void DisegnaMarker(unsigned short x,unsigned short y,unsigned short y_old);
void DisegnaCarattereBlink(char char_to_blink,unsigned short x,unsigned short y,unsigned char *toggler);
void RicalcolaCurvaLavoro(void);

void MenuInit(void);
void WriteMyFlashSector(void);



#define MENU_TEMPHUM  				0
#define MENU_PROGR 					1
#define SUBMENU_INOUT  				2
#define SUBMENU_SELEZIONA_PROG 		3
#define SUBMENU_COMUNIC				4
#define SUBMENU_SETCLOCK 			5
#define SUBMENU_SEL_LINGUA			6
#define SUBMENU_SERVIZIO  			7
#define SUBMENU_SELECTED_PROGR		8

#define SUB2MENU_IMPOSTA_SIMBOLI	9
#define SUB2MENU_TK					10
#define SUB2MENU_SEL_TIPO_CURV_LAV  11
#define SUB2MENU_IMPOSTA_SOGLIE		12
#define SUB2MENU_IMPOSTA_TIMER		13
#define SUB3MENU_CURVA_DI_LAVORO	14
#define SUB3MENU_CURVA_DI_LAVORO3pt	15


#define SPOSTA_TRIANGOLINO  10

#define UNIT_MIS_CONCENTR_PERCENTUALE 0
#define UNIT_MIS_CONCENTR_PUNT_TITOL  1
#define UNIT_MIS_CONCENTR_GRAMMILITRO 2
#define UNIT_MIS_CONCENTR_uSIEMENS    3
#define UNIT_MIS_CONCENTR_mSIEMENS 	  4

#define UN_MIS_Y_START 2

#define SOGLIE_SET_CONC_INDEX 		0
#define SOGLIE_ALL_CONC_MIN_INDEX   1
#define SOGLIE_ALL_CONC_MAX_INDEX   2
#define SOGLIE_ISTERESI_CONC_INDEX  3
#define SOGLIE_SET_TEMP_INDEX 		4
#define SOGLIE_ALL_TEMP_MIN_INDEX 	5
#define SOGLIE_ALL_TEMP_MAX_INDEX 	6
#define SOGLIE_ISTERESI_TEMP_INDEX  7

#define LETTURA_CONC	1
#define LETTURA_TEMP	0

#define CALIBR_CENTR 0
#define CALIBR_3pt   1




#define BLINK_PUMP 								0x1
#define MARK_BLINK_PUMP  			(global_flags |= BLINK_PUMP )
#define CLEAR_BLINK_PUMP 			(global_flags &=~BLINK_PUMP )
#define CHECK_BLINK_PUMP 			(global_flags &  BLINK_PUMP )

#define ADC0_AQUIRED 								0x2
#define MARK_ADC0_AQUIRED  			(global_flags |= ADC0_AQUIRED )
#define CLEAR_ADC0_AQUIRED 			(global_flags &=~ADC0_AQUIRED )
#define CHECK_ADC0_AQUIRED 			(global_flags &  ADC0_AQUIRED )

#define ADC1_AQUIRED 								0x4
#define MARK_ADC1_AQUIRED  			(global_flags |= ADC1_AQUIRED )
#define CLEAR_ADC1_AQUIRED 			(global_flags &=~ADC1_AQUIRED )
#define CHECK_ADC1_AQUIRED 			(global_flags &  ADC1_AQUIRED )



#define TASTO_PROG_PRESSED 						0x1
#define MARK_TASTO_PROG_PRESSED  	(key_flags |= TASTO_PROG_PRESSED )
#define CLEAR_TASTO_PROG_PRESSED 	(key_flags &=~TASTO_PROG_PRESSED )
#define CHECK_TASTO_PROG_PRESSED 	(key_flags &  TASTO_PROG_PRESSED )

#define TASTO_UP_SX_PRESSED 					0x2
#define MARK_TASTO_UP_SX_PRESSED  	(key_flags |= TASTO_UP_SX_PRESSED )
#define CLEAR_TASTO_UP_SX_PRESSED 	(key_flags &=~TASTO_UP_SX_PRESSED )
#define CHECK_TASTO_UP_SX_PRESSED 	(key_flags &  TASTO_UP_SX_PRESSED )


#define TASTO_DN_DX_PRESSED 					0x4
#define MARK_TASTO_DN_DX_PRESSED  	(key_flags |= TASTO_DN_DX_PRESSED )
#define CLEAR_TASTO_DN_DX_PRESSED 	(key_flags &=~TASTO_DN_DX_PRESSED )
#define CHECK_TASTO_DN_DX_PRESSED 	(key_flags &  TASTO_DN_DX_PRESSED )

#define TASTO_PLUS_PRESSED 						0x8
#define MARK_TASTO_PLUS_PRESSED  	(key_flags |= TASTO_PLUS_PRESSED )
#define CLEAR_TASTO_PLUS_PRESSED 	(key_flags &=~TASTO_PLUS_PRESSED )
#define CHECK_TASTO_PLUS_PRESSED 	(key_flags &  TASTO_PLUS_PRESSED )

#define TASTO_MENO_PRESSED 						0x10
#define MARK_TASTO_MENO_PRESSED  	(key_flags |= TASTO_MENO_PRESSED )
#define CLEAR_TASTO_MENO_PRESSED 	(key_flags &=~TASTO_MENO_PRESSED )
#define CHECK_TASTO_MENO_PRESSED 	(key_flags &  TASTO_MENO_PRESSED )

#define TASTO_OK_PRESSED 						0x20
#define MARK_TASTO_OK_PRESSED  		(key_flags |= TASTO_OK_PRESSED )
#define CLEAR_TASTO_OK_PRESSED 		(key_flags &=~TASTO_OK_PRESSED )
#define CHECK_TASTO_OK_PRESSED 		(key_flags &  TASTO_OK_PRESSED )


#define ARROW_KEYS_MOVE_UPDOWN 						0x40
#define MARK_ARROW_KEYS_MOVE_UPDOWN  	(key_flags |= ARROW_KEYS_MOVE_UPDOWN )
#define CLEAR_ARROW_KEYS_MOVE_UPDOWN 	(key_flags &=~ARROW_KEYS_MOVE_UPDOWN )
#define CHECK_ARROW_KEYS_MOVE_UPDOWN 	(key_flags &  ARROW_KEYS_MOVE_UPDOWN )

#define ARROW_KEYS_MOVE_SXDX 						0x80
#define MARK_ARROW_KEYS_MOVE_SXDX  	(key_flags |= ARROW_KEYS_MOVE_SXDX )
#define CLEAR_ARROW_KEYS_MOVE_SXDX 	(key_flags &=~ARROW_KEYS_MOVE_SXDX )
#define CHECK_ARROW_KEYS_MOVE_SXDX 	(key_flags &  ARROW_KEYS_MOVE_SXDX )

#define PIU_MENO_ENABLED 						0x100
#define MARK_PIU_MENO_ENABLED  		(key_flags |= PIU_MENO_ENABLED )
#define CLEAR_PIU_MENO_ENABLED 		(key_flags &=~PIU_MENO_ENABLED )
#define CHECK_PIU_MENO_ENABLED 		(key_flags &  PIU_MENO_ENABLED )


#define TASTO_PLUS_RELEASED 						0x200
#define MARK_TASTO_PLUS_RELEASED  	(key_flags |= TASTO_PLUS_RELEASED )
#define CLEAR_TASTO_PLUS_RELEASED 	(key_flags &=~TASTO_PLUS_RELEASED )
#define CHECK_TASTO_PLUS_RELEASED 	(key_flags &  TASTO_PLUS_RELEASED )

#define TASTO_MENO_RELEASED 						0x400
#define MARK_TASTO_MENO_RELEASED  	(key_flags |= TASTO_MENO_RELEASED )
#define CLEAR_TASTO_MENO_RELEASED 	(key_flags &=~TASTO_MENO_RELEASED )
#define CHECK_TASTO_MENO_RELEASED 	(key_flags &  TASTO_MENO_RELEASED )

#define KEY_READY 						0x800
#define MARK_KEY_READY  	(key_flags |= KEY_READY )
#define CLEAR_KEY_READY 	(key_flags &=~KEY_READY )
#define CHECK_KEY_READY 	(key_flags &  KEY_READY )


#define STATO_POMPA_RIPOSO 				0
#define STATO_POMPA_ON_CONC_SCARSA		1
#define STATO_POMPA_TIMEOUT				2
#define STATO_INIZIALE				    3

#define STATO_RISC_RIPOSO 				0
#define STATO_RISC_ON_TEMP_SCARSA		1
#define STATO_RISC_TIMEOUT				2













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
#define MARK_TASTO_OK_OLD  		(keyold_flags |= TASTO_OK_OLD )
#define CLEAR_TASTO_OK_OLD 		(keyold_flags &=~TASTO_OK_OLD )
#define CHECK_TASTO_OK_OLD 		(keyold_flags &  TASTO_OK_OLD )

#endif /* SOURCES_MY_DEFINITIONS_H_ */
