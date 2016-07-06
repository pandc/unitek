/*
 * my_types.h
 *
 *  Created on: Jun 26, 2014
 *      Author: Die go
 */

#ifndef MY_TYPES_H_
#define MY_TYPES_H_





typedef union
{
 unsigned char  u08data[40];
 unsigned int   u32data[5];
}arr_data_type;


typedef struct
{
	unsigned int SetConc;
	unsigned int AllConcMin;
	unsigned int AllConcMax;
	unsigned int IsteresiConc;
	unsigned int SetTemp;
	unsigned int AllTempMin;
	unsigned int AllTempMax;
	unsigned int IsteresiTemp;
}setp_e_soglie_struct;

typedef union
{
	setp_e_soglie_struct ses_struct;

	unsigned int setp_e_soglie_arr[8];
}setp_e_soglie_type;

typedef struct
{
	/*unsigned */int old_TK;
	/*unsigned */int nuovo_TK;
}tk_2_type;

typedef union
{
	tk_2_type tk2;

	/*unsigned */int TK_array[2];
}TK_type;


typedef struct
{
  unsigned int T1_Rit_Accensione_CH_C;
  unsigned int T2_Dosaggio;
  unsigned int T3_Rit_Dos;
  unsigned int T4_AllMin_C;
  unsigned int T5_AllMax_C;
  unsigned int T6_Rit_Accensione_CH_T;
  unsigned int T7_Riscald;
  unsigned int T8_AllMin_T;
  unsigned int T9_AllMax_T;
  
}T9_type;


typedef union
{
  T9_type Timers_9;
  
  unsigned int Timers_values[9];
  
  
  
}Timer9_type;





typedef struct
{
	unsigned char unita_mis_concentr;//1
        setp_e_soglie_type setp_e_soglie;//16 bytes
	TK_type TK;						 //8

        
       
	unsigned char curva_lav_cal_type;//1
 
        unsigned int curva_lav_Yconcent[16]; //32 non indispensabile da salvare perchè ricalcolabile
        
        float curva_lav_XconducL;
        float curva_lav_XconducC;
        float curva_lav_XconducH;
        
	unsigned char curva_lav_L_index;//1
	unsigned char curva_lav_C_index;//1
	unsigned char curva_lav_H_index;//1
        
	float temp_acq_curva_lav;
        
        unsigned int curva_lav_YconcentC_intero;
        
        Timer9_type   Timers;
}program_type;







typedef struct
{
	program_type ptype_arr[5];
	unsigned char selected_program_id;
	unsigned char abilita_disabilita;
	unsigned char ComunRS485;
	unsigned char ComunUSB;
	unsigned char Linguaggio;
        
        
        
}my_RamSettings_type;

typedef struct
{
	char Stringa[4];
}MenuStringType;


typedef struct
{
	unsigned char sec_to_print;
	unsigned char min_to_print;
	unsigned char hour_to_print;
	unsigned char weekday_to_print;
	unsigned char day_to_print;
	unsigned char month_to_print;
	unsigned char year_to_print;
}timedate_toprint_type;



typedef  struct
{
	unsigned char* bmp_start;
	unsigned char  bmp_n_col;
	unsigned char  bmp_n_row;
	unsigned char  bmp_x;
	unsigned char  bmp_y;
}disp_bmp_type;

typedef struct
{   //[numero possibili valori] [lunghezza stringa]
	char strings    [4][8];
}my_strings_type;

typedef struct
{
	unsigned char  min;
	unsigned char  ore;
	unsigned char  giorni;
	unsigned char  mese_anno;
}my_RTC_type1;



typedef struct
{
  float        fconc_to_print;
  unsigned int conc_to_print;
  unsigned int decimali_to_print;
  unsigned int resto;
}print_conc_var;




#endif /* MY_TYPES_H_ */
