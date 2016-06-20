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
  unsigned int decimillesimi,multiplied,temp_test;
  
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)  	{decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==DUE_DECIMALI)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
  multiplied=decimillesimi*1;
 // multiplied+=struct_conc_print.resto;
  temp_test=multiplied/1;//era 10
  return  temp_test;
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
  unsigned int decimillesimi,multiplied,temp_test;
  
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)  {	 decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==DUE_DECIMALI)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
  multiplied=decimillesimi*64;
  multiplied+=struct_conc_print.resto;
  temp_test=multiplied/10;
  return temp_test;
 
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
  unsigned int decimillesimi,multiplied,temp_test;
  
  if(struct_conc_print.decimali_to_print==INTERO     )  {	 decimillesimi=struct_conc_print.conc_to_print*10;  }
  if(struct_conc_print.decimali_to_print==UN_DECIMALE)	{	 decimillesimi=struct_conc_print.conc_to_print   ;	 }
  multiplied=decimillesimi*64;
  multiplied+=struct_conc_print.resto;
  temp_test=multiplied/10;
  return temp_test;
 
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
  unsigned int decimillesimi,multiplied,temp_test;
  
  if(struct_conc_print.decimali_to_print==INTERO)  {decimillesimi=struct_conc_print.conc_to_print*1;}
  
  multiplied=decimillesimi*64;
  multiplied+=struct_conc_print.resto;
  temp_test=multiplied;
  
  return temp_test;
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
  unsigned int decimillesimi,multiplied,temp_test;
  
  if(struct_conc_print.decimali_to_print==INTERO)  {decimillesimi=struct_conc_print.conc_to_print*1;}
  
  multiplied=decimillesimi*64;
  multiplied+=struct_conc_print.resto;
  temp_test=multiplied;
  
  return temp_test;
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