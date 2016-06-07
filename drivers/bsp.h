/**
  ******************************************************************************
  * @file    bsp.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   This file contains all the functions prototypes for the STM32F107 
  *          file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "types.h"
#include "stm32f10x.h"

#define MAGIC_WORD_LAUNCH		0xABCDEF0110FEDCBAULL
#define MAGIC_WORD_PROG			0x12345ABCDEF54321ULL
#define MAGIC_WORD_PARSER		0x5432109876512345ULL
#define MAGIC_WORD_STANDBY		0x057A8DB11BD8A750ULL

#define GPIO_Speed_Default	GPIO_Speed_10MHz

#if defined(NO_WATCHDOG)
#define WDOG_Refresh()		__no_operation()
#else
#define WDOG_Refresh()		IWDG_ReloadCounter()
#endif

/* Includes ------------------------------------------------------------------*/
void GpioInit(GPIO_TypeDef* reg,u16 pin,GPIOMode_TypeDef type, u8 defValue);
void System_Setup(void);
void NVIC_Configuration(void);

void system_reset(void);
void system_setcmd(uint64_t cmd);
uint64_t system_getcmd(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10F107_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
