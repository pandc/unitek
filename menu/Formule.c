#include "bsp.h"
#include "io.h"

#include "freertos.h"
#include "task.h"
#include "ffile.h"
#include "meas.h"
#include "math.h"

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
            struct_conc_print.decimali_to_print =INTERO;
    }
    else
    {
            struct_conc_print.conc_to_print=decimillesimi;
            struct_conc_print.decimali_to_print =UN_DECIMALE;
    }
 }

//****************************************************************************************************************************************************
unsigned int  FormulaInversa_Conc_PuntTitol(void)
{
  unsigned int decimillesimi,multiplied;
  
  if(struct_conc_print.decimali_to_print==INTERO     )  {	 decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
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
  
  float delta_temp=f1-PROGR_IN_USO.temp_acq_curva_lav;//delta_temp è in °C
  
  if(1)//PROGR_IN_USO.curva_lav_cal_type==CURVA_LAV_1PT)
  {/*
    f1=(ftk/100)*delta_temp;
    f1=(1+(ftk/100)*delta_temp);*/
    f1=*meas_conduc /(1+(ftk/100)*delta_temp);
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
  
  float delta_temp=f1-PROGR_IN_USO.temp_acq_curva_lav;//delta_temp è in °C
  
  if(1)//PROGR_IN_USO.curva_lav_cal_type==CURVA_LAV_1PT)
  {/*
    f1=(ftk/100)*delta_temp;
    f1=(1+(ftk/100)*delta_temp);*/
    f1=*conduc_to_correct  /(1+(ftk/100)*delta_temp);
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
  switch(PROGR_IN_USO.curva_lav_cal_type)
  {  
    case CURVA_LAVORO_1PT:
      
      if(  ((conduc_meas-CONDUC_H20_DISTILL)<0) ||  ((conduc_meas-CONDUC_H20_DISTILL)==0)  )
      {
        f_concent=0;
        conduc_meas=CONDUC_H20_DISTILL;
      }
      else
      {  
        //NB la curva di lavoro parte da VALORI DISCRETI IMPOSTATI DA TASTIERA,da qui i decimali sono traformati e salvati in INTERI 100 VOLTE PIU' GRANDI
        pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index])/((PROGR_IN_USO.curva_lav_XconducC)-CONDUC_H20_DISTILL);
        f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);//se per qualche motivo(conduc_meas-CONDUC_H20_DISTILL)<0) uscirò dopo quando questa condizione verrà valutata
        //si potrebbe mettere qui il limite tipo if(f_concent>xxx)f_concent=xxx;  
      }
      break;
      
      
     case CURVA_LAVORO_2PT: 
     if((conduc_meas-PROGR_IN_USO.curva_lav_XconducL)<0)
        {
          f_concent=0;
          conduc_meas=PROGR_IN_USO.curva_lav_XconducL;
        } 
        
        else
        {
          
           if(!(conduc_meas>PROGR_IN_USO.curva_lav_XconducC))//per comprendere anche C
           {
             pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index] - 0) / ((PROGR_IN_USO.curva_lav_XconducC)-(PROGR_IN_USO.curva_lav_XconducL));
             f_concent=pendenza_m*(conduc_meas-PROGR_IN_USO.curva_lav_XconducL)+0;
           }
           else //cioè se sono nel tratto da C a [15]
           {
              pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index] - 0) / ((PROGR_IN_USO.curva_lav_XconducC)-(PROGR_IN_USO.curva_lav_XconducL));
             f_concent=pendenza_m*(conduc_meas-PROGR_IN_USO.curva_lav_XconducC) + (float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index];
             
             
           }
        }
       
       
      break;
    
     case CURVA_LAVORO_3PT: 
     //nel caso di curva a 3 punti dovrebbe cambiare solo la pendenza...
      
        if((conduc_meas-PROGR_IN_USO.curva_lav_XconducL)<0)
        {
          f_concent=0;
          conduc_meas=PROGR_IN_USO.curva_lav_XconducL;
        } 
        
        else
        {
           //trova entro quale segmento sono ora
           if(!(conduc_meas>PROGR_IN_USO.curva_lav_XconducC))//per comprendere anche C
           {
             pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]  - 0 ) / ((PROGR_IN_USO.curva_lav_XconducC)-(PROGR_IN_USO.curva_lav_XconducL));
             f_concent=pendenza_m*(conduc_meas-PROGR_IN_USO.curva_lav_XconducL)+0;
           }
           
           else if(!(conduc_meas>PROGR_IN_USO.curva_lav_XconducH))//per comprendere anche H
           {
             pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index]-  (float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]    )/
                    ((PROGR_IN_USO.curva_lav_XconducH)-(PROGR_IN_USO.curva_lav_XconducC));
             f_concent=pendenza_m*(conduc_meas-PROGR_IN_USO.curva_lav_XconducC) + (float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index];
           }
           else if((conduc_meas>PROGR_IN_USO.curva_lav_XconducH)) 
           {
             pendenza_m=((float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index]-  (float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_C_index]    )/
                    ((PROGR_IN_USO.curva_lav_XconducH)-(PROGR_IN_USO.curva_lav_XconducC));
             f_concent=pendenza_m*(conduc_meas-PROGR_IN_USO.curva_lav_XconducH)  + (float)PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index];
             //((float)PROGR_IN_USO.curva_lav_Yconcent[15])/((MAX_CONCENTR_x100)-(PROGR_IN_USO.curva_lav_XconducH));
             
           }
        }
    
      break;
    
    
    
    
     
  }
  

       
       
       
  switch(PROGR_IN_USO.unita_mis_concentr)
    {
      case UNIT_MIS_CONCENTR_PERCENTUALE:

            if   (f_concent<1000)struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                 struct_conc_print.decimali_to_print=UN_DECIMALE;
            //NB la curva di lavoro parte da VALORI DISCRETI IMPOSTATI DA TASTIERA,da qui i decimali sono traformati e salvati in INTERI 100 VOLTE PIU' GRANDI
            struct_conc_print.fconc_to_print=f_concent/100; // devo dare il vero valore da stampare,sprintf pensa solo a arrotondarlo a n decimali

            if(struct_conc_print.fconc_to_print>99.9)struct_conc_print.fconc_to_print=99.9;
            break ;
            
      case UNIT_MIS_CONCENTR_PUNT_TITOL:
            
            if   (f_concent<1000)struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                 struct_conc_print.decimali_to_print=UN_DECIMALE;
            struct_conc_print.fconc_to_print=f_concent/10;
            if(struct_conc_print.fconc_to_print>999)struct_conc_print.fconc_to_print=999;
            break ;
            
      case UNIT_MIS_CONCENTR_GRAMMILITRO:
           
            if   (f_concent<1000)struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                 struct_conc_print.decimali_to_print=UN_DECIMALE;
            struct_conc_print.fconc_to_print=f_concent/10;
 
            if(struct_conc_print.fconc_to_print>999)struct_conc_print.fconc_to_print=999;
            break ;
            
      case UNIT_MIS_CONCENTR_uSIEMENS:
        
//#define VALORE_RAW
#ifdef VALORE_RAW
            f_concent=conduc_meas*MULTIPLIER_uS;
#else
            
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return CONDUC_H20_DISTILL;

            
#endif       
            if   (f_concent<10) struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                struct_conc_print.decimali_to_print=UN_DECIMALE;
            
            if(struct_conc_print.fconc_to_print>999)struct_conc_print.fconc_to_print=999;
            break ;
 
    case UNIT_MIS_CONCENTR_mSIEMENS:
#ifdef VALORE_RAW
            f_concent=conduc_meas*MULTIPLIER_mS;
#else
            
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return (CONDUC_H20_DISTILL/MULTIPLIER_mS);
            //f_concent=pendenza_m*(conduc_meas-CONDUC_H20_DISTILL);
            
            
#endif   
            if   (f_concent<10) struct_conc_print.decimali_to_print=DUE_DECIMALI;
            else                struct_conc_print.decimali_to_print=UN_DECIMALE;
            struct_conc_print.fconc_to_print=f_concent;
            if(struct_conc_print.fconc_to_print>999)struct_conc_print.fconc_to_print=999;
            break ;
            
          default:
            if((conduc_meas-CONDUC_H20_DISTILL)<0)return 0;
            break;
    }//fine switch(PROGR_IN_USO.unita_mis_concentr)     
       
       
       
  
  return f_concent;
  
}
//****************************************************************************************************************************************************
//NB  il risultato è la stessa variabile modificata che gli viene data come argomento
void Convers_Res_to_Temp(float* float_res)
{
  
  

#ifdef INTERPOLAZIONE_DIEGO  
  
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
#else  
  /*
  float determinante;
  float f1,f2,f3;
  
  f1=-PT100_Ro*PT100_A_COEFF;
  f2=sqrt(ecc ecc.....
  
  
  */
#endif
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
      
      stepH_15 =(MAX_CONCENT_LIMIT_9990-PROGR_IN_USO.curva_lav_Yconcent[PROGR_IN_USO.curva_lav_H_index])/
        (15-PROGR_IN_USO.curva_lav_H_index);
      
      
      
     
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