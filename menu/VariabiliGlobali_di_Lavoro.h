/*
 * VariabiliGlobali_di_Lavoro.h
 *
 *  Created on: 29/apr/2016
 *      Author: Administrator
 */

#ifndef SOURCES_VARIABILIGLOBALI_DI_LAVORO_H_
#define SOURCES_VARIABILIGLOBALI_DI_LAVORO_H_

#include "my_types.h"

volatile extern unsigned int global_flags;
extern unsigned int timer_flags;



//extern unsigned char unita_mis_concentr;
extern unsigned int var_concentr_to_print;
extern unsigned int TK_attuale;
extern unsigned int TK_nuovo  ;
extern unsigned int keyold_flags;

extern setp_e_soglie_type setp_e_soglie;
//extern setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;

extern unsigned char selected_curva_lavoro_index;




extern unsigned char stato_intervento_temper,stato_intervento_conc;



//variabili selezione in uso
//menu IN OUT

//menu TK
extern unsigned int TK_attuale;
extern unsigned int TK_nuovo ;
//menu seleziona simbolo
//extern unsigned int abilita_disabilita;

//menu comunicazioni
extern unsigned char stato_RS485;
extern unsigned char stato_USB;
extern unsigned char linguaggio;

extern unsigned int *sector_image;
extern my_RamSettings_type RamSettings;

extern const my_RamSettings_type  ucFlash_Settings;

extern const float TabLinPT100[24];

//extern program_type __attribute__((section (".MySector"))) saved_prog_arr[5];
//extern unsigned char __attribute__((section (".MySector"))) saved_selected_program_id;
//extern unsigned char __attribute__((section (".MySector"))) saved_abilita_disabilita;

#endif /* SOURCES_VARIABILIGLOBALI_DI_LAVORO_H_ */
