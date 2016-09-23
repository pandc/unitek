#include "bsp.h"
#include "io.h"
#include "irqprio.h"

#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "i2c.h"

#include "com.h"
#include "debug.h"
#include "ioexp.h"

#define IOEXP0_ADDR		0x20
#define IOEXP1_ADDR		0x21

static const uint8_t ioexp0_conf[4] = { 0, 0, 0, 0 };

static xSemaphoreHandle xSemaDigin,xSemaMutex;
static uint8_t ioexp0_out = 0xff,initok,ioexp1_inp;

void DIG_IN_IRQn_EXTI_IRQ_HANDLER(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (EXTI_GetITStatus(DIG_IN_IRQn_EXTI_LINE) != RESET)
	{
		/* Clear IT flag */
		EXTI_ClearITPendingBit(DIG_IN_IRQn_EXTI_LINE);
		/* Disable EXTI Line9 IRQ */
		//EXTI->IMR &= ~KB_IRQn_EXTI_LINE;
		
		/* CR95HF is ready to send data */
		xSemaphoreGiveFromISR(xSemaDigin,&xHigherPriorityTaskWoken);
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

uint8_t IOEXP_get(void)
{
	return ioexp1_inp;
}

void IOEXP_set(uint8_t mask)
{
uint8_t b;

	xSemaphoreTake(xSemaMutex,portMAX_DELAY);
	b = ioexp0_out | mask;
	if (b != ioexp0_out)
	{
		ioexp0_out = b;
		if (initok)
			I2C_RandWrite(IOEXP0_ADDR,1,1,&ioexp0_out, 1);
	}
	xSemaphoreGive(xSemaMutex);
}

void IOEXP_clr(uint8_t mask)
{
uint8_t b;

	xSemaphoreTake(xSemaMutex,portMAX_DELAY);
	b = ioexp0_out & ~mask;
	if (b != ioexp0_out)
	{
		ioexp0_out = b;
		if (initok)
			I2C_RandWrite(IOEXP0_ADDR,1,1,&ioexp0_out, 1);
	}
	xSemaphoreGive(xSemaMutex);
}

static void IOEXP_task(void *par)
{
uint8_t b;

	xSemaphoreTake(xSemaMutex,portMAX_DELAY);
	for (;;)
	{
		vTaskDelay(kCen);
		if (!I2C_RandWrite(IOEXP0_ADDR,0x01,1,&ioexp0_out, 1))
			continue;
		vTaskDelay(kCen);
		if (!I2C_RandWrite(IOEXP0_ADDR,0x4F,1,ioexp0_conf, 1))
			continue;
		vTaskDelay(kCen);
		if (!I2C_RandWrite(IOEXP0_ADDR,0x03,1,ioexp0_conf + 2, 1))
			continue;
		vTaskDelay(kCen);
		if (!I2C_RandWrite(IOEXP0_ADDR,0x43,1,ioexp0_conf + 3, 1))
			continue;
		vTaskDelay(kCen);
		b = 0xFF;
		if (!I2C_RandWrite(IOEXP1_ADDR,0x42,1,&b, 1))	// input latch
			continue;
		b=0;
		if (!I2C_RandWrite(IOEXP1_ADDR,0x45,1,&b, 1))	// input latch
			continue;
		if (I2C_RandRead(IOEXP1_ADDR,0,1,&ioexp1_inp,1))			// read input
			break;
	}
	initok = TRUE;
	xSemaphoreGive(xSemaMutex);
	EXTI_ClearITPendingBit(DIG_IN_IRQn_EXTI_LINE);
	NVIC_EnableIRQ(DIG_IN_IRQn_EXTI_IRQn);
	for (;;)
	{
		xSemaphoreTake(xSemaDigin,portMAX_DELAY);
		I2C_RandRead(IOEXP1_ADDR,0,1,&ioexp1_inp,1);
	}
}

void IOEXP_init(void)
{
EXTI_InitTypeDef EXTI_InitStructure;

	if (xSemaDigin)
		return;

	xSemaMutex = xSemaphoreCreateMutex();

	vSemaphoreCreateBinary(xSemaDigin);
	xSemaphoreTake(xSemaDigin,0);		// clear it

	NVIC_DisableIRQ(DIG_IN_IRQn_EXTI_IRQn);

	/* Enable the Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	NVIC_SetPriority(DIG_IN_IRQn_EXTI_IRQn,DIG_IN_IRQn_EXTI_IRQ_PRIO);

	/* Configure IRQ pin as input floating */
	GpioInit(DIG_IN_IRQn_PORT,DIG_IN_IRQn_PIN,GPIO_Mode_IN_FLOATING,0);
	
	/* Connect IRQ EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(DIG_IN_IRQn_PORT_SOURCE, DIG_IN_IRQn_PIN_SOURCE);

	/* Configure CR95HF IRQ EXTI line */
	EXTI_InitStructure.EXTI_Line 		= DIG_IN_IRQn_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd 	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_ClearITPendingBit(DIG_IN_IRQn_EXTI_LINE);

	xTaskCreate(IOEXP_task,"digin",128, NULL,tskIDLE_PRIORITY,NULL);
}
