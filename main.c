/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "bsp.h"
#include "tools.h"
#include "io.h"

#if defined(USE_DFMEM)
#include "ffile.h"
#include "config.h"
#endif

#include "freertos.h"
#include "task.h"

#include "irqprio.h"

#include "usart2.h"
#include "com.h"
#include "debug.h"
#include "backup.h"
#include "parser.h"
#include "datetime.h"
#include "mspi.h"

#include "power.h"
#include "i2c.h"
#include "keyboard.h"
#include "eeprom.h"
#include "parser.h"
#include "menu.h"
#include "meas.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
	for (;;);
}

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	for (;;);
}

static void vInitTask(void *pvParameters)
{
portTickType xLastWakeTime;

	MSPI_Init();
	COM_Open(115200,UARTP_None,UARTSB_1);

	I2C_Open(MAX_I2C_FREQ);
	EEP_Init();

#if defined(USE_DFMEM)
	ffstart();
#endif

	DBG_Init();

	WDOG_Refresh();

	parser_create_task();
	
	keyboard_init();

	MenuInit();

	meas_start();

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for (;;)
	{
		WDOG_Refresh();

#if defined(USE_DFMEM)
		ffrefresh();
#endif
		vTaskDelayUntil( &xLastWakeTime, kSec );
	}
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	__disable_interrupt();
	/* Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources */
	System_Setup();

	/*Allow access to Backup Registers*/
	PWR_BackupAccessCmd(ENABLE);

	//InitRtc();

	xTaskCreate(vInitTask,"initTask",128,NULL,tskIDLE_PRIORITY,NULL);

	vTaskStartScheduler();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
