#ifndef __COM_H
#define __COM_H

#include "usart1.h"

#define COM_Open(a,b,c)		U1_Open(a,b,c)
#define COM_Puts(a)			U1_Puts(a)
#define COM_Printf(...)		U1_Printf(__VA_ARGS__)
#define COM_Echo(a)			U1_Putc(a)
#define COM_Putc(a)			U1_Putc(a)
#define COM_RxEmpty()		U1_RxEmpty()
#define COM_Getch(a)		U1_Getch(a)
#define COM_TGetch(a,b)		U1_TGetch(a,b)
#define COM_Write(a,b)		U1_Write(a,b)

#endif