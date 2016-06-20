/*
 * Menu.h
 *
 *  Created on: 11/apr/2016
 *      Author: Administrator
 */

#ifndef SOURCES_MENU_H_
#define SOURCES_MENU_H_

#include "my_types.h"

extern print_conc_var struct_conc_print;
extern unsigned char simbolo_in_uso;
extern void (*MenuFunctionPt[30])(void);
extern void (*WorkMenu_CalcPrint_UnMisura_Conc[8])(unsigned int);
extern void (*CalcPrint_UnMisura_Conc[8])(unsigned int ,unsigned int,unsigned int);
extern void (*IncrPrint_UnMisura_Conc[8])(unsigned short*,unsigned int,unsigned int,unsigned int);


void Formula_ConcConvers_Percent(unsigned int  bin);
unsigned int  FormulaInversa_Conc_Percent(void);

void Formula_ConcConvers_PuntTitol(unsigned int  bin);
unsigned int  FormulaInversa_Conc_PuntTitol(void);

void Formula_ConcConvers_grammiLitro(unsigned int  bin);
unsigned int FormulaInversa_Conc_grammiLitro(void);

void Formula_ConcConvers_uSiemens(unsigned int  bin);
unsigned int  FormulaInversa_Conc_uSiemens(void);

void Formula_ConcConvers_milliSiemens(unsigned int  bin);
unsigned int  FormulaInversa_Conc_milliSiemens(void);


void Rappresentazione_Conc_Percent(unsigned int bin);




void LoadDisplay_Logo(void);
void MenuInit(void);
void LoadRamSettingsFrom_uC_Flash(void);
void LoadRamSettingsFrom_External_DataFlash(void);
unsigned char SaveRamSettings_in_External_DataFlash(void);

void DecrSoglia(unsigned short index,unsigned short incr);
void DecrParamConc(unsigned int* par_pt,unsigned short incr);//viene chiamata riga per riga quindi stampa solo 1 valore

void WorkMenu_CalcPrint_Percent(unsigned int bin);
void WorkMenu_CalcPrint_PuntTitol(unsigned int bin);
void WorkMenu_CalcPrint_GrammiLitro(unsigned int bin);
void WorkMenu_CalcPrint_uSiemens(unsigned int bin);
void WorkMenu_CalcPrint_milliSiemens(unsigned int bin);

void CalcPrint_Percent_xy(unsigned int  bin,unsigned int x,unsigned int y);
void CalcPrint_PuntTitol_xy(unsigned int  bin,unsigned int x,unsigned int y);
void CalcPrint_GrammiLitro_xy(unsigned int  bin,unsigned int x,unsigned int y);
void CalcPrint_uSiemens_xy(unsigned int  bin,unsigned int x,unsigned int y);
void CalcPrint_milliSiemens_xy(unsigned int  bin,unsigned int x,unsigned int y);


void IncrPrintConc_Percent_xy(unsigned short* bin,unsigned int x,unsigned int y,unsigned int);
void IncrPrintConc_PuntTitol_xy(unsigned short* bin,unsigned int x,unsigned int y,unsigned int);
void IncrPrintConc_GrammiLitro_xy(unsigned short* bin,unsigned int x,unsigned int y,unsigned int);
void IncrPrintConc_uSiemens_xy(unsigned short* bin,unsigned int x,unsigned int y,unsigned int);
void IncrPrintConc_milliSiemens_xy(unsigned short* bin,unsigned int x,unsigned int y,unsigned int);

void Convers_Res_to_Temp(float* float_res);
float CalcoloConcent_Now(float conduc_meas);
float CompensConduc_TK(float*) ;

#define SET_CONC_INDEX      0
#define ALL_CONC_MIN_INDEX  1
#define ALL_CONC_MAX_INDEX  2
#define ISTERESI_CONC_INDEX 3
#define SET_TEMP_INDEX      4
#define ALL_TEMP_MIN_INDEX  5
#define ALL_TEMP_MAX_INDEX  6
#define ISTERESI_TEMP_INDEX 7

#define PROGR_IN_USO RamSettings.ptype_arr[RamSettings.selected_program_id]


#endif /* SOURCES_MENU_H_ */
