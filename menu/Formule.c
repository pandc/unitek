#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"
#include "ffile.h"
#include "meas.h"

#include "my_definitions.h"
#include "my_types.h"
#include "Conversioni.h"

#include "VariabiliGlobali_di_Lavoro.h"

#include "menu.h"


extern print_conc_var  struct_conc_print;

//****************************************************************************************************************************************************
//dal valore float precalcolato Yconcentr al valore percentuale

void Formula_ConcConvers_Percent(unsigned int  bin)
{
    unsigned int decimillesimi;
    //float mio_coeff_convers;
    
    
    
    decimillesimi=(unsigned int)(bin );
    

    if(decimillesimi>9999)decimillesimi=9999;

    if(decimillesimi>999)
    {
            struct_conc_print.conc_to_print     = decimillesimi/10;
            struct_conc_print.decimali_to_print =UN_DECIMALE;
    }
    else
    {
            struct_conc_print.conc_to_print=decimillesimi;
            struct_conc_print.decimali_to_print =DUE_DECIMALI;
    }
  
}
//****************************************************************************************************************************************************
unsigned int FormulaInversa_Conc_Percent(void)
{
  unsigned int decimillesimi,multiplied;
  
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)  	{decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==DUE_DECIMALI)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
  multiplied=decimillesimi*1;
  return multiplied;
}
//****************************************************************************************************************************************************
void Formula_ConcConvers_PuntTitol(unsigned int  bin)
{
    unsigned int decimillesimi;
    
    
    decimillesimi=(unsigned int)(bin *1);

    if(decimillesimi>9999)decimillesimi=9999;

    if(decimillesimi>999)
    {
            struct_conc_print.conc_to_print     = decimillesimi/10;
            struct_conc_print.decimali_to_print =UN_DECIMALE;
    }
    else
    {
            struct_conc_print.conc_to_print=decimillesimi;
            struct_conc_print.decimali_to_print =DUE_DECIMALI;
    }
 }

