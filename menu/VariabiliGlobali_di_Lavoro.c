/*
 * VariabiliGlobali_di_Lavoro.c
 *
 *  Created on: 29/apr/2016
 *      Author: Administrator
 */


#include "VariabiliGlobali_di_Lavoro.h"

unsigned int global_flags=0;
unsigned int key_flags=0;
unsigned int keyold_flags=0;
//unsigned char unita_mis_concentr=0;
unsigned int var_concentr_to_print;


//variabili RAM su cui lavorerò
program_type program_arr;
setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;
unsigned int abilita_disabilita=1;
//menu TK
unsigned int TK_attuale=0;
unsigned int TK_nuovo  =0;
//menu seleziona simbolo

//menu comunicazioni
unsigned char stato_RS485=0;
unsigned char stato_USB=0;
unsigned char linguaggio;
unsigned char c_index_Curva_di_lavoro=4;

unsigned char stato_intervento_conc,stato_intervento_temper;

//variabili selezione in uso
//menu IN OUT


unsigned int * sector_image;
my_FlashImage_type FlashImage;




//program_type __attribute__((section (".MySector"))) saved_prog_arr[5]={
program_type saved_prog_arr[5]={

	{//######################  PROGR 1  #####################
	.setp_e_soglie.ses_struct.SetConc   =0x7D0,
	.setp_e_soglie.ses_struct.AllConcMin=0x3E8,
	.setp_e_soglie.ses_struct.AllConcMax=4000,
	.setp_e_soglie.ses_struct.IsteresiConc=100,
	.setp_e_soglie.ses_struct.SetTemp=30000,
	.setp_e_soglie.ses_struct.AllTempMin=25000,
	.setp_e_soglie.ses_struct.AllTempMax=33000,
	.setp_e_soglie.ses_struct.IsteresiTemp=400,
	.TK=10001,
	.unita_mis_concentr=0,
	.curva_lav1_C_index=8,
	.curva_lav3_C_index=7,
	.curva_lav3_L_index=3,
	.curva_lav3_H_index=10,
	.curva_lav_cal_type=0,
	.curva_lavoro=0,1600,2600,3600,4600,5600,6600,7600,8600,9600,16800,11600,12600,13600,14600,15600},

	{//######################  PROGR 2  #####################
	.setp_e_soglie.ses_struct.SetConc   =2001,
	.setp_e_soglie.ses_struct.AllConcMin=1001,
	.setp_e_soglie.ses_struct.AllConcMax=4000,
	.setp_e_soglie.ses_struct.IsteresiConc=100,
	.setp_e_soglie.ses_struct.SetTemp=38000,
	.setp_e_soglie.ses_struct.AllTempMin=1000,
	.setp_e_soglie.ses_struct.AllTempMax=4000,
	.setp_e_soglie.ses_struct.IsteresiTemp=100,
	.TK=10002,
	.unita_mis_concentr=1,
	.curva_lav1_C_index=8,
	.curva_lav3_C_index=7,
	.curva_lav3_L_index=3,
	.curva_lav3_H_index=10,
	.curva_lav_cal_type=0,
	.curva_lavoro=0,1500,2500,3500,4500,5500,6500,7500,8500,9500,15600,11500,12500,13500,14500,15500},

	{//######################  PROGR 3  #####################
	.setp_e_soglie.ses_struct.SetConc   =2002,
	.setp_e_soglie.ses_struct.AllConcMin=1002,
	.setp_e_soglie.ses_struct.AllConcMax=4000,
	.setp_e_soglie.ses_struct.IsteresiConc=100,
	.setp_e_soglie.ses_struct.SetTemp=38000,
	.setp_e_soglie.ses_struct.AllTempMin=36000,
	.setp_e_soglie.ses_struct.AllTempMax=40000,
	.setp_e_soglie.ses_struct.IsteresiTemp=100,
	.TK=10003,
	.unita_mis_concentr=2,
	.curva_lav1_C_index=8,
	.curva_lav3_C_index=7,
	.curva_lav3_L_index=3,
	.curva_lav3_H_index=10,
	.curva_lav_cal_type=0,
	.curva_lavoro=0,1600,2600,3600,4600,5600,6600,7600,8600,9600,16800,11600,12600,13600,14600,15600},

	{//######################  PROGR 4  #####################

	.setp_e_soglie.ses_struct.SetConc   =2003,
	.setp_e_soglie.ses_struct.AllConcMin=1003,
	.setp_e_soglie.ses_struct.AllConcMax=4000,
	.setp_e_soglie.ses_struct.IsteresiConc=100,
	.setp_e_soglie.ses_struct.SetTemp=38000,
	.setp_e_soglie.ses_struct.AllTempMin=1000,
	.setp_e_soglie.ses_struct.AllTempMax=4000,
	.setp_e_soglie.ses_struct.IsteresiTemp=100,
	.TK=10004,
	.unita_mis_concentr=3,
	.curva_lav1_C_index=8,
	.curva_lav3_C_index=7,
	.curva_lav3_L_index=3,
	.curva_lav3_H_index=10,
	.curva_lav_cal_type=0,
	.curva_lavoro=0,1700,2700,3700,4700,5700,6700,7700,8700,9700,17800,11700,12700,13700,14700,15700},

	{//######################  PROGR 5  #####################

	.setp_e_soglie.ses_struct.SetConc   =2004,
	.setp_e_soglie.ses_struct.AllConcMin=1004,
	.setp_e_soglie.ses_struct.AllConcMax=4000,
	.setp_e_soglie.ses_struct.IsteresiConc=100,
	.setp_e_soglie.ses_struct.SetTemp=38000,
	.setp_e_soglie.ses_struct.AllTempMin=1000,
	.setp_e_soglie.ses_struct.AllTempMax=4000,
	.setp_e_soglie.ses_struct.IsteresiTemp=100,
	.TK=10005,
	.unita_mis_concentr=4,
	.curva_lav1_C_index=8,
	.curva_lav3_C_index=7,
	.curva_lav3_L_index=3,
	.curva_lav3_H_index=10,
	.curva_lav_cal_type=0,
	.curva_lavoro=0,1800,2800,3800,4800,5800,6800,7800,8800,9800,18000,11800,12800,13800,14800,15800}



	};
//deve essere nelleo stess0 ordine in cui le variabili sono in my_FlashImage_type
//unsigned char __attribute__((section (".MySector"))) saved_selected_program_id=2;
//unsigned char __attribute__((section (".MySector"))) saved_abilita_disabilita=0x34;
//unsigned char __attribute__((section (".MySector"))) saved_ComunRS485=2;
//unsigned char __attribute__((section (".MySector"))) saved_ComunUSB=  1;
//unsigned char __attribute__((section (".MySector"))) saved_Linguaggio=3;

unsigned char saved_selected_program_id=2;
unsigned char saved_abilita_disabilita=0x34;
unsigned char saved_ComunRS485=2;
unsigned char saved_ComunUSB=  1;
unsigned char saved_Linguaggio=3;


unsigned char selected_progr_index=0;





TK_type TK;

