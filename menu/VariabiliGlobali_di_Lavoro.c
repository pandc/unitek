/*
 * VariabiliGlobali_di_Lavoro.c
 *
 *  Created on: 29/apr/2016
 *      Author: Administrator
 */


#include "VariabiliGlobali_di_Lavoro.h"

volatile unsigned int global_flags=0;
volatile unsigned int print_flags=0;
unsigned int timer_flags=0;


unsigned char immagine_stato_uscite=0;
unsigned char I2C_conf_buf[4]={0,0,0,0};





//variabili RAM su cui lavorerò

setp_e_soglie_type conc_soglie_limit_up,conc_soglie_limit_dn;
//unsigned int abilita_disabilita=1;
//menu TK
unsigned int TK_attuale=0;
unsigned int TK_nuovo  =0;
//menu seleziona simbolo

//menu comunicazioni
unsigned char stato_RS485=0;
unsigned char stato_USB=0;



unsigned char stato_intervento_conc,stato_intervento_temper;

//variabili selezione in uso
//menu IN OUT


unsigned int * sector_image;
my_RamSettings_type RamSettings;



//*****VALORI DI EMERGENZA CARICATI IN FLASH DEL MICROCONTROLLORE E MAI PIU' MODIFICATI**********
const my_RamSettings_type ucFlash_Settings={
         //######################  PROGR 1  ##################### 
	.ptype_arr[0].setp_e_soglie.ses_struct.SetConc   =2000,
        .ptype_arr[0].setp_e_soglie.ses_struct.AllConcMin=1000,
	.ptype_arr[0].setp_e_soglie.ses_struct.AllConcMax=4000,
	.ptype_arr[0].setp_e_soglie.ses_struct.IsteresiConc=100,
	.ptype_arr[0].setp_e_soglie.ses_struct.SetTemp=30000,
	.ptype_arr[0].setp_e_soglie.ses_struct.AllTempMin=25000,
	.ptype_arr[0].setp_e_soglie.ses_struct.AllTempMax=33000,
	.ptype_arr[0].setp_e_soglie.ses_struct.IsteresiTemp=400,
	.ptype_arr[0].TK=100,
	.ptype_arr[0].unita_mis_concentr=0,
	.ptype_arr[0].curva_lav_C_index=7,
	.ptype_arr[0].curva_lav_L_index=3,
	.ptype_arr[0].curva_lav_H_index=10,
	.ptype_arr[0].curva_lav_cal_type=0,
	.ptype_arr[0].curva_lav_Yconcent=0,1600,2600,3600,4600,5600,6600,7600,8600,9600,16800,11600,12600,13600,14600,15600,
        .ptype_arr[0].Timers.Timers_9.T1_Rit_Accensione_CH_C=20,
        .ptype_arr[0].Timers.Timers_9.T2_Dosaggio=25,
        .ptype_arr[0].Timers.Timers_9.T3_Rit_Dos=30,
        .ptype_arr[0].Timers.Timers_9.T4_AllMin_C=35,
        .ptype_arr[0].Timers.Timers_9.T5_AllMax_C=40,
        .ptype_arr[0].Timers.Timers_9.T6_Rit_Accensione_CH_T=45,
        .ptype_arr[0].Timers.Timers_9.T7_Riscald=50,
        .ptype_arr[0].Timers.Timers_9.T8_AllMin_T=55,
        .ptype_arr[0].Timers.Timers_9.T9_AllMax_T=60,
	//######################  PROGR 2  #####################
	.ptype_arr[1].setp_e_soglie.ses_struct.SetConc   =2001,
	.ptype_arr[1].setp_e_soglie.ses_struct.AllConcMin=1001,
	.ptype_arr[1].setp_e_soglie.ses_struct.AllConcMax=4000,
	.ptype_arr[1].setp_e_soglie.ses_struct.IsteresiConc=100,
	.ptype_arr[1].setp_e_soglie.ses_struct.SetTemp=38000,
	.ptype_arr[1].setp_e_soglie.ses_struct.AllTempMin=1000,
	.ptype_arr[1].setp_e_soglie.ses_struct.AllTempMax=4000,
	.ptype_arr[1].setp_e_soglie.ses_struct.IsteresiTemp=100,
	.ptype_arr[1].TK=200,
	.ptype_arr[1].unita_mis_concentr=1,
	.ptype_arr[1].curva_lav_C_index=7,
	.ptype_arr[1].curva_lav_L_index=3,
	.ptype_arr[1].curva_lav_H_index=10,
	.ptype_arr[1].curva_lav_cal_type=0,
	.ptype_arr[1].curva_lav_Yconcent=0,1500,2500,3500,4500,5500,6500,7500,8500,9500,15600,11500,12500,13500,14500,15500,
        .ptype_arr[1].Timers.Timers_9.T1_Rit_Accensione_CH_C=20,
        .ptype_arr[1].Timers.Timers_9.T2_Dosaggio=25,
        .ptype_arr[1].Timers.Timers_9.T3_Rit_Dos=30,
        .ptype_arr[1].Timers.Timers_9.T4_AllMin_C=35,
        .ptype_arr[1].Timers.Timers_9.T5_AllMax_C=40,
        .ptype_arr[1].Timers.Timers_9.T6_Rit_Accensione_CH_T=45,
        .ptype_arr[1].Timers.Timers_9.T7_Riscald=50,
        .ptype_arr[1].Timers.Timers_9.T8_AllMin_T=55,
        .ptype_arr[1].Timers.Timers_9.T9_AllMax_T=60,
	//######################  PROGR 3  #####################
	.ptype_arr[2].setp_e_soglie.ses_struct.SetConc   =2002,
	.ptype_arr[2].setp_e_soglie.ses_struct.AllConcMin=1002,
	.ptype_arr[2].setp_e_soglie.ses_struct.AllConcMax=4000,
	.ptype_arr[2].setp_e_soglie.ses_struct.IsteresiConc=100,
	.ptype_arr[2].setp_e_soglie.ses_struct.SetTemp=38000,
	.ptype_arr[2].setp_e_soglie.ses_struct.AllTempMin=36000,
	.ptype_arr[2].setp_e_soglie.ses_struct.AllTempMax=40000,
	.ptype_arr[2].setp_e_soglie.ses_struct.IsteresiTemp=100,
	.ptype_arr[2].TK=300,
	.ptype_arr[2].unita_mis_concentr=2,
	.ptype_arr[2].curva_lav_C_index=7,
	.ptype_arr[2].curva_lav_L_index=3,
	.ptype_arr[2].curva_lav_H_index=10,
	.ptype_arr[2].curva_lav_cal_type=0,
	.ptype_arr[2].curva_lav_Yconcent=0,1600,2600,3600,4600,5600,6600,7600,8600,9600,16800,11600,12600,13600,14600,15600,
        .ptype_arr[2].Timers.Timers_9.T1_Rit_Accensione_CH_C=20,
        .ptype_arr[2].Timers.Timers_9.T2_Dosaggio=25,
        .ptype_arr[2].Timers.Timers_9.T3_Rit_Dos=30,
        .ptype_arr[2].Timers.Timers_9.T4_AllMin_C=35,
        .ptype_arr[2].Timers.Timers_9.T5_AllMax_C=40,
        .ptype_arr[2].Timers.Timers_9.T6_Rit_Accensione_CH_T=45,
        .ptype_arr[2].Timers.Timers_9.T7_Riscald=50,
        .ptype_arr[2].Timers.Timers_9.T8_AllMin_T=55,
        .ptype_arr[2].Timers.Timers_9.T9_AllMax_T=60,
	//######################  PROGR 4  #####################

	.ptype_arr[3].setp_e_soglie.ses_struct.SetConc   =2003,
	.ptype_arr[3].setp_e_soglie.ses_struct.AllConcMin=1003,
	.ptype_arr[3].setp_e_soglie.ses_struct.AllConcMax=4000,
	.ptype_arr[3].setp_e_soglie.ses_struct.IsteresiConc=100,
	.ptype_arr[3].setp_e_soglie.ses_struct.SetTemp=38000,
	.ptype_arr[3].setp_e_soglie.ses_struct.AllTempMin=1000,
	.ptype_arr[3].setp_e_soglie.ses_struct.AllTempMax=4000,
	.ptype_arr[3].setp_e_soglie.ses_struct.IsteresiTemp=100,
	.ptype_arr[3].TK=400,
	.ptype_arr[3].unita_mis_concentr=3,
	.ptype_arr[3].curva_lav_C_index=7,
	.ptype_arr[3].curva_lav_L_index=3,
	.ptype_arr[3].curva_lav_H_index=10,
	.ptype_arr[3].curva_lav_cal_type=0,
	.ptype_arr[3].curva_lav_Yconcent=0,1700,2700,3700,4700,5700,6700,7700,8700,9700,17800,11700,12700,13700,14700,15700,
        .ptype_arr[3].Timers.Timers_9.T1_Rit_Accensione_CH_C=20,
        .ptype_arr[3].Timers.Timers_9.T2_Dosaggio=25,
        .ptype_arr[3].Timers.Timers_9.T3_Rit_Dos=30,
        .ptype_arr[3].Timers.Timers_9.T4_AllMin_C=35,
        .ptype_arr[3].Timers.Timers_9.T5_AllMax_C=40,
        .ptype_arr[3].Timers.Timers_9.T6_Rit_Accensione_CH_T=45,
        .ptype_arr[3].Timers.Timers_9.T7_Riscald=50,
        .ptype_arr[3].Timers.Timers_9.T8_AllMin_T=55,
        .ptype_arr[3].Timers.Timers_9.T9_AllMax_T=60,
	//######################  PROGR 5  #####################

	.ptype_arr[4].setp_e_soglie.ses_struct.SetConc   =2004,
	.ptype_arr[4].setp_e_soglie.ses_struct.AllConcMin=1004,
	.ptype_arr[4].setp_e_soglie.ses_struct.AllConcMax=4000,
	.ptype_arr[4].setp_e_soglie.ses_struct.IsteresiConc=100,
	.ptype_arr[4].setp_e_soglie.ses_struct.SetTemp=38000,
	.ptype_arr[4].setp_e_soglie.ses_struct.AllTempMin=1000,
	.ptype_arr[4].setp_e_soglie.ses_struct.AllTempMax=4000,
	.ptype_arr[4].setp_e_soglie.ses_struct.IsteresiTemp=100,
	.ptype_arr[4].TK=500,
	.ptype_arr[4].unita_mis_concentr=4,
	.ptype_arr[4].curva_lav_C_index=7,
	.ptype_arr[4].curva_lav_L_index=3,
	.ptype_arr[4].curva_lav_H_index=10,
	.ptype_arr[4].curva_lav_cal_type=0,
	.ptype_arr[4].curva_lav_Yconcent=0,1800,2800,3800,4800,5800,6800,7800,8800,9800,18000,11800,12800,13800,14800,15800,
        .ptype_arr[4].Timers.Timers_9.T1_Rit_Accensione_CH_C=20,
        .ptype_arr[4].Timers.Timers_9.T2_Dosaggio=25,
        .ptype_arr[4].Timers.Timers_9.T3_Rit_Dos=30,
        .ptype_arr[4].Timers.Timers_9.T4_AllMin_C=35,
        .ptype_arr[4].Timers.Timers_9.T5_AllMax_C=40,
        .ptype_arr[4].Timers.Timers_9.T6_Rit_Accensione_CH_T=45,
        .ptype_arr[4].Timers.Timers_9.T7_Riscald=50,
        .ptype_arr[4].Timers.Timers_9.T8_AllMin_T=55,
        .ptype_arr[4].Timers.Timers_9.T9_AllMax_T=60,
        .selected_program_id=0,
        .abilita_disabilita =0x34,//tanto per mettere un valore verficabile a colpo d'occhio
        .ComunRS485=2,
        .ComunUSB=1,
        .Linguaggio=3,
          
      


	};
/*
const unsigned int TabLinPT100[24]={10000,
                                    10390,
                                    10779,
                                    11167,
                                    11554,
                                    11940,
                                    12324,
                                    12708,
                                    13090,
                                    13471,
                                    13851,
                                    14229,
                                    14607,
                                    14983,
                                    15358,
                                    15733,
                                    16105,
                                    16477,
                                    16848,
                                    17217,
                                    17586};

*/

const float        TabLinPT100[24]={100.00,
                                    103.90,
                                    107.79,
                                    111.67,
                                    115.54,
                                    119.40,
                                    123.24,
                                    127.08,
                                    130.90,
                                    134.71,
                                    138.51,
                                    142.29,
                                    146.07,
                                    149.83,
                                    153.58,
                                    157.33,
                                    161.05,
                                    164.77,
                                    168.48,
                                    172.17,
                                    175.86};