//****************************************************************************************************************************************************
unsigned int  FormulaInversa_Conc_PuntTitol(void)
{
  unsigned int decimillesimi,multiplied;
  
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)  {	 decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==DUE_DECIMALI)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
  multiplied=decimillesimi;//*64;
  multiplied+=struct_conc_print.resto;
  return multiplied;
 
}
//****************************************************************************************************************************************************
void Formula_ConcConvers_grammiLitro(unsigned int  bin)
{
    unsigned int decimillesimi;

   
     decimillesimi=(unsigned int)(bin *1);

     if(decimillesimi>9999)decimillesimi=9999;

     if(decimillesimi>999)
     {
              struct_conc_print.conc_to_print=decimillesimi/10;
              struct_conc_print.decimali_to_print =INTERO;
     }
     else
     {
              struct_conc_print.conc_to_print=decimillesimi;
              struct_conc_print.decimali_to_print =UN_DECIMALE;
     }
}
//****************************************************************************************************************************************************
unsigned int  FormulaInversa_Conc_grammiLitro(void)
{
  unsigned int decimillesimi,multiplied;
  
  if(struct_conc_print.decimali_to_print==INTERO     )  {	 decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
  multiplied=decimillesimi;//*64;
  multiplied+=struct_conc_print.resto;
  return multiplied;
 
}
//****************************************************************************************************************************************************
void Formula_ConcConvers_uSiemens(unsigned int  bin)
{
    unsigned int decimillesimi;
    decimillesimi=(unsigned int)(bin *1);

    if(decimillesimi>999)decimillesimi=999;

    struct_conc_print.conc_to_print=decimillesimi;
    struct_conc_print.decimali_to_print=INTERO;
}
//****************************************************************************************************************************************************
unsigned int  FormulaInversa_Conc_uSiemens(void)
{
  unsigned int decimillesimi,multiplied;
  
  if(struct_conc_print.decimali_to_print==INTERO)  {decimillesimi=struct_conc_print.conc_to_print*1;}
  
  multiplied=decimillesimi;//*64;
  multiplied+=struct_conc_print.resto;
  return multiplied;
}
//****************************************************************************************************************************************************
void Formula_ConcConvers_milliSiemens(unsigned int  bin)
{
    unsigned int decimillesimi;
    decimillesimi=(unsigned int)(bin *1);

    if(decimillesimi>999)decimillesimi=999;
    struct_conc_print.conc_to_print=decimillesimi;
    struct_conc_print.decimali_to_print=INTERO;
}
//****************************************************************************************************************************************************
unsigned int  FormulaInversa_Conc_milliSiemens(void)
{
  unsigned int decimillesimi,multiplied;
  
  if(struct_conc_print.decimali_to_print==INTERO)  {decimillesimi=struct_conc_print.conc_to_print*1;}
  
  multiplied=decimillesimi;//*64;
  multiplied+=struct_conc_print.resto;
  return multiplied;
}

//****************************************************************************************************************************************************
void Rappresentazione_Conc_Percent(unsigned int bin)
{
    unsigned int decimillesimi;
    

    decimillesimi=bin;
    

    if(decimillesimi>9999)decimillesimi=9999;

    if(decimillesimi>999)
    {
            struct_conc_print.conc_to_print     = decimillesimi/10;
            struct_conc_print.decimali_to_print =UN_DECIMALE;
    }
    else
    {
            struct_conc_print.conc_to_print=decimillesimi;
            struct_conc_print.decimali_to_print =DUE_DECIMALI;
    }
  
}
//****************************************************************************************************************************************************
float CompensConduc_TK(float* meas_conduc) //
{
  float f1,ftk;
  
  ftk=(float)PROGR_IN_USO.TK.tk2.old_TK/100;
  
  f1=measures.temp_resist;
  Convers_Res_to_Temp(&f1);//ora f1 è temperatura in °C,la temp di riferimento è già in °C
  
  float delta_temp=PROGR_IN_USO.temp_acq_curva_lav-f1;//delta_temp è in °C
  
  if(1)//PROGR_IN_USO.curva_lav_cal_type==CURVA_LAV_1PT)
  {/*
    f1=(ftk/100)*delta_temp;
    f1=(1+(ftk/100)*delta_temp);*/
    f1=*meas_conduc  *(1+(ftk/100)*delta_temp);
  }
  else  //curva_lav_3pt 
  {
    
  }
  
  
  return f1;
  
}


//****************************************************************************************************************************************************
float NormalizzaConduc_TK(float* conduc_to_correct,float* fixed_temp) //
{
  float f1,ftk;
  
  ftk=(float)PROGR_IN_USO.TK.tk2.old_TK/100;
  
  f1=*fixed_temp;
  //Convers_Res_to_Temp(&f1);//ora f1 è temperatura in °C,la temp di riferimento è già in °C
  
  float delta_temp=PROGR_IN_USO.temp_acq_curva_lav-f1;//delta_temp è in °C
  
  if(1)//PROGR_IN_USO.curva_lav_cal_type==CURVA_LAV_1PT)
  {/*
    f1=(ftk/100)*delta_temp;
    f1=(1+(ftk/100)*delta_temp);*/
    f1=*conduc_to_correct  *(1+(ftk/100)*delta_temp);
  }
  else  //curva_lav_3pt 
  {
    
  }
  
  
  return f1;
  
}
//****************************************************************************************************************************************************
float CalcoloConcent_Now(float conduc_meas)
{
  float f_concent;

  float pendenza_m;
  
  
  
  //il punto 0x ha conducibilità dell'acqua distillata: 5,5 us  
  //per il valore in punto intermedio applico formula della curva passante per 2 punti:
  //se devo trovare 
  /*                            FORMULA 
  
  conc_attuale=(conc_rif[C]-conc_rif[0]/Conduc[rif]-Conduc[0])   *  (Conduc_meas_now - Conduc[0])   -  conc_rif[0];
                                                        =
  conc_attuale=(conc_rif[C]-0/Conduc[rif]-CONDUC_H20_DISTILL)   *  (Conduc_meas_now - CONDUC_H20_DISTILL)   -  conc_rif[0];
  
  */
  if(PROGR_IN_USO.curva_lav_cal_type==CURVA_LAV_1PT)
  {
    
    pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index])/((PROGR_IN_USO.curva_lav_XconducC)-CONDUC_H20_DISTILL);
    switch(PROGR_IN_USO.unita_mis_concentr)
    {
      case UNIT_MIS_CONCENTR_PERCENTUALE:
            if((conduc_meas-CONDUC_H20_DISTILL)<0)
            {  
              global_float=0;
              return 0;
            }
            f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);
            if   (f_concent<1000)struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                 struct_conc_print.decimali_to_print=UN_DECIMALE;
            f_concent/=100; // devo dare il vero valore da stampare,sprintf pensa solo a arrotondarlo a n decimali
            global_float=f_concent;
            if(f_concent>99.9)f_concent=99.9;
            break ;
            
      case UNIT_MIS_CONCENTR_PUNT_TITOL:
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return 0;
            f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);
            if   (f_concent<1000)struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                 struct_conc_print.decimali_to_print=UN_DECIMALE;
            global_float=f_concent/100;
            f_concent/=10;
            if(f_concent>999)f_concent=999;
            break ;
            
      case UNIT_MIS_CONCENTR_GRAMMILITRO:
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return 0;
            f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);
            if   (f_concent<1000)struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                struct_conc_print.decimali_to_print=UN_DECIMALE;
            global_float=f_concent/100;
            f_concent/=10;
            if(f_concent>999)f_concent=999;
            break ;
            
      case UNIT_MIS_CONCENTR_uSIEMENS:
        
