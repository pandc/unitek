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
	unsigned short SetConc;
	unsigned short AllConcMin;
	unsigned short AllConcMax;
	unsigned short IsteresiConc;
	unsigned short SetTemp;
	unsigned short AllTempMin;
	unsigned short AllTempMax;
	unsigned short IsteresiTemp;
}setp_e_soglie_struct;

typedef union
{
	setp_e_soglie_struct ses_struct;

	unsigned short setp_e_soglie_arr[8];
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
	setp_e_soglie_type setp_e_soglie;//16 bytes
	TK_type TK;						 //8

	unsigned short curva_lavoro[16]; //32
        unsigned short curva_lavoro3pt[16]; //32
	unsigned char curva_lav_cal_type;//1
	unsigned char curva_lav1_C_index;//1
	unsigned char curva_lav3_L_index;//1
	unsigned char curva_lav3_C_index;//1
	unsigned char curva_lav3_H_index;//1
	unsigned char unita_mis_concentr;//1
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
  unsigned int conc_to_print;
  unsigned int decimali_to_print;
  unsigned int resto;
}print_conc_var;


#endif /* MY_TYPES_H_ */