//#define VALORE_RAW
#ifdef VALORE_RAW
            f_concent=conduc_meas*MULTIPLIER_uS;
#else
            
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return CONDUC_H20_DISTILL;
            f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);
            
            
#endif       
            if   (f_concent<10) struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                struct_conc_print.decimali_to_print=UN_DECIMALE;
            if(f_concent>999)f_concent=999;
            global_float=conduc_meas;
            break ;
 
            
            
            
      case UNIT_MIS_CONCENTR_mSIEMENS:
#ifdef VALORE_RAW
            f_concent=conduc_meas*MULTIPLIER_mS;
#else
            
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return (CONDUC_H20_DISTILL/MULTIPLIER_mS);
            f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);
            
            
#endif   
            if   (f_concent<10) struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                struct_conc_print.decimali_to_print=UN_DECIMALE;
            if(f_concent>999)f_concent=999;
            global_float=conduc_meas;
            break ;
    }//fine switch(PROGR_IN_USO.unita_mis_concentr)
    
     
  }
  else//nel caso di curva a 3 punti dovrebbe cambiare solo la pendenza...
  {
    //trova entro quale segmento sono ora
     /*                            FORMULA 
  
  conc_attuale=(conc_rif[punto super.]-conc_rif[punto infer.]/Conduc[rif superiore]-Conduc[rif_inferiore])   *  (Conduc_meas_now - Conduc[0])   -  conc_rif[inferiore];
                                                        =
  conc_attuale=(conc_rif[C]-0/Conduc[rif]-CONDUC_H20_DISTILL)   *  (Conduc_meas_now - CONDUC_H20_DISTILL)   -  conc_rif[0];
  
  */
    
    
    
    
  }
  
  return f_concent;
  
}
//****************************************************************************************************************************************************
//NB  il risultato è la stessa variabile modificata che gli viene data come argomento
void Convers_Res_to_Temp(float* float_res)
{
//funzione mia che fa interpolazione lineare a segmenti
  unsigned int tab_index=0;
  float temp,interval_res,delta,f;
  
  f=*float_res;
  
  //deve stare tra 0 130 °C...se è oltre i limiti restituisco i limiti
  if(f < TabLinPT100[0])
  {
    *float_res=0;
    return ;
  }
  while(f > TabLinPT100[tab_index])
  {
    tab_index++;
    if(tab_index>13)
    {
      *float_res=130;
      return;
      
    }
  }
  
  interval_res=TabLinPT100[tab_index]-TabLinPT100[tab_index-1];//ampiezza intervallo res in cui ricade res misurata
  delta=f-TabLinPT100[tab_index-1];//differenza tra temp misurata e lim inferiore intervallo
  
 
  temp=delta/interval_res;
  
  *float_res=(float)(10*(tab_index-1))+temp*10;

}

//***************************************************************************************
//NB 16 elementi
void RicalcolaCurvaLavoro(void)
{
	unsigned char i;
	unsigned short step;

	step=(unsigned short)(PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]/PROGR_IN_USO.curva_lav_C_index);//curva_lavoro[0][selected_curva_lavoro_index]/selected_curva_lavoro_index;
	//per i valori inferiori al punto centrale
	for(i=0;i<PROGR_IN_USO.curva_lav_C_index;i++)
	{
		PROGR_IN_USO.curva_lav_Yconcent[i]=i*step;
	}

	//per i valori superiori al punto centrale
	for(i=PROGR_IN_USO.curva_lav_C_index+1;i<16;i++)
	{
		PROGR_IN_USO.curva_lav_Yconcent[i]=i*step;
	}
}

//***************************************************************************************
//la curva ha 16 elementi
void RicalcolaCurvaLavoro3pt(void)
{
      unsigned char i;
      unsigned short step0_L,stepL_C,stepC_H,stepH_15;
      
      
      step0_L =(unsigned short)(PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_L_index]/PROGR_IN_USO.curva_lav_L_index);
      
      stepL_C =(unsigned short)((PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]-PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_L_index])/
        (PROGR_IN_USO.curva_lav_C_index-PROGR_IN_USO.curva_lav_L_index));
      
       stepC_H =(unsigned short)((PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index]-PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index])/
        (PROGR_IN_USO.curva_lav_H_index-PROGR_IN_USO.curva_lav_C_index));
      
      stepH_15 =stepC_H;//(PROGR_IN_USO.curva_lav_Yconcent[15]-PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index])/
        //(15-PROGR_IN_USO.curva_lav_H_index);
      
      
      
     
      //per i valori inferiori al punto L e per L
      for(i=0;i<(PROGR_IN_USO.curva_lav_L_index);i++)//PROGR_IN_USO.curva_lav_L_index+1  per comprendere anche L ma L è già impostato
      {
              PROGR_IN_USO.curva_lav_Yconcent[i]=i*step0_L;
      }
     

      //per i valori tra L e C,C compreso,L c'è già
      for(i=(PROGR_IN_USO.curva_lav_L_index+1);i<(PROGR_IN_USO.curva_lav_C_index);i++)//PROGR_IN_USO.curva_lav_L_index+1  L è già a posto
      {
              PROGR_IN_USO.curva_lav_Yconcent[i]=PROGR_IN_USO.curva_lav_Yconcent[i-1]+stepL_C;//se L=100 e step =12 il 1° valore della seconda spezzata sarà 112
      }
  
      
      for(i=(PROGR_IN_USO.curva_lav_C_index+1);i<(PROGR_IN_USO.curva_lav_H_index);i++)//PROGR_IN_USO.curva_lav_C_index+1  C è già  a posto
      {
              PROGR_IN_USO.curva_lav_Yconcent[i]=PROGR_IN_USO.curva_lav_Yconcent[i-1]+stepC_H;//se C=200 e step =8 il 1° valore della seconda spezzata sarà 208
      }
    
      
      for(i=(PROGR_IN_USO.curva_lav_H_index+1);i<(15+1);i++)//15+1  per comprendere anche il 15° elemento
      {
              PROGR_IN_USO.curva_lav_Yconcent[i]=PROGR_IN_USO.curva_lav_Yconcent[i-1]+stepH_15;//se C=200 e step =8 il 1° valore della seconda spezzata sarà 208
      }
  
}